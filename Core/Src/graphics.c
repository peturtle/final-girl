#include "main.h"

#define LIGHT         1
#define DARK          0
#define SMALL         1
#define LARGE         0

static uint8_t *gfx_px(uint8_t x, uint8_t y)
{
  return frame_buffer[back] + (y >> 3) * OLED_WIDTH + x;
}

// fill entire back buffer white or black
static void gfx_clear(uint8_t value)
{
  uint8_t *p   = frame_buffer[back];
  uint8_t *end = p + OLED_FB_BYTES;

  while (p < end)
    *p++ = value;
}

// turn a single pixel on or off
static void gfx_pixel(uint8_t x, uint8_t y, uint8_t on)
{
  uint8_t mask;

  if (x >= OLED_WIDTH || y >= OLED_HEIGHT)
    return;                       // off the glass, drop it

  mask = 1 << (y & 7);            // y&7 == y%8, tells us which bit of the column
  if (on)
    *gfx_px(x, y) |= mask;
  else
    *gfx_px(x, y) &= ~mask;
}

// draws a rectangle with the option for it to be filled or empty
static void gfx_fill_rect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t on)
{
  uint8_t mask;
  uint8_t *row;

  if (x0 >= OLED_WIDTH || y0 >= OLED_HEIGHT) return; // top left corner is already off screen

  // clip so we never walk off the right / bottom edge
  if (x0 + w > OLED_WIDTH)
    w = OLED_WIDTH - x0;
  if (y0 + h > OLED_HEIGHT)
    h = OLED_HEIGHT - y0;

  for (uint8_t y = 0; y < h; y++)
  {
    mask = 1 << ((y0 + y) & 7);   
    row  = gfx_px(x0, y0 + y); 

    for (uint8_t x = 0; x < w; x++)
    {
      if (on)
        row[x] |= mask;
      else
        row[x] &= ~mask;
    }
  }
}

// draw text from our font5x7.c 
static void gfx_text(uint8_t x, uint8_t y, const char *text, uint8_t light, uint8_t small)
{
  const uint8_t *character;
  uint8_t col, row, w, h, adv;

  while (*text != 0)
  {
    if (small)
    {
      w = FONT_W_S;
      h = FONT_H_S;
      character = font4x6[*text++];
    }
    else
    {
      w = FONT_W_L;
      h = FONT_H_L;
      character = font5x7[*text++]; // column data for this char, then step the string
    }

    for (col = 0; col < w; col++)
    {
      if (x + col >= OLED_WIDTH) break; // ran off the right edge

      for (row = 0; row < h; row++)
      {
        if (y + row >= OLED_HEIGHT) break; // ran off the bottom edge

        if (light)
          gfx_pixel(x + col, y + row, character[col] & (1 << row)); // draw the pixel
        else
          gfx_pixel(x + col, y + row, !(character[col] & (1 << row))); // draw the pixel
      }
    }
    x += w+1;                // leaves room for the next character
  }
}
