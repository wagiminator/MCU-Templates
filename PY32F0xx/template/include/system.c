// ===================================================================================
// Basic System Functions for PY32F002, PY32F003, and PY32F030                * v0.1 *
// ===================================================================================
//
// This file must be included!!! The system configuration and the system clock are 
// set up automatically on system start.

#include "system.h"

// ===================================================================================
// Setup Microcontroller (this function is called automatically at startup)
// ===================================================================================
void SYS_init(void) {
  // Init system clock
  #if SYS_CLK_INIT > 0
  CLK_init();
  #endif

  // Init SYSTICK
  #if SYS_TICK_INIT > 0
  SysTick->CTRL = SysTick_CTRL_ENABLE | SysTick_CTRL_CLKSOURCE;
  #endif

  // Enable GPIO
  #if SYS_GPIO_EN > 0
  RCC->IOPENR = RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN | RCC_IOPENR_GPIOFEN;
  #endif
}

// ===================================================================================
// System Clock Functions
// ===================================================================================

// Init internal oscillator (non PLL) as system clock source
void CLK_init_HSI(void) {
  #if CLK_DIV > 0
  RCC->CR = CLK_DIV | RCC_RC_HSION;                       // set HSI divisor
  #endif
  RCC->ICSCR = (RCC->ICSCR & 0xFFFF0000) | CLK_MASK;      // set HSI freq and calibration
}

// Init internal oscillator with PLL as system clock source
void CLK_init_HSI_PLL(void) {
  RCC->ICSCR = (RCC->ICSCR & 0xFFFF0000) | CLK_MASK;      // set HSI freq and calibration
  //RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSI;                // set HSI as source for PLL
  RCC->CR |= RCC_CR_PLLON;                                // Enable PLL
  while(!(RCC->CR & RCC_CR_PLLRDY));                      // Wait till PLL is ready      
  RCC->CFGR = 0b010;                                      // PLL as system clock source
  while((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL); // Wait for PLL
}

// Init external crystal (non PLL) as system clock source
void CLK_init_HSE(void) {
  RCC->CR |= RCC_CR_HSEON;                                // Turn on HSE
  while(!(RCC->CR & RCC_CR_HSERDY));                      // wait till HSE is ready
  RCC->CFGR = 0b001;                                      // HSE as system clock source
  while((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_HSE); // Wait for HSE
}

// ===================================================================================
// Real-Time Clock (RTC) Functions
// ===================================================================================

// ===================================================================================
// Delay Functions
// ===================================================================================

// Wait n+1 counts of SysTick
void DLY_ticks(uint32_t n) {
  SysTick->LOAD = n;
  SysTick->VAL  = 0;
  while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG));
}

// ===================================================================================
// Independent Watchdog Timer (IWDG) Functions
// ===================================================================================

// ===================================================================================
// Sleep Functions
// ===================================================================================

// ===================================================================================
// C++ Support
// ===================================================================================
#ifdef __cplusplus
extern void __cxa_pure_virtual() { while (1); }
extern void (*__preinit_array_start[]) (void) __attribute__((weak));
extern void (*__preinit_array_end[]) (void) __attribute__((weak));
extern void (*__init_array_start[]) (void) __attribute__((weak));
extern void (*__init_array_end[]) (void) __attribute__((weak));

void __libc_init_array(void) {
  uint32_t count, i;
  count = __preinit_array_end - __preinit_array_start;
  for (i = 0; i < count; i++) __preinit_array_start[i]();
  count = __init_array_end - __init_array_start;
  for (i = 0; i < count; i++) __init_array_start[i]();
}
#endif

// ===================================================================================
// C version of PY32F030 Startup .s file
// ===================================================================================
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _data_lma;
extern uint32_t _data_vma;
extern uint32_t _edata;
extern void _eusrstack(void);

// Prototypes
int main(void)                __attribute__((section(".text.main"), used));
void (*const vectors[])(void) __attribute__((section(".vectors"), used));
void Default_Handler(void)    __attribute__((section(".text.irq_handler"), naked, used));
void Reset_Handler(void)      __attribute__((section(".text.irq_handler"), naked, used));

// All interrupt handlers are aliased to default_handler unless overridden individually
#define DUMMY_HANDLER __attribute__((section(".text.irq_handler"), weak, alias("Default_Handler"), used))
DUMMY_HANDLER void NMI_Handler(void);
DUMMY_HANDLER void HardFault_Handler(void);
DUMMY_HANDLER void SVC_Handler(void);
DUMMY_HANDLER void PendSV_Handler(void);
DUMMY_HANDLER void SysTick_Handler(void);
DUMMY_HANDLER void WWDG_IRQHandler(void);
DUMMY_HANDLER void PVD_IRQHandler(void);
DUMMY_HANDLER void RTC_IRQHandler(void);
DUMMY_HANDLER void FLASH_IRQHandler(void);
DUMMY_HANDLER void RCC_IRQHandler(void);
DUMMY_HANDLER void EXTI0_1_IRQHandler(void);
DUMMY_HANDLER void EXTI2_3_IRQHandler(void);
DUMMY_HANDLER void EXTI4_15_IRQHandler(void);
DUMMY_HANDLER void LCD_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel1_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel2_3_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel4_5_6_7_IRQHandler(void);
DUMMY_HANDLER void ADC_COMP_IRQHandler(void);
DUMMY_HANDLER void TIM1_BRK_UP_TRG_COM_IRQHandler(void);
DUMMY_HANDLER void TIM1_CC_IRQHandler(void);
DUMMY_HANDLER void TIM2_IRQHandler(void);
DUMMY_HANDLER void TIM3_IRQHandler(void);
DUMMY_HANDLER void LPTIM1_IRQHandler(void);
DUMMY_HANDLER void TIM7_IRQHandler(void);
DUMMY_HANDLER void TIM14_IRQHandler(void);
DUMMY_HANDLER void TIM15_IRQHandler(void);
DUMMY_HANDLER void TIM16_IRQHandler(void);
DUMMY_HANDLER void TIM17_IRQHandler(void);
DUMMY_HANDLER void I2C1_IRQHandler(void);
DUMMY_HANDLER void I2C2_IRQHandler(void);
DUMMY_HANDLER void SPI1_IRQHandler(void);
DUMMY_HANDLER void SPI2_IRQHandler(void);
DUMMY_HANDLER void USART1_IRQHandler(void);
DUMMY_HANDLER void USART2_IRQHandler(void);
DUMMY_HANDLER void USART3_4_IRQHandler(void);
DUMMY_HANDLER void LED_IRQHandler(void);
DUMMY_HANDLER void USB_IRQHandler(void);

// Interrupt vector table
void (*const vectors[])(void) = {
  &_eusrstack,                   //  0 - Initial Stack Pointer Value

  // Cortex-M0+ handlers
  Reset_Handler,                 //  1 - Reset
  NMI_Handler,                   //  2 - NMI
  HardFault_Handler,             //  3 - Hard Fault
  0,                             //  4 - Reserved
  0,                             //  5 - Reserved
  0,                             //  6 - Reserved
  0,                             //  7 - Reserved
  0,                             //  8 - Reserved
  0,                             //  9 - Reserved
  0,                             // 10 - Reserved
  SVC_Handler,                   // 11 - SVCall
  0,                             // 12 - Reserved
  0,                             // 13 - Reserved
  PendSV_Handler,                // 14 - PendSV
  SysTick_Handler,               // 15 - SysTick

  // Peripheral handlers
  WWDG_IRQHandler,               //  0 - Window Watchdog
  PVD_IRQHandler,                //  1 - PVD through EXTI Line detect
  RTC_IRQHandler,                //  2 - RTC through EXTI Line
  FLASH_IRQHandler,              //  3 - FLASH
  RCC_IRQHandler,                //  4 - RCC
  EXTI0_1_IRQHandler,            //  5 - EXTI Line 0 and 1
  EXTI2_3_IRQHandler,            //  6 - EXTI Line 2 and 3
  EXTI4_15_IRQHandler,           //  7 - EXTI Line 4 to 15
  LCD_IRQHandler,                //  8 - LCD
  DMA1_Channel1_IRQHandler,      //  9 - DMA1 Channel 1
  DMA1_Channel2_3_IRQHandler,    // 10 - DMA1 Channel 2 and 3
  DMA1_Channel4_5_6_7_IRQHandler,// 11 - DMA1 Channel 4 to 7
  ADC_COMP_IRQHandler,           // 12 - ADC&COMP1
  TIM1_BRK_UP_TRG_COM_IRQHandler,// 13 - TIM1 Break, Update, Trigger and Commutation
  TIM1_CC_IRQHandler,            // 14 - TIM1 Capture Compare
  TIM2_IRQHandler,               // 15 - TIM2
  TIM3_IRQHandler,               // 16 - TIM3
  LPTIM1_IRQHandler,             // 17 - LPTIM1
  TIM7_IRQHandler,               // 18 - TIM7
  TIM14_IRQHandler,              // 19 - TIM14
  TIM15_IRQHandler,              // 20 - TIM15
  TIM16_IRQHandler,              // 21 - TIM16
  TIM17_IRQHandler,              // 22 - TIM17
  I2C1_IRQHandler,               // 23 - I2C1
  I2C2_IRQHandler,               // 24 - I2C2
  SPI1_IRQHandler,               // 25 - SPI1
  SPI2_IRQHandler,               // 26 - SPI2
  USART1_IRQHandler,             // 27 - USART1
  USART2_IRQHandler,             // 28 - USART2
  USART3_4_IRQHandler,           // 29 - USART3, USART4
  LED_IRQHandler,                // 30 - LED
  USB_IRQHandler,                // 31 - USB
};

// Unless a specific handler is overridden, it just spins forever
void Default_Handler(void) { while(1); }

// Reset handler
void Reset_Handler(void) {
  uint32_t *src, *dst;

  // Configure vector table location
  SCB->VTOR = (uint32_t)vectors;

  // Copy data from FLASH to RAM
  src = &_data_lma;
  dst = &_data_vma;
  while(dst < &_edata) *dst++ = *src++;

  // Clear uninitialized variables
  dst = &_sbss;
  while(dst < &_ebss) *dst++ = 0;

  // C++ Support
  #ifdef __cplusplus
  __libc_init_array();
  #endif

  // Init system
  SYS_init();

  // Start main function
  main();
  while(1);
}