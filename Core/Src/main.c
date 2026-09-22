#include "main.h"
#include "i2c1.c"            
#include "ssd1306.c"         
#include "oled_dma.c"        
#include "font5x7.c"
#include  "font4x6.c"         
#include "graphics.c"             
#include "input_controls.c"
#include "nav_helpers.c"
#include "scene.c"     
#include "static_components.c"

// arm-none-eabi-gcc is the compiler
// STM32_Programmer_CLI is the programmer executable

// basically just have to the compiler and the programmer executable in PATH
// for mac run "brew install --cask gcc-arm-embedded" to download the compiler. it adds to the path automatically on my mac
// download stm32 programmer and then find the STM32_Programmer_CLI file. then add it to ur path. then make flash from directory should work 

// CubeMX generated functions
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_ICACHE_Init(void);
// CubeMX generated functions

/// configures our gpio
void gpio_init(void)
{
  // Enable clk access to all 3 gpio banks-- A, B, and C
  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN);

  // GPIO C3 output heartbeat led
  GPIOC->MODER   = (GPIOC->MODER & ~(3UL << (2U * 3U))) | (1UL << (2U * 3U));
  GPIOC->OSPEEDR = (GPIOC->OSPEEDR & ~(3UL << (2U * 3U))) | (1UL << (2U * 3U));
  GPIOC->PUPDR   &= ~(3UL << (2U * 3U)); // clear for no pull up/down resistor
  GPIOC->ODR     &= ~(1UL << 3U);   // clear pin reset state.

  // GPIO A5 output debug led
  GPIOA->MODER   = (GPIOA->MODER & ~(3UL << (2U * 5U))) | (1UL << (2U * 5U));
  GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~(3UL << (2U * 5U))) | (1UL << (2U * 5U));
  GPIOA->PUPDR   &= ~(3UL << (2U * 5U)); // clear for no pull up/down resistor
  GPIOA->ODR     &= ~(1UL << 5U);   // clear pin reset state.

  // I2C1_SDA on PB10, alternate function 11
  GPIOB->MODER   = (GPIOB->MODER & ~(3UL << (2U * 10U))) | (2UL << (2U * 10U)); // Set pin to af mode
  GPIOB->OTYPER  |= (1UL << 10U);   // open-drain
  GPIOB->OSPEEDR &= ~(3UL << (2U * 10U));
  GPIOB->PUPDR   &= ~(3UL << (2U * 10U)); // no pull up or down
  GPIOB->AFR[1]  = (GPIOB->AFR[1] & ~(0xFUL << (4U * (10U - 8U)))) | (0xBUL << (4U * (10U - 8U)));  // Set to AF11 

  // I2C1_SCL on PC8, alternate function 4
  GPIOC->MODER   = (GPIOC->MODER & ~(3UL << (2U * 8U)))  | (2UL << (2U * 8U)); // Set pin to af mode
  GPIOC->OTYPER  |= (1UL << 8U); // open-drain
  GPIOC->OSPEEDR &= ~(3UL << (2U * 8U));
  GPIOC->PUPDR   &= ~(3UL << (2U * 8U)); // no pull up or down
  GPIOC->AFR[1]  = (GPIOC->AFR[1] & ~(0xFUL << (4U * (8U - 8U))))  | (0x4UL << (4U * (8U - 8U)));   // Set to AF4

  // encoder 1: A PC0, B PC1, switch PC2
  GPIOC->MODER   &= ~(3UL << (2U * 0U));  // input mode
  GPIOC->OSPEEDR &= ~(3UL << (2U * 0U));
  GPIOC->PUPDR    = (GPIOC->PUPDR & ~(3UL << (2U * 0U))) | (1UL << (2U * 0U)); // pull-up

  GPIOC->MODER   &= ~(3UL << (2U * 1U));  // input mode
  GPIOC->OSPEEDR &= ~(3UL << (2U * 1U));
  GPIOC->PUPDR    = (GPIOC->PUPDR & ~(3UL << (2U * 1U))) | (1UL << (2U * 1U)); // pull-up

  GPIOC->MODER   &= ~(3UL << (2U * 2U));  // input mode
  GPIOC->OSPEEDR &= ~(3UL << (2U * 2U));
  GPIOC->PUPDR    = (GPIOC->PUPDR & ~(3UL << (2U * 2U))) | (1UL << (2U * 2U)); // pull-up

  // encoder 2: A PC4, B PC5, switch PB0
  GPIOC->MODER   &= ~(3UL << (2U * 4U));  // input mode
  GPIOC->OSPEEDR &= ~(3UL << (2U * 4U));
  GPIOC->PUPDR    = (GPIOC->PUPDR & ~(3UL << (2U * 4U))) | (1UL << (2U * 4U)); // pull-up

  GPIOC->MODER   &= ~(3UL << (2U * 5U));  // input mode
  GPIOC->OSPEEDR &= ~(3UL << (2U * 5U));
  GPIOC->PUPDR    = (GPIOC->PUPDR & ~(3UL << (2U * 5U))) | (1UL << (2U * 5U)); // pull-up

  GPIOB->MODER   &= ~(3UL << (2U * 0U));  // input mode
  GPIOB->OSPEEDR &= ~(3UL << (2U * 0U));
  GPIOB->PUPDR    = (GPIOB->PUPDR & ~(3UL << (2U * 0U))) | (1UL << (2U * 0U)); // pull-up

  // encoder 3: A PB12, PB13, switch PB14
  GPIOB->MODER   &= ~(3UL << (2U * 12U));  // input mode
  GPIOB->OSPEEDR &= ~(3UL << (2U * 12U));
  GPIOB->PUPDR    = (GPIOB->PUPDR & ~(3UL << (2U * 12U))) | (1UL << (2U * 12U)); // pull-up

  GPIOB->MODER   &= ~(3UL << (2U * 13U));  // input mode
  GPIOB->OSPEEDR &= ~(3UL << (2U * 13U));
  GPIOB->PUPDR    = (GPIOB->PUPDR & ~(3UL << (2U * 13U))) | (1UL << (2U * 13U)); // pull-up

  GPIOB->MODER   &= ~(3UL << (2U * 14U));  // input mode
  GPIOB->OSPEEDR &= ~(3UL << (2U * 14U));
  GPIOB->PUPDR    = (GPIOB->PUPDR & ~(3UL << (2U * 14U))) | (1UL << (2U * 14U)); // pull-up

  // encoder 4: A PB15, B PC6, switch PC7
  GPIOB->MODER   &= ~(3UL << (2U * 15U));  // input mode
  GPIOB->OSPEEDR &= ~(3UL << (2U * 15U));
  GPIOB->PUPDR    = (GPIOB->PUPDR & ~(3UL << (2U * 15U))) | (1UL << (2U * 15U)); // pull-up

  GPIOC->MODER   &= ~(3UL << (2U * 6U));  // input mode
  GPIOC->OSPEEDR &= ~(3UL << (2U * 6U));
  GPIOC->PUPDR    = (GPIOC->PUPDR & ~(3UL << (2U * 6U))) | (1UL << (2U * 6U)); // pull-up

  GPIOC->MODER   &= ~(3UL << (2U * 7U));  // input mode
  GPIOC->OSPEEDR &= ~(3UL << (2U * 7U));
  GPIOC->PUPDR    = (GPIOC->PUPDR & ~(3UL << (2U * 7U))) | (1UL << (2U * 7U)); // pull-up

  // tact 1 PC9
  GPIOC->MODER   &= ~(3UL << (2U * 9U));  // input mode
  GPIOC->OSPEEDR &= ~(3UL << (2U * 9U));
  GPIOC->PUPDR    = (GPIOC->PUPDR & ~(3UL << (2U * 9U))) | (1UL << (2U * 9U)); // pull-up

  // tact 2 PA8
  GPIOA->MODER   &= ~(3UL << (2U * 8U));  // input mode
  GPIOA->OSPEEDR &= ~(3UL << (2U * 8U));
  GPIOA->PUPDR    = (GPIOA->PUPDR & ~(3UL << (2U * 8U))) | (1UL << (2U * 8U)); // pull-up

  // tact 3 PA9
  GPIOA->MODER   &= ~(3UL << (2U * 9U));  // input mode
  GPIOA->OSPEEDR &= ~(3UL << (2U * 9U));
  GPIOA->PUPDR    = (GPIOA->PUPDR & ~(3UL << (2U * 9U))) | (1UL << (2U * 9U)); // pull-up

  // tact 4 PA10
  GPIOA->MODER   &= ~(3UL << (2U * 10U));  // input mode
  GPIOA->OSPEEDR &= ~(3UL << (2U * 10U));
  GPIOA->PUPDR    = (GPIOA->PUPDR & ~(3UL << (2U * 10U))) | (1UL << (2U * 10U)); // pull-up
}

// sets up timer 6 which we use to trigger interupts that poll our inputs (encoders + buttons)
void tim6_init(void)
{
  RCC->APB1LENR |= RCC_APB1LENR_TIM6EN; // enable clk to TIM6
  TIM6->CR1  = 0U;           // disable control register during init

  TIM6->PSC  = 250U;         // set prescaler (timer counts => system_clk / prescaler.. if prescaler 1 timer counts as fast as the system clock)
  TIM6->ARR  = 999U;         // count to 0 to 999 => 1 kHz tick: heartbeat + input poll rate
  TIM6->DIER |= (1 << 0);    // enable update event interrupt

  NVIC_SetPriority(TIM6_IRQn, 6U); // add TIM6 interrupt with priority 6 to NVIC
  NVIC_EnableIRQ(TIM6_IRQn);       // enable TIM6 interrupt

  TIM6->CR1 = TIM_CR1_CEN;  // start timer
}

void TIM6_IRQHandler(void) // TIM6 isr triggered 1khz. 
{
  if ((TIM6->SR & TIM_SR_UIF) == 0U) return; // exit immediately if false trigger
  TIM6->SR = ~TIM_SR_UIF;    // clear the update interupt flag before handling interrupt 

  GPIOC->ODR ^= (1UL << 3U); // toggle PC3 
  inputs_poll();             // update input state 
}

int main(void)
{
  // HAL stuff
  MPU_Config();
  HAL_Init();
  HAL_NVIC_SetPriority(SysTick_IRQn, 0U, 0U);
  SystemClock_Config();
  MX_ICACHE_Init();

  gpio_init();                       // heartbeat/debug LEDs, I2C SDA/SCL, all the inputs
  inputs_init();                     // sets known value for inputs 
  tim6_init();                       // 1 kHz timer interrupt
  oled_init();                       // sets up i2c, dma, and oled
  while(oled_init_complete == 0U);   // wait for oled to finish initialization
  
  static_components();              // draws static background once into the back buffer
  while (1)
  {
    scene_tick();                    // render and push frames at 40 fps
  }
}

// Cubemx init stuff no touchy
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_CSI;
  RCC_OscInitStruct.CSIState = RCC_CSI_ON;
  RCC_OscInitStruct.CSICalibrationValue = RCC_CSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_CSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 125;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the programming delay
  */
  __HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_2);
}
static void MX_ICACHE_Init(void)
{
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
}
void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};
  MPU_Attributes_InitTypeDef MPU_AttributesInit = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region 0 and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x08FFF000;
  MPU_InitStruct.LimitAddress = 0x08FFFFFF;
  MPU_InitStruct.AttributesIndex = MPU_ATTRIBUTES_NUMBER0;
  MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RO;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Attribute 0 and the memory to be protected
  */
  MPU_AttributesInit.Number = MPU_ATTRIBUTES_NUMBER0;
  MPU_AttributesInit.Attributes = INNER_OUTER(MPU_NOT_CACHEABLE);

  HAL_MPU_ConfigMemoryAttributes(&MPU_AttributesInit);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */