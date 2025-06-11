// ===================================================================================
// Basic System Functions for STM32F030 and STM32F031                         * v0.2 *
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
  #if F_CPU > 24000000
  FLASH->ACR |= FLASH_ACR_LATENCY;          // FLASH latency: one wait state
  #endif
  CLK_init();                               // Init clock according to settings
  #endif

  // Init SYSTICK
  #if SYS_TICK_INIT > 0
  SysTick->CTRL = SysTick_CTRL_ENABLE | SysTick_CTRL_CLKSOURCE;
  #endif

  // Enable GPIO A-C
  #if SYS_GPIO_EN > 0
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN;
  #endif
}

// ===================================================================================
// System Clock Functions
// ===================================================================================

// Init internal oscillator (non PLL) as system clock source
void CLK_init_HSI(void) {
  RCC->CFGR = CLK_DIV;                                    // set clock divider
}

// Init internal oscillator with PLL as system clock source
void CLK_init_HSI_PLL(void) {
  RCC->CFGR = CLK_PLL_MUL | CLK_DIV;                      // set PLL factor and divider
  RCC->CR  |= RCC_CR_PLLON;                               // enable PLL
  while(!(RCC->CR & RCC_CR_PLLRDY));                      // wait for PLL ready    
  RCC->CFGR |= 0b10;                                      // PLL as system clock source
  while((RCC->CFGR & RCC_CFGR_SWS_Msk) != (0b10<<2));     // wait until switched to PLL
}

// Init external crystal (non PLL) as system clock source
void CLK_init_HSE(void) {
  RCC->CR |= RCC_CR_HSEON;                                // enable HSE
  while(!(RCC->CR & RCC_CR_HSERDY));                      // wait for HSE ready
  RCC->CFGR = 0b01;                                       // HSE as system clock source
  while((RCC->CFGR & RCC_CFGR_SWS_Msk) != (0b01<<2));     // wait until switched to HSE
}

// Init external crystal with PLL as system clock source
void CLK_init_HSE_PLL(void) {
  RCC->CR |= RCC_CR_HSEON;                                // enable HSE
  while(!(RCC->CR & RCC_CR_HSERDY));                      // wait for HSE ready
  RCC->CFGR = CLK_PLL_MUL | CLK_DIV                       // set PLL factor and divider
            | RCC_CFGR_PLLSRC;                            // set HSE as PLL source
  RCC->CR  |= RCC_CR_PLLON;                               // enable PLL
  while(!(RCC->CR & RCC_CR_PLLRDY));                      // wait for PLL ready    
  RCC->CFGR |= 0b10;                                      // PLL as system clock source
  while((RCC->CFGR & RCC_CFGR_SWS_Msk) != (0b10<<2));     // wait until switched to PLL
}

// Reset system clock to default state
void CLK_reset(void) {
  RCC->CR |= RCC_CR_HSION;                                // enable HSI
  while(!(RCC->CR & RCC_CR_HSIRDY));                      // wait for HSI ready
  RCC->CFGR   = 0x00000000;                               // HSI as system clock source
  while(RCC->CFGR & RCC_CFGR_SWS_Msk);                    // wait until switched to HSI
  RCC->CR     = 0x00000081;                               // keep only HSI on
  RCC->CIR    = 0x00ff0000;                               // disable all interrupts
  FLASH->ACR &= 0xfffffffe;                               // reset flash latency
}

// ===================================================================================
// Real-Time Clock (RTC) Functions
// ===================================================================================

// Init RTC with LSI as clock source and 1s clock period
void RTC_init(void) {
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;    // enable low power control block clock
  PWR->CR   |= PWR_CR_DBP;              // enable access to RTC domain
  RCC->CSR  |= RCC_CSR_LSION;           // enable internal low-speed clock (LSI)
  RCC->BDCR |= (0b10<<8)                // set LSI as clock source
            | RCC_BDCR_RTCEN;           // enable RTC module clock
  RCC->APB1ENR &=~ RCC_APB1ENR_PWREN;   // disable low power control block clock
  RTC->WPR = 0xCA;                      // remove RTC register write protection
  RTC->WPR = 0x53;
  RTC_setPrescaler(63, 624);            // set prescaler (40kHz -> 1Hz)
}

// Init RTC with LSE as clock source and 1s clock period
void RTC_init_LSE(void) {
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;    // enable low power control block clock
  PWR->CR  |= PWR_CR_DBP;               // enable access to RTC domain
  RCC->BDCR |= RCC_BDCR_LSEON           // enable external low-speed clock (LSE)
            | (0b01<<8)                 // set LSE as clock source
            | RCC_BDCR_RTCEN;           // enable RTC module clock
  RCC->APB1ENR &=~ RCC_APB1ENR_PWREN;   // disable low power control block clock
  RTC->WPR = 0xCA;                      // remove RTC register write protection
  RTC->WPR = 0x53;
}

// Set RTC prescaler (default 127, 255)
void RTC_setPrescaler(uint8_t async, uint16_t sync) {
  RTC->ISR |= RTC_ISR_INIT;             // enter initialization mode
  while(!(RTC->ISR & RTC_ISR_INITF));   // wait until init mode is entered
  RTC->PRER = ((uint32_t)async << 16)
            | sync;
  RTC->ISR &= ~RTC_ISR_INIT;            // exit initialization mode
}

// Set time in BC format
void RTC_setTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
  RTC->ISR |= RTC_ISR_INIT;             // enter initialization mode
  while(!(RTC->ISR & RTC_ISR_INITF));   // wait until init mode is entered
  RTC->TR = (uint32_t)hours   << 16     // set hours in BCD format
          | (uint32_t)minutes <<  8     // set minutes in BCD format
          | (uint32_t)seconds <<  0;    // set seconds in BCD format
  RTC->CR |= RTC_CR_FMT;                // set time format to 24 hours
  RTC->ISR &= ~RTC_ISR_INIT;            // exit initialization mode
}

// Set Date in BCD format
void RTC_setDate(uint8_t year, uint8_t weekday, uint8_t month, uint8_t day) {
  RTC->ISR |= RTC_ISR_INIT;             // enter initialization mode
  while(!(RTC->ISR & RTC_ISR_INITF));   // wait until init mode is entered
  RTC->DR = (uint32_t)year    << 16     // set year in BCD format
          | (uint32_t)weekday << 13     // set day of the week (1:Monday - 7:Sunday)
          | (uint32_t)month   <<  8     // set month in BCD format
          | (uint32_t)day     <<  0;    // set day in BCD format
  RTC->ISR &= ~RTC_ISR_INIT;            // exit initialization mode
}

// Read time register
uint32_t RTC_readTime(void) {
  while(!(RTC->ISR & RTC_ISR_RSF));     // wait for registers synchronized
  RTC->ISR &= ~RTC_ISR_RSF;             // clear synchronization flag
  return RTC->TR;
}

// Read date register
uint32_t RTC_readDate(void) {
  while(!(RTC->ISR & RTC_ISR_RSF));     // wait for registers synchronized
  RTC->ISR &= ~RTC_ISR_RSF;             // clear synchronization flag
  return RTC->DR;
}

// Set Alarm A
void RTC_setAlarmA(uint8_t weekday, uint8_t hours, uint8_t minutes, uint8_t seconds) {
  RTC->CR &= ~RTC_CR_ALRAE;             // disable Alarm A
  while(!(RTC->ISR & RTC_ISR_ALRAWF));  // wait for write access
  RTC->ALRMASSR = 0;                    // no sub second comparison
  RTC->ALRMAR = (RTC->ALRMAR & 0xc0808080)  // keep alarm flags
              | (uint32_t)weekday << 24 // set day of the week (1:Monday - 7:Sunday)
              | (uint32_t)hours   << 16 // set hours in BCD format
              | (uint32_t)minutes <<  8 // set minutes in BCD format
              | (uint32_t)seconds <<  0 // set seconds in BCD format
              | RTC_ALRMAR_WDSEL;       // select week day mode
  RTC->ISR   &= ~RTC_ISR_ALRAF;         // clear alarm flag
  RTC->CR    |=  RTC_CR_ALRAE;          // enable Alarm A
}

// Start wakeup timer
void RTC_startWakeupTimer(uint16_t ms) {
  RTC->CR &= ~RTC_CR_WUTE;              // disable wakeup timer
  while(!(RTC->ISR & RTC_ISR_WUTWF));   // wait to gain access
  RTC->WUTR = ms << 1;                  // set timer (base 32kHz / 16 = 2kHz)
  RTC->ISR &= RTC_ISR_WUTF;             // clear wakup timer flag
  RTC->CR  |= RTC_CR_WUTE;              // enable wakeup timer
}

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

// Start independent watchdog timer (IWDG) with given time in milliseconds (max 8191).
// Once the IWDG has been started, it cannot be disabled, only reloaded (feed).
// It can be stopped by disabling the internal low-speed clock (LSI).
void IWDG_start(uint16_t ms) {
  LSI_enable();                         // enable internal low-speed clock (LSI)
  IWDG->KR   = 0xCCCC;                  // enable IWDG
  IWDG->KR   = 0x5555;                  // allow register modification
  IWDG->PR   = 0b100;                   // set LSI clock prescaler 64 (~0.5kHz)
  IWDG->RLR  = ms >> 1;                 // set watchdog counter reload value
  while(IWDG->SR);                      // wait for register to be updated
  IWDG->KR   = 0xAAAA;                  // load reload value into watchdog counter
}

// Reload watchdog counter with n milliseconds, n<=8191
void IWDG_reload(uint16_t ms) {
  IWDG->KR   = 0x5555;                  // allow register modification
  IWDG->RLR = ms >> 1;                  // set watchdog counter reload value
  while(IWDG->SR);                      // wait for register to be updated
  IWDG->KR   = 0xAAAA;                  // load reload value into watchdog counter
}

// ===================================================================================
// Sleep Functions
// ===================================================================================

// Put device into SLEEP, wake up by interrupt
void SLEEP_WFI_now(void) {
  __WFI();                              // wait for interrupt
}

// Put device into SLEEP, wake up by event
void SLEEP_WFE_now(void) {
  __WFE();                              // wait for event
}

// Put device into STOP, wake up interrupt
void STOP_WFI_now(void) {
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;    // enable low power control block clock
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;    // set deep sleep mode
  NVIC->ICPR[0] = 0xffffffff;           // clear pending interrupts
  PWR->CR = (PWR->CR & ~PWR_CR_PDDS)    // select STOP mode
          | PWR_CR_LPDS;                // select the voltage regulator mode
  __WFI();                              // wait for interrupt
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;   // unset deep sleep mode
  RCC->APB1ENR &=~ RCC_APB1ENR_PWREN;   // disable low power control block clock
}

// Put device into STOP, wake up event
void STOP_WFE_now(void) {
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;    // enable low power control block clock
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;    // set deep sleep mode
  NVIC->ICPR[0] = 0xffffffff;           // clear pending interrupts
  EXTI->PR = 0xffffffff;                // clear all EXTI pending flags
  PWR->CR = (PWR->CR & ~PWR_CR_PDDS)    // select STOP mode
          | PWR_CR_LPDS;                // select the voltage regulator mode
  __WFE();                              // wait for event
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;   // unset deep sleep mode
  RCC->APB1ENR &=~ RCC_APB1ENR_PWREN;   // disable low power control block clock
}

// Put device into STANDBY, wake up interrupt
void STDBY_WFI_now(void) {
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;    // enable low power control block clock
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;    // set deep sleep mode
  NVIC->ICPR[0] = 0xffffffff;           // clear pending interrupts
  PWR->CR |= PWR_CR_PDDS                // select STANDBY mode
           | PWR_CR_CWUF;               // clear wakup flag
  __WFI();                              // wait for interrupt
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;   // unset deep sleep mode
  RCC->APB1ENR &=~ RCC_APB1ENR_PWREN;   // disable low power control block clock
}

// Put device into STANDBY, wake up event
void STDBY_WFE_now(void) {
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;    // enable low power control block clock
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;    // set deep sleep mode
  NVIC->ICPR[0] = 0xffffffff;           // clear pending interrupts
  EXTI->PR = 0xffffffff;                // clear all EXTI pending flags
  PWR->CR |= PWR_CR_PDDS                // select STANDBY mode
           | PWR_CR_CWUF;               // clear wakup flag
  __WFE();                              // wait for event
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;   // unset deep sleep mode
  RCC->APB1ENR &=~ RCC_APB1ENR_PWREN;   // disable low power control block clock
}

// ===================================================================================
// Bootloader Functions
// ===================================================================================
void BOOT_now(void) {
  void (*SysMemBootJump)(void);
  __disable_irq();                      // disable all interrupts
  SysTick->CTRL = 0;                    // disable SysTick Timer
  CLK_reset();                          // reset system clock settings
  NVIC->ICER[0]=0xffffffff;             // clear interrupt enable register
  NVIC->ICPR[0]=0xffffffff;             // clear interrupt pending register
  __enable_irq();                       // re-enable all interrupts
  SysMemBootJump = (void (*)(void)) (*((uint32_t *) ((BOOT_ADDR + 4))));
  __set_MSP(*(uint32_t *)BOOT_ADDR);    // set main stack pointer for bootloader
  SysMemBootJump();                     // jump to bootloader
  while(1);                             // just be sure
}

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
// C version of STM32F031x6.s Startup File
// ===================================================================================
extern uint32_t _sbss, _ebss, _sdata, _edata, _sidata;
extern void _estack(void);

// Prototypes
int main(void)                __attribute__((section(".text.main"), used));
void (*const vectors[])(void) __attribute__((section(".isr_vector"), used));
void Reset_Handler(void)      __attribute__((section(".text.irq_handler"), naked, used, noreturn));

#if SYS_USE_VECTORS > 0
// Unless a specific handler is overridden, it just spins forever
void Default_Handler(void)    __attribute__((section(".text.irq_handler"), naked, used));
void Default_Handler(void)    { while(1); }

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
DUMMY_HANDLER void DMA1_Channel1_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel2_3_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel4_5_IRQHandler(void);
DUMMY_HANDLER void ADC1_IRQHandler(void);
DUMMY_HANDLER void TIM1_BRK_UP_TRG_COM_IRQHandler(void);
DUMMY_HANDLER void TIM1_CC_IRQHandler(void);
DUMMY_HANDLER void TIM2_IRQHandler(void);
DUMMY_HANDLER void TIM3_IRQHandler(void);
DUMMY_HANDLER void TIM6_IRQHandler(void);
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
DUMMY_HANDLER void USART3_6_IRQHandler(void);
#endif  // SYS_USE_VECTORS > 0

// Interrupt vector table
void (*const vectors[])(void) = {
  _estack,                        //  0 - Initial Stack Pointer Value

  // Cortex-M0+ handlers
  Reset_Handler,                  //  1 - Reset
  #if SYS_USE_VECTORS > 0
  NMI_Handler,                    //  2 - NMI
  HardFault_Handler,              //  3 - Hard Fault
  0,                              //  4 - Reserved
  0,                              //  5 - Reserved
  0,                              //  6 - Reserved
  0,                              //  7 - Reserved
  0,                              //  8 - Reserved
  0,                              //  9 - Reserved
  0,                              // 10 - Reserved
  SVC_Handler,                    // 11 - SVCall
  0,                              // 12 - Reserved
  0,                              // 13 - Reserved
  PendSV_Handler,                 // 14 - PendSV
  SysTick_Handler,                // 15 - SysTick

  // Peripheral handlers
  WWDG_IRQHandler,                //  0 - Window Watchdog
  PVD_IRQHandler,                 //  1 - PVD through EXTI Line detect
  RTC_IRQHandler,                 //  2 - RTC through EXTI Line
  FLASH_IRQHandler,               //  3 - FLASH
  RCC_IRQHandler,                 //  4 - RCC
  EXTI0_1_IRQHandler,             //  5 - EXTI Line 0 and 1
  EXTI2_3_IRQHandler,             //  6 - EXTI Line 2 and 3
  EXTI4_15_IRQHandler,            //  7 - EXTI Line 4 to 15
  0,                              //  8 - Reserved
  DMA1_Channel1_IRQHandler,       //  9 - DMA1 Channel 1
  DMA1_Channel2_3_IRQHandler,     // 10 - DMA1 Channel 2 and 3
  DMA1_Channel4_5_IRQHandler,     // 11 - DMA1 Channel 4, 5
  ADC1_IRQHandler,                // 12 - ADC1
  TIM1_BRK_UP_TRG_COM_IRQHandler, // 13 - TIM1 Break, Update, Trigger and Commutation
  TIM1_CC_IRQHandler,             // 14 - TIM1 Capture Compare
  TIM2_IRQHandler,                // 15 - TIM2
  TIM3_IRQHandler,                // 16 - TIM3
  TIM6_IRQHandler,                // 17 - TIM6
  TIM7_IRQHandler,                // 18 - TIM7
  TIM14_IRQHandler,               // 19 - TIM14
  TIM15_IRQHandler,               // 20 - TIM15
  TIM16_IRQHandler,               // 21 - TIM16
  TIM17_IRQHandler,               // 22 - TIM17
  I2C1_IRQHandler,                // 23 - I2C1
  I2C2_IRQHandler,                // 24 - I2C2
  SPI1_IRQHandler,                // 25 - SPI1
  SPI2_IRQHandler,                // 26 - SPI2
  USART1_IRQHandler,              // 27 - USART1
  USART2_IRQHandler,              // 28 - USART2
  USART3_6_IRQHandler,            // 29 - USART3 to 6
  0,                              // 30 - Reserved
  0,                              // 31 - Reserved
  #endif  // SYS_USE_VECTORS > 0
};

// Reset handler
void Reset_Handler(void) {
  uint32_t *src, *dst;

  // Set stack pointer
  asm volatile(
  " ldr r0, =_estack  \n\
    mov sp, r0        \n"
  : : : "r0", "memory");

  // Init system
  SYS_init();

  // Copy data from FLASH to RAM
  src = &_sidata;
  dst = &_sdata;
  while(dst < &_edata) *dst++ = *src++;

  // Clear uninitialized variables
  #if SYS_CLEAR_BSS > 0
  dst = &_sbss;
  while(dst < &_ebss) *dst++ = 0;
  #endif

  // C++ Support
  #ifdef __cplusplus
  __libc_init_array();
  #endif

  // Start main function
  main();
  while(1);
}
