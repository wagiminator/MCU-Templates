// ===================================================================================
// Basic System Functions for STM32L010, STM32L011, STM32L021, STM32L031      * v1.1 *
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
  #if CLK_VOS_RANGE == 1
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;        // enable power control clock
  while(PWR->CSR & PWR_CSR_VOSF);           // wait until regulator is ready
  PWR->CR = (PWR->CR & ~PWR_CR_VOS)         // select voltage range 1 (1.8V)
          | PWR_CR_VOS_0;                   // (needed for F_CPU > 16MHz, RM0451)
  while(PWR->CSR & PWR_CSR_VOSF);           // wait until voltage range is applied
  RCC->APB1ENR &= ~RCC_APB1ENR_PWREN;       // disable power control clock
  #endif
  #if CLK_LATENCY > 0
  FLASH->ACR |= FLASH_ACR_LATENCY;          // FLASH latency: one wait state
  while(!(FLASH->ACR & FLASH_ACR_LATENCY)); // wait until applied
  FLASH->ACR |= FLASH_ACR_PRFTEN;           // enable prefetch (compensates wait state)
  #endif
  CLK_init();                               // Init clock according to settings
  #endif

  // Init SYSTICK
  #if SYS_TICK_INIT > 0
  SysTick->CTRL = SysTick_CTRL_ENABLE | SysTick_CTRL_CLKSOURCE;
  #endif

  // Enable GPIO A-C
  #if SYS_GPIO_EN > 0
  RCC->IOPENR = RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN | RCC_IOPENR_GPIOCEN;
  #endif
}

// ===================================================================================
// System Clock Functions
// ===================================================================================

// Init internal multi-speed oscillator as system clock source
void CLK_init_MSI(void) {
  #if F_CPU != 2097152
  RCC->ICSCR = (RCC->ICSCR & ~RCC_ICSCR_MSIRANGE) | CLK_MSI_DIV;
  #endif
}

// Init internal oscillator (non PLL) as system clock source
void CLK_init_HSI(void) {
  RCC->CR = RCC_CR_MSION                                  // keep MSI on
          | RCC_CR_HSION                                  // enable HSI
          | CLK_HSI_DIV;                                  // set HSI divider
  while(!(RCC->CR & RCC_CR_HSIRDY));                      // wait for HSI ready
  #if CLK_HSI_DIV > 0
  while(!(RCC->CR & RCC_CR_HSIDIVF));                     // wait for HSI divider applied
  #endif
  RCC->CFGR = CLK_AHB_DIV                                 // set AHB prescaler
            | RCC_CFGR_STOPWUCK                           // HSI for wake-up from STOP
            | 0b01;                                       // HSI as system clock source
  while((RCC->CFGR & RCC_CFGR_SWS_Msk) != (0b01<<2));     // wait until switched to HSI
}

// Init internal oscillator with PLL as system clock source
void CLK_init_HSI_PLL(void) {
  RCC->CR = RCC_CR_MSION                                  // keep MSI on
          | RCC_CR_HSION;                                 // enable HSI
  while(!(RCC->CR & RCC_CR_HSIRDY));                      // wait for HSI ready
  RCC->CFGR = (RCC->CFGR & RCC_CFGR_SW)                   // keep current clock source
            | CLK_PLL_MUL | CLK_PLL_DIV                   // set PLL factor and divider
            | RCC_CFGR_STOPWUCK;                          // HSI for wake-up from STOP
  RCC->CR |= RCC_CR_PLLON;                                // enable PLL
  while(!(RCC->CR & RCC_CR_PLLRDY));                      // wait for PLL ready    
  RCC->CFGR |= 0b11;                                      // PLL as system clock source
  while((RCC->CFGR & RCC_CFGR_SWS_Msk) != (0b11<<2));     // wait until switched to PLL
}

// Init external crystal (non PLL) as system clock source
void CLK_init_HSE(void) {
  RCC->CR = RCC_CR_MSION                                  // keep MSI on
          #if SYS_HSE_BYPASS > 0
          | RCC_CR_HSEBYP                                 // external clock signal
          #endif
          | RCC_CR_HSEON;                                 // enable HSE
  while(!(RCC->CR & RCC_CR_HSERDY));                      // wait for HSE ready
  RCC->CFGR = 0b10;                                       // HSE as system clock source
  while((RCC->CFGR & RCC_CFGR_SWS_Msk) != (0b10<<2));     // wait until switched to HSE
}

// Init external crystal with PLL as system clock source
void CLK_init_HSE_PLL(void) {
  RCC->CR = RCC_CR_MSION                                  // keep MSI on
          #if SYS_HSE_BYPASS > 0
          | RCC_CR_HSEBYP                                 // external clock signal
          #endif
          | RCC_CR_HSEON;                                 // enable HSE
  while(!(RCC->CR & RCC_CR_HSERDY));                      // wait for HSE ready
  RCC->CFGR = (RCC->CFGR & RCC_CFGR_SW)                   // keep current clock source
            | CLK_PLL_MUL | CLK_PLL_DIV                   // set PLL factor and divider
            | RCC_CFGR_PLLSRC;                            // set HSE as PLL source
  RCC->CR |= RCC_CR_PLLON;                                // enable PLL
  while(!(RCC->CR & RCC_CR_PLLRDY));                      // wait for PLL ready    
  RCC->CFGR |= 0b11;                                      // PLL as system clock source
  while((RCC->CFGR & RCC_CFGR_SWS_Msk) != (0b11<<2));     // wait until switched to PLL
}

// Reset system clock to default state
void CLK_reset(void) {
  RCC->CR |= RCC_CR_MSION;                                // enable MSI
  while(!(RCC->CR & RCC_CR_MSIRDY));                      // wait for MSI ready
  RCC->CFGR   = 0x00000000;                               // MSI as system clock source
  while(RCC->CFGR & RCC_CFGR_SWS_Msk);                    // wait until switched to MSI
  RCC->ICSCR  = (RCC->ICSCR & 0x00ff00ff) | 0x0000b000;   // reset ICSCR register
  RCC->CR     = 0x00000100;                               // keep only MSI on
  RCC->CIER   = 0x00000000;                               // disable all interrupts
  RCC->CICR   = 0x000001ff;                               // clear all interrupt flags
  FLASH->ACR &= ~(FLASH_ACR_LATENCY | FLASH_ACR_PRFTEN);  // reset flash latency
}

// ===================================================================================
// Real-Time Clock (RTC) Functions
// ===================================================================================

// Select RTC clock source (RTCSEL can only be changed after an RTC domain reset)
// and remove RTC register write protection
static void RTC_unlock(uint32_t rtcsel) {
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;    // enable low power control block clock
  PWR->CR      |= PWR_CR_DBP;           // enable access to RTC domain
  if((RCC->CSR & RCC_CSR_RTCSEL) != (rtcsel << RCC_CSR_RTCSEL_Pos)) {
    if(RCC->CSR & RCC_CSR_RTCSEL) {     // other clock source already selected?
      uint32_t lse = RCC->CSR & (RCC_CSR_LSEON | RCC_CSR_LSEBYP | RCC_CSR_LSEDRV);
      RCC->CSR |=  RCC_CSR_RTCRST;      // -> reset RTC domain (clears LSE settings)
      RCC->CSR &= ~RCC_CSR_RTCRST;
      RCC->CSR |=  lse;                 // -> restore LSE settings
      if(lse & RCC_CSR_LSEON) while(!(RCC->CSR & RCC_CSR_LSERDY));
    }
    RCC->CSR |= rtcsel << RCC_CSR_RTCSEL_Pos; // set clock source
  }
  RCC->CSR |= RCC_CSR_RTCEN;            // enable RTC
  RCC->APB1ENR &= ~RCC_APB1ENR_PWREN;   // disable low power control block clock
  RTC->WPR = 0xCA;                      // remove RTC register write protection
  RTC->WPR = 0x53;
}

// Init RTC with LSI as clock source and 1s clock period
void RTC_init(void) {
  LSI_enable();                         // enable internal low-speed clock (LSI)
  while(!LSI_ready());                  // wait until LSI is stable
  RTC_unlock(0b10);                     // set LSI as clock source and unlock RTC
  RTC_setPrescaler(73, 499);            // 37kHz / 74 / 500 = 1Hz (LSI is not precise)
}

// Init RTC with LSE as clock source and 1s clock period
void RTC_init_LSE(void) {
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;    // enable low power control block clock
  PWR->CR      |= PWR_CR_DBP;           // enable access to RTC domain
  LSE_enable();                         // enable external low-speed clock (LSE)
  while(!LSE_ready());                  // wait until LSE is stable
  RTC_unlock(0b01);                     // set LSE as clock source and unlock RTC
  RTC_setPrescaler(127, 255);           // 32768Hz / 128 / 256 = 1Hz
}

// Enter/exit RTC initialization mode
static void RTC_enterInit(void) {
  RTC->ISR = 0xffffffff;                // enter initialization mode (rc_w0 flags unchanged)
  while(!(RTC->ISR & RTC_ISR_INITF));   // wait until init mode is entered
}
static void RTC_exitInit(void) {
  RTC->ISR = ~(uint32_t)RTC_ISR_INIT;   // exit initialization mode (rc_w0 flags unchanged)
}

// Set RTC prescaler (default 127, 255)
void RTC_setPrescaler(uint8_t async, uint16_t sync) {
  RTC_enterInit();                      // enter initialization mode
  RTC->PRER = (uint32_t)sync;           // write sync first, then async (two writes, RM)
  RTC->PRER = ((uint32_t)(async & 0x7f) << 16)
            | sync;
  RTC_exitInit();                       // exit initialization mode
}

// Set time in BCD format (24-hour format)
void RTC_setTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
  RTC_enterInit();                      // enter initialization mode
  RTC->CR &= ~RTC_CR_FMT;               // set time format to 24 hours (FMT = 0)
  RTC->TR = (uint32_t)hours   << 16     // set hours in BCD format
          | (uint32_t)minutes <<  8     // set minutes in BCD format
          | (uint32_t)seconds <<  0;    // set seconds in BCD format
  RTC_exitInit();                       // exit initialization mode
  RTC_clearFlag(RTC_ISR_RSF);           // wait for new synchronization before read
}

// Set Date in BCD format
void RTC_setDate(uint8_t year, uint8_t weekday, uint8_t month, uint8_t day) {
  RTC_enterInit();                      // enter initialization mode
  RTC->DR = (uint32_t)year    << 16     // set year in BCD format
          | (uint32_t)weekday << 13     // set day of the week (1:Monday - 7:Sunday)
          | (uint32_t)month   <<  8     // set month in BCD format
          | (uint32_t)day     <<  0;    // set day in BCD format
  RTC_exitInit();                       // exit initialization mode
  RTC_clearFlag(RTC_ISR_RSF);           // wait for new synchronization before read
}

// Read time register
uint32_t RTC_readTime(void) {
  uint32_t time;
  while(!(RTC->ISR & RTC_ISR_RSF));     // wait for registers synchronized
  RTC_clearFlag(RTC_ISR_RSF);           // clear synchronization flag
  time = RTC->TR;                       // read time (locks date shadow register)
  (void)RTC->DR;                        // read date to unlock shadow registers (RM)
  return time;
}

// Read date register
uint32_t RTC_readDate(void) {
  while(!(RTC->ISR & RTC_ISR_RSF));     // wait for registers synchronized
  RTC_clearFlag(RTC_ISR_RSF);           // clear synchronization flag
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
  RTC_clearFlag(RTC_ISR_ALRAF);         // clear alarm flag
  RTC->CR |= RTC_CR_ALRAE;              // enable Alarm A
}

// Set Alarm B
void RTC_setAlarmB(uint8_t weekday, uint8_t hours, uint8_t minutes, uint8_t seconds) {
  RTC->CR &= ~RTC_CR_ALRBE;             // disable Alarm B
  while(!(RTC->ISR & RTC_ISR_ALRBWF));  // wait for write access
  RTC->ALRMBSSR = 0;                    // no sub second comparison
  RTC->ALRMBR = (RTC->ALRMBR & 0xc0808080)  // keep alarm flags
              | (uint32_t)weekday << 24 // set day of the week (1:Monday - 7:Sunday)
              | (uint32_t)hours   << 16 // set hours in BCD format
              | (uint32_t)minutes <<  8 // set minutes in BCD format
              | (uint32_t)seconds <<  0 // set seconds in BCD format
              | RTC_ALRMBR_WDSEL;       // select week day mode
  RTC_clearFlag(RTC_ISR_ALRBF);         // clear alarm flag
  RTC->CR |= RTC_CR_ALRBE;              // enable Alarm B
}

// Start wakeup timer (ms = 1..28000 with LSI, 1..32000 with LSE)
void RTC_startWakeupTimer(uint16_t ms) {
  uint32_t rtcclk = ((RCC->CSR & RCC_CSR_RTCSEL) == (0b10 << RCC_CSR_RTCSEL_Pos)) ? LSI_VALUE : LSE_VALUE;
  uint32_t ticks  = ((uint32_t)ms * rtcclk + 8000) / 16000;  // RTCCLK / 16 ticks
  if(ticks < 1) ticks = 1;
  if(ticks > 0x10000) ticks = 0x10000;  // 16-bit counter
  RTC->CR &= ~RTC_CR_WUTE;              // disable wakeup timer
  while(!(RTC->ISR & RTC_ISR_WUTWF));   // wait to gain access
  RTC->CR &= ~RTC_CR_WUCKSEL;           // clock: RTCCLK / 16 (LSI: 2.3kHz, LSE: 2048Hz)
  RTC->WUTR = ticks - 1;                // set timer (period = WUT + 1 ticks)
  RTC_clearFlag(RTC_ISR_WUTF);          // clear wakeup timer flag
  RTC->CR |= RTC_CR_WUTE;               // enable wakeup timer
}

// ===================================================================================
// Delay Functions
// ===================================================================================

// Wait n+1 counts of SysTick
void DLY_ticks(uint32_t n) {
  while(n > 0xFFFFFF) {                 // SysTick has only 24 bits
    SysTick->LOAD = 0xFFFFFF;
    SysTick->VAL  = 0;
    while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG));
    n -= 0x1000000;
  }
  if(!n) return;                        // LOAD = 0 would stop the SysTick
  SysTick->LOAD = n;
  SysTick->VAL  = 0;
  while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG));
}

// ===================================================================================
// Independent Watchdog Timer (IWDG) Functions
// ===================================================================================

// IWDG counter clock: LSI / 64 (~578Hz with LSI = 37kHz, 4095 counts max ~7s)
#define IWDG_COUNTS(ms)   (((uint32_t)(ms) * (LSI_VALUE / 1000) + 32) / 64)

// Start independent watchdog timer (IWDG) with given time in milliseconds (max 7000).
// Once the IWDG has been started, it cannot be disabled, only reloaded (feed).
// The LSI has a large tolerance (26..56kHz, datasheet), plan a safety margin!
void IWDG_start(uint16_t ms) {
  uint32_t cnt = IWDG_COUNTS(ms);
  if(cnt > 4095) cnt = 4095;            // limit to max value
  IWDG->KR   = 0xCCCC;                  // enable IWDG (also starts LSI), see RM
  IWDG->KR   = 0x5555;                  // allow register modification
  IWDG->PR   = 0b100;                   // set LSI clock prescaler 64
  IWDG->RLR  = cnt;                     // set watchdog counter reload value
  while(IWDG->SR);                      // wait for registers to be updated
  IWDG->KR   = 0xAAAA;                  // load reload value into watchdog counter
}

// Reload watchdog counter with n milliseconds, n<=7000
void IWDG_reload(uint16_t ms) {
  uint32_t cnt = IWDG_COUNTS(ms);
  if(cnt > 4095) cnt = 4095;            // limit to max value
  IWDG->KR   = 0x5555;                  // allow register modification
  while(IWDG->SR & IWDG_SR_RVU);        // wait for reload register to be ready
  IWDG->RLR  = cnt;                     // set watchdog counter reload value
  while(IWDG->SR & IWDG_SR_RVU);        // wait for reload value to be updated
  IWDG->KR   = 0xAAAA;                  // load reload value into watchdog counter
}

// ===================================================================================
// Low Power Timer (LPT) Functions
// ===================================================================================

// Init and enable low-power timer (LSI / 32 = ~1.16kHz, 1 count = ~0.86ms)
// CFGR and IER must only be written while the LPTIM is disabled (RM0451)
void LPT_init(void) {
  LSI_enable();                         // enable internal low-speed clock (LSI)
  while(!LSI_ready());                  // wait until LSI is stable
  RCC->APB1ENR |= RCC_APB1ENR_LPTIM1EN; // enable LPT module clock
  RCC->CCIPR    = (RCC->CCIPR & ~RCC_CCIPR_LPTIM1SEL)
                | (uint32_t)0b01<<18;   // set LSI as LPT clock source
  LPTIM1->CR    = 0;                    // disable timer
  LPTIM1->CFGR  = (uint32_t)0b101<<9;   // set prescaler to 32 (~1kHz)
  LPTIM1->CR    = LPTIM_CR_ENABLE;      // enable timer
}

// Set auto reload value (ARR must be > 0, write is completed when ARROK is set)
static void LPT_setARR(uint16_t ms) {
  if(!ms) ms = 1;                       // ARR must be greater than CMP (0)
  LPTIM1->ICR = LPTIM_ICR_ARROKCF;      // clear ARROK flag
  LPTIM1->ARR = ms;                     // set interval
  while(!(LPTIM1->ISR & LPTIM_ISR_ARROK)); // wait until written
}

// Start low-power timer single shot with period in ms
void LPT_shot(uint16_t ms) {
  LPT_setARR(ms);                       // set interval
  LPTIM1->ICR = LPTIM_ICR_ARRMCF;       // clear auto reload flag
  LPTIM1->CR |= LPTIM_CR_SNGSTRT;       // start timer in single mode
}

// Start low-power timer in continuous mode with period in ms
void LPT_start(uint16_t ms) {
  LPT_setARR(ms);                       // set interval
  LPTIM1->ICR = LPTIM_ICR_ARRMCF;       // clear auto reload flag
  LPTIM1->CR |= LPTIM_CR_CNTSTRT;       // start timer in continuous mode
}

// Enable/disable auto reload interrupt (IER can only be written when disabled)
void LPT_enableReloadInt(void) {
  uint32_t cr = LPTIM1->CR & LPTIM_CR_ENABLE;
  LPTIM1->CR   = 0;                     // disable timer (stops counting!)
  LPTIM1->IER |= LPTIM_IER_ARRMIE;      // enable auto reload interrupt
  LPTIM1->CR   = cr;                    // re-enable timer
}
void LPT_disableReloadInt(void) {
  uint32_t cr = LPTIM1->CR & LPTIM_CR_ENABLE;
  LPTIM1->CR   = 0;                     // disable timer (stops counting!)
  LPTIM1->IER &= ~LPTIM_IER_ARRMIE;     // disable auto reload interrupt
  LPTIM1->CR   = cr;                    // re-enable timer
}

// Put device in to SLEEP and wake-up after LPT period in ms (LPT_init() first)
void LPT_sleep(uint16_t ms) {
  LPTIM1->CR   = 0;                     // disable timer to write IER
  LPTIM1->IER  = LPTIM_IER_ARRMIE;      // enable auto reload interrupt (event)
  LPTIM1->CR   = LPTIM_CR_ENABLE;       // enable timer
  EXTI->EMR   |= (uint32_t)1 << 29;     // enable wake up with event
  LPT_shot(ms);                         // start low-power timer (LPT) single shot
  while(!(LPTIM1->ISR & LPTIM_ISR_ARRM)) SLEEP_WFE_now(); // sleep until LPT event
  LPTIM1->ICR  = LPTIM_ICR_ARRMCF;      // clear auto reload flag
  EXTI->EMR   &= ~((uint32_t)1 << 29);  // disable wake up with event
  LPTIM1->CR   = 0;                     // disable timer to write IER
  LPTIM1->IER  = 0;                     // disable auto reload interrupt
  LPTIM1->CR   = LPTIM_CR_ENABLE;       // enable timer
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
  EXTI->PR = 0xffffffff;                // clear all EXTI pending flags
  PWR->CR = (PWR->CR & ~PWR_CR_PDDS)    // select STOP mode
          | PWR_CR_LPSDSR               // low-power regulator in STOP mode
          | PWR_CR_CWUF;                // clear wakup flag
  __WFI();                              // wait for interrupt
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;   // unset deep sleep mode
  RCC->APB1ENR &=~ RCC_APB1ENR_PWREN;   // disable low power control block clock
  #if SYS_CLK_INIT > 0
  CLK_init();                           // restore system clock (MSI/HSI after wake-up)
  #endif
}

// Put device into STOP, wake up event
void STOP_WFE_now(void) {
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;    // enable low power control block clock
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;    // set deep sleep mode
  NVIC->ICPR[0] = 0xffffffff;           // clear pending interrupts
  EXTI->PR = 0xffffffff;                // clear all EXTI pending flags
  PWR->CR = (PWR->CR & ~PWR_CR_PDDS)    // select STOP mode
          | PWR_CR_LPSDSR               // low-power regulator in STOP mode
          | PWR_CR_CWUF;                // clear wakup flag
  __WFE();                              // wait for event
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;   // unset deep sleep mode
  RCC->APB1ENR &=~ RCC_APB1ENR_PWREN;   // disable low power control block clock
  #if SYS_CLK_INIT > 0
  CLK_init();                           // restore system clock (MSI/HSI after wake-up)
  #endif
}

// Put device into STANDBY, wake up interrupt
void STDBY_WFI_now(void) {
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;    // enable low power control block clock
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;    // set deep sleep mode
  NVIC->ICPR[0] = 0xffffffff;           // clear pending interrupts
  EXTI->PR = 0xffffffff;                // clear all EXTI pending flags
  PWR->CR |= PWR_CR_PDDS                // select STANDBY mode
           | PWR_CR_CWUF;               // clear wakup flag
  RTC_clearFlag((uint32_t)0xff<<8);     // clear RTC interrupt flags
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
  RTC_clearFlag((uint32_t)0xff<<8);     // clear RTC interrupt flags
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
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // enable SYSCFG clock
  SYSCFG->CFGR1 = (SYSCFG->CFGR1 & ~SYSCFG_CFGR1_MEM_MODE)
                | SYSCFG_CFGR1_MEM_MODE_0;  // map system memory to 0x00000000
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
// C version of startup_stm32l010xb.s Startup File
// ===================================================================================
extern uint32_t _sbss, _ebss, _sdata, _edata, _sidata;
extern void _estack(void);

// Prototypes
int main(void)                __attribute__((section(".text.main"), used));
void (*const vectors[])(void) __attribute__((section(".isr_vector"), used));
void Reset_Handler(void)      __attribute__((section(".text.irq_handler"), used, noreturn));

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
DUMMY_HANDLER void DMA1_Channel4_5_IRQHandler(void);  // DMA1 channel 4..7
DUMMY_HANDLER void ADC1_IRQHandler(void);
DUMMY_HANDLER void LPTIM1_IRQHandler(void);
DUMMY_HANDLER void TIM2_IRQHandler(void);
DUMMY_HANDLER void TIM21_IRQHandler(void);
DUMMY_HANDLER void TIM22_IRQHandler(void);
DUMMY_HANDLER void I2C1_IRQHandler(void);
DUMMY_HANDLER void SPI1_IRQHandler(void);
DUMMY_HANDLER void USART2_IRQHandler(void);
DUMMY_HANDLER void LPUART1_IRQHandler(void);
#endif  // SYS_USE_VECTORS > 0

// Interrupt vector table
void (*const vectors[])(void) = {
  &_estack,                       //  0 - Initial Stack Pointer Value

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
  PVD_IRQHandler,                 //  1 - PVD through EXTI Line detection
  RTC_IRQHandler,                 //  2 - RTC through EXTI Line
  FLASH_IRQHandler,               //  3 - FLASH
  RCC_IRQHandler,                 //  4 - RCC
  EXTI0_1_IRQHandler,             //  5 - EXTI Line 0 and 1
  EXTI2_3_IRQHandler,             //  6 - EXTI Line 2 and 3
  EXTI4_15_IRQHandler,            //  7 - EXTI Line 4 to 15
  0,                              //  8 - Reserved
  DMA1_Channel1_IRQHandler,       //  9 - DMA1 Channel 1
  DMA1_Channel2_3_IRQHandler,     // 10 - DMA1 Channel 2 and 3
  DMA1_Channel4_5_IRQHandler,     // 11 - DMA1 Channel 4, 5, (6 and 7)
  ADC1_IRQHandler,                // 12 - ADC1 (COMP1 and COMP2)
  LPTIM1_IRQHandler,              // 13 - LPTIM1
  0,                              // 14 - Reserved
  TIM2_IRQHandler,                // 15 - TIM2
  0,                              // 16 - Reserved
  0,                              // 17 - Reserved
  0,                              // 18 - Reserved
  0,                              // 19 - Reserved
  TIM21_IRQHandler,               // 20 - TIM21
  0,                              // 21 - Reserved
  TIM22_IRQHandler,               // 22 - TIM22
  I2C1_IRQHandler,                // 23 - I2C1
  0,                              // 24 - Reserved
  SPI1_IRQHandler,                // 25 - SPI1
  0,                              // 26 - Reserved
  0,                              // 27 - Reserved
  USART2_IRQHandler,              // 28 - USART2
  LPUART1_IRQHandler,             // 29 - LPUART1 (AES)
  0,                              // 30 - Reserved
  0,                              // 31 - Reserved
  #endif  // SYS_USE_VECTORS > 0
};

// Reset handler
void Reset_Handler(void) {
  uint32_t *src, *dst;

  // Set stack pointer (no literal pool, works with large programs)
  asm volatile("mov sp, %0" : : "r" (&_estack) : "memory");

  // Configure vector table location
  SCB->VTOR = (uint32_t)vectors;

  // Check if boot space corresponds to system memory (started by bootloader)
  if(((*(uint32_t*)0x00000004) >> 24) == 0x1f) {
    RCC->APB2ENR  = RCC_APB2ENR_SYSCFGEN;
    SYSCFG->CFGR1 = (uint32_t)0;        // map main flash to 0x00000000
  }

  // Copy data from FLASH to RAM
  src = &_sidata;
  dst = &_sdata;
  while(dst < &_edata) *dst++ = *src++;

  // Clear uninitialized variables
  #if SYS_CLEAR_BSS > 0
  dst = &_sbss;
  while(dst < &_ebss) *dst++ = 0;
  #endif

  // Init system
  SYS_init();

  // C++ Support
  #ifdef __cplusplus
  __libc_init_array();
  #endif

  // Start main function
  main();
  while(1);
}
