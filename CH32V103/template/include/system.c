// ===================================================================================
// Basic System Functions for CH32V103                                        * v1.2 *
// ===================================================================================
//
// This file must be included!!!!
//
// References:
// -----------
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

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
  STK_init();
  #endif

  // Enable GPIO
  #if SYS_GPIO_EN > 0
  RCC->APB2PCENR |= RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_IOPDEN;
  #endif
}

// ===================================================================================
// System Clock Functions
// ===================================================================================

// Init internal oscillator (non PLL) as system clock source
void CLK_init_HSI(void) {
  RCC->INTR  = 0x009F0000;                                // Clear ready flags
  RCC->CFGR0 = RCC_HPRE_DIV1                              // HCLK = SYSCLK
             | RCC_PPRE2_DIV1                             // PCLK2 = HCLK
             | RCC_PPRE1_DIV1                             // PCLK1 = HCLK
             | RCC_SW_HSI;                                // HSI as system clock source
  while((RCC->CFGR0 & RCC_SWS));                          // Wait for HSI
  FLASH->ACTLR = FLASH_ACTLR_PRFTBE|FLASH_ACTLR_LATENCY_0;// Enable prefetch buffer, no delay
}

// Init internal oscillator with PLL as system clock source
void CLK_init_HSI_PLL(void) {
  #if F_CPU <= 24000000
  FLASH->ACTLR = FLASH_ACTLR_PRFTBE|FLASH_ACTLR_LATENCY_0;// Enable prefetch buffer, no delay
  #elif F_CPU <= 48000000
  FLASH->ACTLR = FLASH_ACTLR_PRFTBE|FLASH_ACTLR_LATENCY_1;// Enable prefetch buffer, delay 1
  #else
  FLASH->ACTLR = FLASH_ACTLR_PRFTBE|FLASH_ACTLR_LATENCY_2;// Enable prefetch buffer, delay 2
  #endif

  RCC->INTR = 0x009F0000;                                 // Clear ready flags
  EXTEN->EXTEN_CTR |= EXTEN_PLL_HSI_PRE;                  // HSI not divided for PLL
  FLASH->ACTLR |= FLASH_ACTLR_PRFTBE;                     // Enable prefetch buffer
  RCC->CFGR0 = RCC_HPRE_DIV1                              // HCLK = SYSCLK
             | RCC_PPRE2_DIV1                             // PCLK2 = HCLK
             | RCC_PPRE1_DIV1                             // PCLK1 = HCLK
             | RCC_PLLSRC_HSI_Div2                        // HSI for PLL entry
             | CLK_MUL;                                   // PLL multiplier
  RCC->CTLR |= RCC_PLLON;                                 // Enable PLL
  while(!(RCC->CTLR & RCC_PLLRDY));                       // Wait till PLL is ready      
  RCC->CFGR0 |= RCC_SW_PLL;                               // PLL as system clock source
  while((RCC->CFGR0 & RCC_SWS) != (uint32_t)0x08);        // Wait for PLL
}

// Init external crystal (non PLL) as system clock source
void CLK_init_HSE(void) {
  #if F_CPU <= 24000000
  FLASH->ACTLR = FLASH_ACTLR_PRFTBE|FLASH_ACTLR_LATENCY_0;// Enable prefetch buffer, no delay
  #elif F_CPU <= 48000000
  FLASH->ACTLR = FLASH_ACTLR_PRFTBE|FLASH_ACTLR_LATENCY_1;// Enable prefetch buffer, delay 1
  #else
  FLASH->ACTLR = FLASH_ACTLR_PRFTBE|FLASH_ACTLR_LATENCY_2;// Enable prefetch buffer, delay 2
  #endif

  RCC->INTR    = 0x009F0000;                              // Clear ready flags
  RCC->CTLR   |= RCC_HSEON;                               // Turn on HSE
  while(!(RCC->CTLR & RCC_HSERDY));                       // wait till HSE is ready
  RCC->CFGR0   = RCC_HPRE_DIV1                            // HCLK = SYSCLK
               | RCC_PPRE2_DIV1                           // PCLK2 = HCLK
               | RCC_PPRE1_DIV1                           // PCLK1 = HCLK
               | RCC_SW_HSE;                              // HSE as system clock source
  while((RCC->CFGR0 & RCC_SWS) != (uint32_t)0x04);        // Wait for HSE
}

// Init external oscillator with PLL as system clock source
void CLK_init_HSE_PLL(void) {
  #if F_CPU <= 24000000
  FLASH->ACTLR = FLASH_ACTLR_PRFTBE|FLASH_ACTLR_LATENCY_0;// Enable prefetch buffer, no delay
  #elif F_CPU <= 48000000
  FLASH->ACTLR = FLASH_ACTLR_PRFTBE|FLASH_ACTLR_LATENCY_1;// Enable prefetch buffer, delay 1
  #else
  FLASH->ACTLR = FLASH_ACTLR_PRFTBE|FLASH_ACTLR_LATENCY_2;// Enable prefetch buffer, delay 2
  #endif

  RCC->APB2PCENR |= RCC_AFIOEN;                           // enable auxiliary clock module
  AFIO->PCFR1 |= AFIO_PCFR1_PD01_REMAP;                   // Use PD0/PD1 for crystal
  RCC->INTR    = 0x009F0000;                              // Clear ready flags
  RCC->CTLR   |= RCC_HSEON;                               // Turn on HSE
  while(!(RCC->CTLR & RCC_HSERDY));                       // wait till HSE is ready
  RCC->CFGR0   = RCC_HPRE_DIV1                            // HCLK = SYSCLK
               | RCC_PPRE2_DIV1                           // PCLK2 = HCLK
               | RCC_PPRE1_DIV1                           // PCLK1 = HCLK
               | RCC_PLLSRC_HSE                           // HSE for PLL entry
               | RCC_PLLXTPRE_HSE                         // HSE not divided for PLL entry
               | CLK_MUL;                                 // PLL multiplier
  RCC->CTLR    |= RCC_PLLON;                              // Enable PLL
  while(!(RCC->CTLR & RCC_PLLRDY));                       // Wait till PLL is ready      
  RCC->CFGR0   |= RCC_SW_PLL;                             // PLL as system clock source
  while((RCC->CFGR0 & RCC_SWS) != (uint32_t)0x08);        // Wait for PLL
}

// Setup pin PA8 for MCO (output, push-pull, 50MHz, auxiliary)
void MCO_init(void) {
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPAEN;
  GPIOA->CFGHR    = (GPIOA->CFGHR & ~((uint32_t)0b1111<<(0<<2))) | ((uint32_t)0b1011<<(0<<2));
}

// ===================================================================================
// Real-Time Clock (RTC) Functions
// ===================================================================================

// Init RTC with internal low-speed clock
void RTC_init_LSI(void) {
  //PWR->CTLR |= PWR_CTLR_DBP;          // enable access to backup domain
  //RCC->APB1PCENR |= RCC_BKPEN;        // enable backup domain interface
  RCC->RSTSCKR |= RCC_LSION;            // enable LSI
  while(!(RCC->RSTSCKR & RCC_LSIRDY));  // wait till LSI is ready
  RCC->BDCTLR |= RCC_RTCSEL_LSI;        // select LSI as RTC clock source
  RCC->BDCTLR |= RCC_RTCEN;             // enable RTC
  RTC->CTLRL &= ~RTC_CTLRL_RSF;         // start synchronization
  while((~RTC->CTLRL) & (RTC_CTLRL_RSF | RTC_CTLRL_RTOFF));   // wait ready
  RTC->CTLRL |= RTC_CTLRL_CNF;          // enter configuration mode
  RTC->PSCRL  = LSI_VALUE;              // set prescaler for 1 second ticks
  RTC->PSCRH  = 0;
  RTC->CTLRL &= ~RTC_CTLRL_CNF;         // exit configuration mode
}

// Init RTC with external low-speed clock
void RTC_init_LSE(void) {
  RCC->BDCTLR |= RCC_LSEON;             // enable LSE
  while(!(RCC->BDCTLR & RCC_LSERDY));   // wait till LSE is ready
  RCC->BDCTLR |= RCC_RTCSEL_LSE;        // select LSE as RTC clock source
  RCC->BDCTLR |= RCC_RTCEN;             // enable RTC
  RTC->CTLRL &= ~RTC_CTLRL_RSF;         // start synchronization
  while((~RTC->CTLRL) & (RTC_CTLRL_RSF | RTC_CTLRL_RTOFF));   // wait ready
  RTC->CTLRL |= RTC_CTLRL_CNF;          // enter configuration mode
  RTC->PSCRL  = LSE_VALUE;              // set prescaler for 1 second ticks
  RTC->PSCRH  = 0;
  RTC->CTLRL &= ~RTC_CTLRL_CNF;         // exit configuration mode
}

// Set RTC counter value
void RTC_set(uint32_t n) {
  RTC->CTLRL &= ~RTC_CTLRL_RSF;         // start synchronization
  while((~RTC->CTLRL) & (RTC_CTLRL_RSF | RTC_CTLRL_RTOFF));   // wait ready
  RTC->CTLRL |= RTC_CTLRL_CNF;          // enter configuration mode
  RTC->CNTL   = n;                      // set counter value
  RTC->CNTH   = n >> 16;
  RTC->CTLRL &= ~RTC_CTLRL_CNF;         // exit configuration mode
}

// Set RTC ALARM value
void RTC_ALARM_set(uint32_t n) {
  RTC->CTLRL &= ~RTC_CTLRL_RSF;         // start synchronization
  while((~RTC->CTLRL) & (RTC_CTLRL_RSF | RTC_CTLRL_RTOFF));   // wait ready
  RTC->CTLRL |= RTC_CTLRL_CNF;          // enter configuration mode
  RTC->ALRML  = n;                      // set ALARM value
  RTC->ALRMH  = n >> 16;
  RTC->CTLRL &= ~RTC_CTLRL_CNF;         // exit configuration mode
}

// ===================================================================================
// Delay Functions
// ===================================================================================

// Wait n counts of SysTick
void DLY_ticks(uint32_t n) {
  uint32_t end = STK->CNTL + n;
  while(((int32_t)(STK->CNTL - end)) < 0);
}

// ===================================================================================
// Independent Watchdog Timer (IWDG) Functions
// ===================================================================================

// Start independent watchdog timer (IWDG) with given amount of WDG clock cycles
// (ticks). One tick is 64/40 ms long, max ticks is 4095 = 6552ms.
// Once the IWDG has been started, it cannot be disabled, only reloaded (feed).
// It can be stopped by disabling the internal low-speed clock (LSI).
void IWDG_start(uint16_t ticks) {
  LSI_enable();                         // enable internal low-speed clock (LSI)
  IWDG->CTLR = 0x5555;                  // allow register modification
  while(IWDG->STATR & IWDG_PVU);        // wait for clock register to be ready
  IWDG->PSCR = 0b100;                   // set LSI clock prescaler 64
  while(IWDG->STATR & IWDG_RVU);        // wait for reload register to be ready
  IWDG->RLDR = ticks;                   // set watchdog counter reload value
  IWDG->CTLR = 0xAAAA;                  // load reload value into watchdog counter
  IWDG->CTLR = 0xCCCC;                  // enable IWDG
}

// Reload watchdog counter with n ticks, n<=4095
void IWDG_reload(uint16_t ticks) {
  IWDG->CTLR = 0x5555;                  // allow register modification
  while(IWDG->STATR & IWDG_RVU);        // wait for reload register to be ready
  IWDG->RLDR = ticks;                   // set watchdog counter reload value
  IWDG->CTLR = 0xAAAA;                  // load reload value into watchdog counter
}

// ===================================================================================
// Sleep Functions
// ===================================================================================

// Put device into sleep, wake up by interrupt
void SLEEP_WFI_now(void) {
  PWR->CTLR &= ~PWR_CTLR_PDDS;          // set power-down mode to SLEEP
  __WFI();                              // wait for interrupt
}

// Put device into sleep, wake up by event
void SLEEP_WFE_now(void) {
  PWR->CTLR &= ~PWR_CTLR_PDDS;          // set power-down mode to SLEEP
  __WFE();                              // wait for event
}

// Put device into stop, wake up interrupt
void STOP_WFI_now(void) {
  PWR->CTLR   &= ~PWR_CTLR_PDDS;        // set power-down mode to STOP
  PFIC->SCTLR |= PFIC_SLEEPDEEP;
  __WFI();                              // wait for interrupt
  PFIC->SCTLR &= ~PFIC_SLEEPDEEP;       // unset deep sleep mode
}

// Put device into stop, wake up event
void STOP_WFE_now(void) {
  PWR->CTLR   &= ~PWR_CTLR_PDDS;        // set power-down mode to STOP
  PFIC->SCTLR |= PFIC_SLEEPDEEP;
  __WFE();                              // wait for event
  PFIC->SCTLR &= ~PFIC_SLEEPDEEP;       // unset deep sleep mode
}

// Put device into standby (deep sleep), wake up interrupt
void STDBY_WFI_now(void) {
  PWR->CTLR   |= PWR_CTLR_PDDS;         // set power-down mode to STANDBY
  PFIC->SCTLR |= PFIC_SLEEPDEEP;
  __WFI();                              // wait for interrupt
  PFIC->SCTLR &= ~PFIC_SLEEPDEEP;       // unset deep sleep mode
}

// Put device into standby (deep sleep), wake up event
void STDBY_WFE_now(void) {
  PWR->CTLR   |= PWR_CTLR_PDDS;         // set power-down mode to STANDBY
  PFIC->SCTLR |= PFIC_SLEEPDEEP;
  __WFE();                              // wait for event
  PFIC->SCTLR &= ~PFIC_SLEEPDEEP;       // unset deep sleep mode
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
// C version of CH32V103 Startup .s file from WCH
// ===================================================================================
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _data_lma;
extern uint32_t _data_vma;
extern uint32_t _edata;

// Prototypes
int main(void)                __attribute__((section(".text.main"), used));
void jump_reset(void)         __attribute__((section(".init.jump"), naked, used));
const uint32_t init_data[]    __attribute__((section(".init.data"), used));
void vectors(void)            __attribute__((section(".vector"), naked, used));
void reset_handler(void)      __attribute__((section(".text.reset_handler"), naked, used));

#if SYS_USE_VECTORS > 0
// Unless a specific handler is overridden, it just spins forever
void default_handler(void)    __attribute__((section(".text.vector_handler"), naked, used));
void default_handler(void)    { while(1); }

// All interrupt handlers are aliased to default_handler unless overridden individually
#define DUMMY_HANDLER __attribute__((section(".text.vector_handler"), weak, alias("default_handler"), used))
DUMMY_HANDLER void NMI_Handler(void);
DUMMY_HANDLER void HardFault_Handler(void);
DUMMY_HANDLER void SysTick_Handler(void);
DUMMY_HANDLER void SW_Handler(void);
DUMMY_HANDLER void WWDG_IRQHandler(void);
DUMMY_HANDLER void PVD_IRQHandler(void);
DUMMY_HANDLER void TAMPER_IRQHandler(void);
DUMMY_HANDLER void RTC_IRQHandler(void);
DUMMY_HANDLER void FLASH_IRQHandler(void);
DUMMY_HANDLER void RCC_IRQHandler(void);
DUMMY_HANDLER void EXTI0_IRQHandler(void);
DUMMY_HANDLER void EXTI1_IRQHandler(void);
DUMMY_HANDLER void EXTI2_IRQHandler(void);
DUMMY_HANDLER void EXTI3_IRQHandler(void);
DUMMY_HANDLER void EXTI4_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel1_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel2_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel3_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel4_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel5_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel6_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel7_IRQHandler(void);
DUMMY_HANDLER void ADC1_2_IRQHandler(void);
DUMMY_HANDLER void EXTI9_5_IRQHandler(void);
DUMMY_HANDLER void TIM1_BRK_IRQHandler(void);
DUMMY_HANDLER void TIM1_UP_IRQHandler(void);
DUMMY_HANDLER void TIM1_TRG_COM_IRQHandler(void);
DUMMY_HANDLER void TIM1_CC_IRQHandler(void);
DUMMY_HANDLER void TIM2_IRQHandler(void);
DUMMY_HANDLER void TIM3_IRQHandler(void);
DUMMY_HANDLER void TIM4_IRQHandler(void);
DUMMY_HANDLER void I2C1_EV_IRQHandler(void);
DUMMY_HANDLER void I2C1_ER_IRQHandler(void);
DUMMY_HANDLER void I2C2_EV_IRQHandler(void);
DUMMY_HANDLER void I2C2_ER_IRQHandler(void);
DUMMY_HANDLER void SPI1_IRQHandler(void);
DUMMY_HANDLER void SPI2_IRQHandler(void);
DUMMY_HANDLER void USART1_IRQHandler(void);
DUMMY_HANDLER void USART2_IRQHandler(void);
DUMMY_HANDLER void USART3_IRQHandler(void);
DUMMY_HANDLER void EXTI15_10_IRQHandler(void);
DUMMY_HANDLER void RTCAlarm_IRQHandler(void);
DUMMY_HANDLER void USBWakeUp_IRQHandler(void);
DUMMY_HANDLER void USBHD_IRQHandler(void);
#endif  // SYS_USE_VECTORS > 0

// FLASH starts with a jump to the reset handler
void jump_reset(void) { asm volatile("j reset_handler"); }

// Afterwards there comes some data
const uint32_t init_data[] = {
  0x00000013, 0x00000013, 0x00000013, 0x00000013,
  0x00000013, 0x00000013, 0x00000013, 0x00000013,
  0x00000013, 0x00000013, 0x00000013, 0x00000013,
  0x00100073
};

// Interrupt vector table
void vectors(void) {
  asm volatile(
  
  // RISC-V handlers
  " j jump_reset                \n"   //  0 - Reset
  #if SYS_USE_VECTORS > 0
  " .word 0                     \n"   //  1 - Reserved
  " j NMI_Handler               \n"   //  2 - NMI Handler
  " j HardFault_Handler         \n"   //  3 - Hard Fault Handler
  " .word 0                     \n"   //  4 - Reserved
  " .word 0                     \n"   //  5 - Reserved
  " .word 0                     \n"   //  6 - Reserved
  " .word 0                     \n"   //  7 - Reserved
  " .word 0                     \n"   //  8 - Reserved
  " .word 0                     \n"   //  9 - Reserved
  " .word 0                     \n"   // 10 - Reserved
  " .word 0                     \n"   // 11 - Reserved
  " j SysTick_Handler           \n"   // 12 - SysTick Handler
  " .word 0                     \n"   // 13 - Reserved
  " j SW_Handler                \n"   // 14 - SW Handler
  " .word 0                     \n"   // 15 - Reserved

  // Peripheral handlers
  " j WWDG_IRQHandler           \n"   // 16 - Window Watchdog
  " j PVD_IRQHandler            \n"   // 17 - PVD through EXTI Line detect
  " j TAMPER_IRQHandler         \n"   // 18 - TAMPER
  " j RTC_IRQHandler            \n"   // 19 - RTC
  " j FLASH_IRQHandler          \n"   // 20 - Flash
  " j RCC_IRQHandler            \n"   // 21 - RCC
  " j EXTI0_IRQHandler          \n"   // 22 - EXTI Line 0
  " j EXTI1_IRQHandler          \n"   // 23 - EXTI Line 1
  " j EXTI2_IRQHandler          \n"   // 24 - EXTI Line 2
  " j EXTI3_IRQHandler          \n"   // 25 - EXTI Line 3
  " j EXTI4_IRQHandler          \n"   // 26 - EXTI Line 4
  " j DMA1_Channel1_IRQHandler  \n"   // 27 - DMA1 Channel 1
  " j DMA1_Channel2_IRQHandler  \n"   // 28 - DMA1 Channel 2
  " j DMA1_Channel3_IRQHandler  \n"   // 29 - DMA1 Channel 3
  " j DMA1_Channel4_IRQHandler  \n"   // 30 - DMA1 Channel 4
  " j DMA1_Channel5_IRQHandler  \n"   // 31 - DMA1 Channel 5
  " j DMA1_Channel6_IRQHandler  \n"   // 32 - DMA1 Channel 6
  " j DMA1_Channel7_IRQHandler  \n"   // 33 - DMA1 Channel 7
  " j ADC1_2_IRQHandler         \n"   // 34 - ADC1/2
  " .word 0                     \n"   // 35 - Reserved
  " .word 0                     \n"   // 36 - Reserved
  " .word 0                     \n"   // 37 - Reserved
  " .word 0                     \n"   // 38 - Reserved
  " j EXTI9_5_IRQHandler        \n"   // 39 - EXTI Line 9..5
  " j TIM1_BRK_IRQHandler       \n"   // 40 - TIM1 Break
  " j TIM1_UP_IRQHandler        \n"   // 41 - TIM1 Update
  " j TIM1_TRG_COM_IRQHandler   \n"   // 42 - TIM1 Trigger and Commutation
  " j TIM1_CC_IRQHandler        \n"   // 43 - TIM1 Capture Compare
  " j TIM2_IRQHandler           \n"   // 44 - TIM2
  " j TIM3_IRQHandler           \n"   // 45 - TIM3
  " j TIM4_IRQHandler           \n"   // 46 - TIM4
  " j I2C1_EV_IRQHandler        \n"   // 47 - I2C1 Event
  " j I2C1_ER_IRQHandler        \n"   // 48 - I2C1 Error
  " j I2C2_EV_IRQHandler        \n"   // 49 - I2C2 Event
  " j I2C2_ER_IRQHandler        \n"   // 50 - I2C2 Error
  " j SPI1_IRQHandler           \n"   // 51 - SPI1
  " j SPI2_IRQHandler           \n"   // 52 - SPI2
  " j USART1_IRQHandler         \n"   // 53 - USART1
  " j USART2_IRQHandler         \n"   // 54 - USART2
  " j USART3_IRQHandler         \n"   // 55 - USART3
  " j EXTI15_10_IRQHandler      \n"   // 56 - EXTI Line 15..10
  " j RTCAlarm_IRQHandler       \n"   // 57 - RTC Alarm through EXTI Line
  " j USBWakeUp_IRQHandler      \n"   // 58 - USB Wake up from suspend
  " j USBHD_IRQHandler          \n"   // 59 - USBHD
  #endif  // SYS_USE_VECTORS > 0
  );
}

// Reset handler
void reset_handler(void) {
  uint32_t *src, *dst;

  // Set pointers, vectors, processor status, and interrupts
  asm volatile(
  " .option push                \n\
    .option norelax             \n\
    la gp, __global_pointer$    \n\
    .option pop                 \n\
    la sp, _eusrstack           \n\
    li a0, 0x88                 \n\
    csrw mstatus, a0            \n\
    la a0, vectors              \n\
    ori a0, a0, 1               \n\
    csrw mtvec, a0              \n\
    csrw mepc, %[main]          \n"
    : : [main] "r" (main) : "a0" , "memory"
  );

  // Copy data from FLASH to RAM
  src = &_data_lma;
  dst = &_data_vma;
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

  // Init system
  SYS_init();

  // Return
  asm volatile("mret");
}
