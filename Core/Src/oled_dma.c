#include "main.h"
#include <string.h>

// panel + frame geometry (128x64 = 8 pages of 8 rows)
#define OLED_WIDTH    128
#define OLED_HEIGHT   64
#define OLED_PAGES    8
#define OLED_FB_BYTES ((OLED_WIDTH * OLED_HEIGHT) / 8U)  // 1024 bytes per full frame
#define DISPLAY_AREA_HEIGHT 47


// DMA config defines
#define OLED_DMA_CHANNEL           GPDMA1_Channel7         // oled i2c dma gets channel 7
#define OLED_DMA_TXIS_REQUEST_CODE 13U                    // GPDMA1 hw request value

static uint8_t frame_buffer[2][OLED_FB_BYTES]; // double frame buffer
static uint8_t front = 1U;    // buffer being pushed
static uint8_t back  = 0U;   // buffer we render the next frame into
static uint8_t oled_init_complete = 0U; // sorta not needed but not terrible practice 

// dma staging buffers
static uint8_t position_msg[5];             // position message: [0x00][0xB0|page][0x21][0x00][0x7F]
static uint8_t data_msg[1U + OLED_WIDTH];  // data message:     [0x40][128 data bytes]

// dma pump state values
static uint8_t  pump_busy;         // 1 = a push is in flight
static uint8_t  pump_first;       // first page of the current push
static uint8_t  pump_last;       // last page of the current push
static uint8_t  pump_current;   // which message of the push is in flight

// stock dma init.
static void oled_dma_init(void)
{
  __HAL_RCC_GPDMA1_CLK_ENABLE(); // enable clk to dma peripheral

  // dma channel config
  OLED_DMA_CHANNEL->CCR   = 0U;                                             // clear the control register
  OLED_DMA_CHANNEL->CTR1  |= DMA_CTR1_SINC;                                 // set increment src address bit
  OLED_DMA_CHANNEL->CTR2  |= OLED_DMA_TXIS_REQUEST_CODE;                    // set dma request code to I2C1_TX
  OLED_DMA_CHANNEL->CTR2  |= DMA_CTR2_DREQ;                                 // set destination request bit (we are sending data to i2c1_tx)
}

// configures the dma with the data and length for the next transfer
static void dma_send(const uint8_t *buf, uint16_t len)
{
  OLED_DMA_CHANNEL->CCR  &= ~DMA_CCR_EN;              // stop the channel before reconfiguring it
  OLED_DMA_CHANNEL->CFCR  = (0xFFFF);                 // clear all channel flags

  OLED_DMA_CHANNEL->CSAR  = (uint32_t)buf;            // set source address
  OLED_DMA_CHANNEL->CDAR  = (uint32_t)&I2C1->TXDR;    // set destination address
  OLED_DMA_CHANNEL->CBR1  = (uint32_t)len;            // set length of message
  OLED_DMA_CHANNEL->CCR  |= DMA_CCR_EN;               // enable channel

  I2C1->CR1 |= I2C_CR1_TXDMAEN | I2C_CR1_STOPIE;      // enable i2c interrupt for DMA TX and STOP condition
  i2c1_cr2_start(oled_address, len);                  // configures and starts i2c tranfer
}

// position message: park the panel's write pointer at the top of one page
static void arm_page(uint8_t page)
{
  position_msg[0] = OLED_COMMAND_BYTE;
  position_msg[1] = 0xB0U | page;                // page-select command (with the select page masked into the lower 4 bits)
  position_msg[2] = 0x21U;                       // set column address command
  position_msg[3] = 0U;                          // column start = 0
  position_msg[4] = (uint8_t)(OLED_WIDTH - 1U);  // column end   = 127

  dma_send(position_msg, sizeof(position_msg));
}

// data message: one page's worth of pixels, copied out of the front buffer
static void arm_data(uint8_t page)
{
  data_msg[0] = OLED_DATA_BYTE;
  const uint8_t *tx = &frame_buffer[front][page * OLED_WIDTH]; // grab page of data from the front buffer

  for (int k = 0; k < OLED_WIDTH; k++) // place it in buffer with leading data byte control code (0x40)
    data_msg[k + 1] = tx[k];

  dma_send(data_msg, sizeof(data_msg));
}

// how many i2c messages this push is made of (one position + one data per page)
static uint8_t pump_total(void)
{
  return (uint8_t)((pump_last - pump_first + 1U) * 2U);
}

// arm whatever message pump_current currently points at
static void pump_arm_msg(void)
{
  if ((pump_current & 1U) == 0U) // evens are page position, odds are data messages
    arm_page(pump_first + (pump_current >> 1U));
  else
    arm_data(pump_first + (pump_current >> 1U));
}

// a message finished: chain the next one, or close the push out
static void pump_advance(void)
{
  pump_current++;

  if (pump_current < pump_total())                       // more to go: send the next message
  {
    pump_arm_msg();
    return;
  }

  I2C1->CR1 &= ~(I2C_CR1_TXDMAEN | I2C_CR1_STOPIE);  // push done bus interrupts off
  pump_busy = 0U; // pump is done
}

static void oled_init(void)
{
  i2c1_bus_init();                       // i2c init
  oled_dma_init();                       // DMA channel + EV interrupt

  oled_address = oled_probe();           // initialized oled address which kicks off every i2c transaction
  oled_hw_init();                        // runs init sequence for oled
  oled_init_complete = 1U;
}

static void oled_push(void)
{
  if (pump_busy) return; // pump is already busy drop this frame

  // init for finding the first and last changed pages
  uint8_t first = 8U;
  uint8_t last  = 0U;

  for (uint8_t page = 0U; page < OLED_PAGES; page++)
  {
    const uint8_t *a = &frame_buffer[back][page * OLED_WIDTH];  // back buffer
    const uint8_t *b = &frame_buffer[front][page * OLED_WIDTH]; // front buffer
    if (memcmp(a, b, OLED_WIDTH) != 0) // do we have a difference on this page
    {
      if (page < first) first = page; // if this page is before the current first changed page
      if (page > last)  last  = page; // if this page is after the current last changed page
    }
  }

  if (first > last) return; // nothing to send

  front ^= 1U;  // flip
  back ^= 1U;

  // arming pump
  pump_first     = first;
  pump_last      = last;
  pump_current   = 0U;
  pump_busy      = 1U;

  // arm the first message; the ISR chains every message after it
  pump_arm_msg();
}

// each finished i2c message triggers this interrupt handler
void I2C1_EV_IRQHandler(void)
{
  I2C1->ICR = I2C_ICR_STOPCF;               // clear the stop flag

  pump_advance();                           // next dma pump
}