// ===================================================================================
// Basic System Functions for RP2350                                          * v1.0 *
// ===================================================================================
//
// This file must be included!!! The system configuration and the system clock are
// set up automatically on system start.

#include "system.h"

// ===================================================================================
// Setup Microcontroller (this function is called automatically at startup)
// ===================================================================================

// Peripherals that are held in reset until they are used by the libraries
#define SYS_RESET_MASK  ( RESETS_RESET_ADC     | RESETS_RESET_DMA     | RESETS_RESET_HSTX    \
                        | RESETS_RESET_I2C0    | RESETS_RESET_I2C1    | RESETS_RESET_IO_BANK0\
                        | RESETS_RESET_PADS_BANK0 | RESETS_RESET_PIO0 | RESETS_RESET_PIO1    \
                        | RESETS_RESET_PIO2    | RESETS_RESET_PWM     | RESETS_RESET_SHA256  \
                        | RESETS_RESET_SPI0    | RESETS_RESET_SPI1    | RESETS_RESET_TIMER0  \
                        | RESETS_RESET_TIMER1  | RESETS_RESET_TRNG    | RESETS_RESET_UART0   \
                        | RESETS_RESET_UART1   | RESETS_RESET_USBCTRL )

// System blocks that are always released from reset
#define SYS_RELEASE_MASK  ( RESETS_RESET_BUSCTRL | RESETS_RESET_JTAG | RESETS_RESET_SYSCFG \
                          | RESETS_RESET_SYSINFO | RESETS_RESET_TBMAN )

void SYS_init(void) {
  // Hold peripherals in reset, release system blocks
  RESET_assert(SYS_RESET_MASK);
  RESET_release(SYS_RELEASE_MASK);

  // Init system clock
  #if SYS_CLK_INIT > 0
  CLK_init();
  #endif

  // Init SYSTICK
  #if SYS_TICK_INIT > 0
  SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
  #endif

  // Release GPIO from reset
  #if SYS_GPIO_EN > 0
  RESET_release(RESETS_RESET_IO_BANK0 | RESETS_RESET_PADS_BANK0);
  #endif
}

// ===================================================================================
// System Clock Functions
// ===================================================================================

// Enable crystal oscillator (XOSC, 12MHz)
void XOSC_enable(void) {
  if(XOSC_ready()) return;                          // already running
  XOSC->STARTUP = ((XOSC_VALUE / 1000) + 128) / 256;// startup delay approx. 1ms
  XOSC->CTRL    = XOSC_CTRL_FREQ_RANGE_1_15MHZ | XOSC_CTRL_ENABLE_ENABLE;
  while(!XOSC_ready());                             // wait until stable
}

// Init PLL with output frequency freq (Hz). The settings are searched so that the
// frequency is generated exactly with the highest possible VCO frequency (lowest
// jitter). Returns 0 if the frequency cannot be generated.
uint8_t PLL_init(PLL_TypeDef* pll, uint32_t freq) {
  uint32_t fbdiv, vco, pd1, pd2;
  for(fbdiv = 1600000000 / XOSC_VALUE; fbdiv * XOSC_VALUE >= 750000000; fbdiv--) {
    vco = fbdiv * XOSC_VALUE;
    for(pd1 = 7; pd1; pd1--) {
      for(pd2 = pd1; pd2; pd2--) {
        if((vco % (pd1 * pd2) == 0) && (vco / (pd1 * pd2) == freq)) {
          RESET_cycle(pll == PLL_SYS ? RESETS_RESET_PLL_SYS : RESETS_RESET_PLL_USB);
          pll->CS        = 1;                       // reference divider 1
          pll->FBDIV_INT = fbdiv;                   // feedback divider
          REG_CLR(pll->PWR, PLL_PWR_PD | PLL_PWR_VCOPD);  // power up PLL and VCO
          while(!PLL_locked(pll));                  // wait for lock
          pll->PRIM = (pd1 << PLL_PRIM_POSTDIV1_Pos) | (pd2 << PLL_PRIM_POSTDIV2_Pos);
          REG_CLR(pll->PWR, PLL_PWR_POSTDIVPD);     // power up post dividers
          return 1;
        }
      }
    }
  }
  return 0;
}

// Configure auxiliary clock (clk_peri, clk_hstx, clk_usb, clk_adc, clk_gpout):
// ctrl points to the CTRL register, the DIV register (integer part in bits 31..16)
// follows directly
static void CLK_setAux(volatile uint32_t* ctrl, uint32_t auxsrc, uint32_t div) {
  volatile uint32_t i;
  REG_CLR(*ctrl, CLOCKS_CLK_PERI_CTRL_ENABLE);      // stop clock
  for(i = 0; i < 500; i++);                         // wait for clock to stop (3 cycles)
  ctrl[1] = div << 16;                              // set integer divider
  *ctrl   = auxsrc;                                 // select clock source
  REG_SET(*ctrl, CLOCKS_CLK_PERI_CTRL_ENABLE);      // start clock
}

// Init system clock according to F_CPU
void CLK_init(void) {
  volatile uint32_t* tick;
  uint8_t i;

  // Switch clk_sys and clk_ref to safe sources (glitchless muxes)
  CLOCKS->CLK_SYS_RESUS_CTRL = 0;                   // disable resus
  XOSC_enable();                                    // start crystal oscillator
  REG_CLR(CLOCKS->CLK_SYS_CTRL, CLOCKS_CLK_SYS_CTRL_SRC); // clk_sys = clk_ref
  while(!(CLOCKS->CLK_SYS_SELECTED & 1));
  REG_CLR(CLOCKS->CLK_REF_CTRL, CLOCKS_CLK_REF_CTRL_SRC_Msk); // clk_ref = ROSC
  while(!(CLOCKS->CLK_REF_SELECTED & 1));

  // Setup PLLs: PLL_USB = 48MHz, PLL_SYS = F_CPU
  PLL_init(PLL_USB, F_USB);
  #if F_CPU != 12000000
  if(!PLL_init(PLL_SYS, F_CPU)) PLL_init(PLL_SYS, 150000000);
  #else
  PLL_stop(PLL_SYS);
  #endif

  // clk_ref = XOSC (12MHz)
  CLOCKS->CLK_REF_DIV  = 1 << CLOCKS_CLK_REF_DIV_INT_Pos;
  CLOCKS->CLK_REF_CTRL = CLOCKS_CLK_REF_CTRL_SRC_XOSC_CLKSRC;
  while(!(CLOCKS->CLK_REF_SELECTED & (1 << 2)));

  // clk_sys = PLL_SYS (or clk_ref = XOSC for 12MHz)
  CLOCKS->CLK_SYS_DIV  = 1 << CLOCKS_CLK_SYS_DIV_INT_Pos;
  #if F_CPU != 12000000
  CLOCKS->CLK_SYS_CTRL = CLOCKS_CLK_SYS_CTRL_AUXSRC_CLKSRC_PLL_SYS;
  REG_SET(CLOCKS->CLK_SYS_CTRL, CLOCKS_CLK_SYS_CTRL_SRC);
  while(!(CLOCKS->CLK_SYS_SELECTED & 2));
  #endif

  // clk_peri = clk_hstx = clk_sys, clk_usb = clk_adc = PLL_USB (48MHz)
  CLK_setAux(&CLOCKS->CLK_PERI_CTRL, CLOCKS_CLK_PERI_CTRL_AUXSRC_CLK_SYS, 1);
  CLK_setAux(&CLOCKS->CLK_HSTX_CTRL, CLOCKS_CLK_HSTX_CTRL_AUXSRC_CLK_SYS, 1);
  CLK_setAux(&CLOCKS->CLK_USB_CTRL,  CLOCKS_CLK_USB_CTRL_AUXSRC_CLKSRC_PLL_USB, 1);
  CLK_setAux(&CLOCKS->CLK_ADC_CTRL,  CLOCKS_CLK_ADC_CTRL_AUXSRC_CLKSRC_PLL_USB, 1);

  // Tick generators (PROC0, PROC1, TIMER0, TIMER1, WATCHDOG, RISCV): 1us ticks from
  // clk_ref (12MHz); each generator has the registers CTRL, CYCLES, COUNT
  tick = &TICKS->PROC[0].CTRL;
  for(i = 0; i < 6; i++, tick += 3) {
    tick[0] = 0;                                    // stop tick generator
    tick[1] = F_REF / 1000000;                      // clk_ref cycles per tick
    tick[0] = TICKS_PROC_CTRL_ENABLE;               // start tick generator
  }
}

// Reset system clock to default state (clk_sys = clk_ref = ROSC, PLLs off)
void CLK_reset(void) {
  REG_CLR(CLOCKS->CLK_SYS_CTRL, CLOCKS_CLK_SYS_CTRL_SRC);
  while(!(CLOCKS->CLK_SYS_SELECTED & 1));
  CLOCKS->CLK_REF_CTRL = CLOCKS_CLK_REF_CTRL_SRC_ROSC_CLKSRC_PH;
  while(!(CLOCKS->CLK_REF_SELECTED & 1));
  PLL_stop(PLL_SYS);
  PLL_stop(PLL_USB);
  XOSC_disable();
}

// Output clock source src / MCO_DIV on MCO_PIN (GPOUT0..3)
#define MCO_GPOUT   ( ((MCO_PIN == GP13) || (MCO_PIN == GP21)) ? 0 : \
                      ((MCO_PIN == GP15) || (MCO_PIN == GP23)) ? 1 : (MCO_PIN - GP22) )

void MCO_set(uint32_t src) {
  RESET_release(RESETS_RESET_IO_BANK0 | RESETS_RESET_PADS_BANK0);
  CLK_setAux(&CLOCKS->CLK_GPOUT[MCO_GPOUT].CTRL, src, MCO_DIV);
  IO_BANK0->GPIO[MCO_PIN].CTRL = GPIO_FUNC_GPCK;
  REG_CLR(PADS_BANK0->GPIO[MCO_PIN], PADS_BANK0_GPIO_OD | PADS_BANK0_GPIO_ISO);
}

// Disable clock output
void MCO_stop(void) {
  IO_BANK0->GPIO[MCO_PIN].CTRL = GPIO_FUNC_NULL;
  REG_CLR(CLOCKS->CLK_GPOUT[MCO_GPOUT].CTRL, CLOCKS_CLK_GPOUT_CTRL_ENABLE);
}

// ===================================================================================
// Real-Time Clock (RTC) Functions (calendar on the AON timer of POWMAN)
// ===================================================================================
uint32_t RTC_date;                                  // date of the last RTC_readTime()

// Leap year, days per month, weekday of RTC_YEAR_REF-01-01 (0: Sunday)
#define RTC_isLeap(y)   ( !((y) & 3) && (((y) % 100) || !((y) % 400)) )
#define RTC_DOTW_REF    ( ( (RTC_YEAR_REF - 1) + (RTC_YEAR_REF - 1) / 4                 \
                          - (RTC_YEAR_REF - 1) / 100 + (RTC_YEAR_REF - 1) / 400 + 1 ) % 7 )
static const uint8_t RTC_mdays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// Read 64-bit AON timer (milliseconds), upper word is read before and after
uint64_t RTC_getMillis(void) {
  uint32_t hi = POWMAN->READ_TIME_UPPER, lo, hi2;
  while(1) {
    lo  = POWMAN->READ_TIME_LOWER;
    hi2 = POWMAN->READ_TIME_UPPER;
    if(hi == hi2) break;
    hi = hi2;
  }
  return ((uint64_t)hi << 32) | lo;
}

// Read AON timer in seconds (64-bit division by 1000 with 32-bit operations)
static uint32_t RTC_getSec(void) {
  uint64_t ms = RTC_getMillis();
  uint32_t hi = ms >> 32, lo = (uint32_t)ms, t, q1, q0;
  t  = ((hi % 1000) << 16) | (lo >> 16);
  q1 = t / 1000;
  t  = ((t  % 1000) << 16) | (lo & 0xFFFF);
  q0 = t / 1000;
  return (q1 << 16) | q0;                           // (hi / 1000 = 0 for 136 years)
}

// Write AON timer (milliseconds), timer is stopped during writing
static void RTC_setMillis(uint64_t ms) {
  uint32_t run = POWMAN->TIMER & POWMAN_TIMER_RUN;
  POWMAN_CLR(POWMAN->TIMER, POWMAN_TIMER_RUN);
  POWMAN_WRITE(POWMAN->SET_TIME_15TO0, ms);
  POWMAN_WRITE(POWMAN->SET_TIME_31TO16, ms >> 16);
  POWMAN_WRITE(POWMAN->SET_TIME_47TO32, ms >> 32);
  POWMAN_WRITE(POWMAN->SET_TIME_63TO48, ms >> 48);
  if(run) POWMAN_SET(POWMAN->TIMER, POWMAN_TIMER_RUN);
}

// Init and start RTC (AON timer clocked by XOSC), date 2000-01-01, time 00:00:00
void RTC_init(void) {
  POWMAN_CLR(POWMAN->TIMER, POWMAN_TIMER_RUN);      // stop AON timer
  POWMAN_WRITE(POWMAN->XOSC_FREQ_KHZ_INT, XOSC_VALUE / 1000);  // XOSC frequency in kHz
  POWMAN_WRITE(POWMAN->XOSC_FREQ_KHZ_FRAC, 0);
  POWMAN_SET(POWMAN->TIMER, POWMAN_TIMER_USE_XOSC); // 1kHz tick from XOSC
  RTC_setMillis(0);                                 // RTC_YEAR_REF-01-01 00:00:00
  POWMAN_SET(POWMAN->TIMER, POWMAN_TIMER_RUN);      // start AON timer
}

// Read time (weekday, hours, minutes, seconds) and store date in RTC_date
uint32_t RTC_readTime(void) {
  uint32_t sec  = RTC_getSec();
  uint32_t days = sec / 86400;
  uint32_t tod  = sec % 86400;
  uint16_t year = RTC_YEAR_REF;
  uint8_t  month = 0, mdays;
  uint32_t time = (((days + RTC_DOTW_REF) % 7) << 24) | ((tod / 3600) << 16)
                | (((tod / 60) % 60) << 8) | (tod % 60);
  while(days >= 365u + RTC_isLeap(year)) {
    days -= 365 + RTC_isLeap(year);
    year++;
  }
  while(days >= (mdays = RTC_mdays[month] + (month == 1 && RTC_isLeap(year)))) {
    days -= mdays;
    month++;
  }
  RTC_date = ((uint32_t)year << 12) | ((uint32_t)(month + 1) << 8) | (days + 1);
  return time;
}

// Set time (hours, minutes, seconds), decimal values
void RTC_setTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
  uint32_t sec = RTC_getSec();
  sec = sec - (sec % 86400) + (uint32_t)hours * 3600 + (uint32_t)minutes * 60 + seconds;
  RTC_setMillis((uint64_t)sec * 1000);
}

// Set date (year since RTC_YEAR_REF, month, day), decimal values. The day of the week
// is calculated automatically.
void RTC_setDate(uint8_t year, uint8_t month, uint8_t day) {
  uint32_t days = 0, sec = RTC_getSec() % 86400;
  uint16_t y;
  uint8_t  m;
  for(y = RTC_YEAR_REF; y < RTC_YEAR_REF + year; y++) days += 365 + RTC_isLeap(y);
  for(m = 1; m < month; m++) days += RTC_mdays[m - 1] + (m == 2 && RTC_isLeap(y));
  sec += (days + day - 1) * 86400;
  RTC_setMillis((uint64_t)sec * 1000);
}

// Set alarm for the next time the clock shows hours:minutes:seconds
void RTC_setAlarm(uint8_t hours, uint8_t minutes, uint8_t seconds) {
  uint32_t sec = RTC_getSec();
  uint32_t tod = sec % 86400;
  uint32_t alarm = (uint32_t)hours * 3600 + (uint32_t)minutes * 60 + seconds;
  uint64_t ms;
  sec = sec - tod + alarm;
  if(alarm <= tod) sec += 86400;                    // tomorrow
  ms = (uint64_t)sec * 1000;
  RTC_stopAlarm();                                  // alarm must be disabled to set time
  POWMAN_WRITE(POWMAN->ALARM_TIME_15TO0, ms);
  POWMAN_WRITE(POWMAN->ALARM_TIME_31TO16, ms >> 16);
  POWMAN_WRITE(POWMAN->ALARM_TIME_47TO32, ms >> 32);
  POWMAN_WRITE(POWMAN->ALARM_TIME_63TO48, ms >> 48);
  POWMAN_CLR(POWMAN->TIMER, POWMAN_TIMER_ALARM);    // clear alarm flag
  POWMAN_SET(POWMAN->TIMER, POWMAN_TIMER_ALARM_ENAB);
}

// ===================================================================================
// Delay Functions
// ===================================================================================

// Wait n+1 counts of SysTick
void DLY_ticks(uint32_t n) {
  while(n > 0xFFFFFF) {                 // SysTick has only 24 bits
    SysTick->LOAD = 0xFFFFFF;
    SysTick->VAL  = 0;
    while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    n -= 0x1000000;
  }
  SysTick->LOAD = n;
  SysTick->VAL  = 0;
  while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
}

// ===================================================================================
// Watchdog Timer (IWDG/WDT) Functions
// ===================================================================================
uint32_t IWDG_loadValue;

// Watchdog resets everything except the oscillators
#define WDT_RESET_SCOPE   (0x01FFFFFF & ~(PSM_WDSEL_ROSC | PSM_WDSEL_XOSC))

// Reload watchdog counter with n milliseconds (max 16700ms)
void IWDG_reload(uint16_t ms) {
  if(ms > 16700) ms = 16700;
  IWDG_loadValue = (uint32_t)ms * 1000;           // counter decrements once per us
  IWDG_feed();
}

// Start watchdog timer with given time in milliseconds (max 16700ms)
void IWDG_start(uint16_t ms) {
  REG_CLR(WATCHDOG->CTRL, WATCHDOG_CTRL_ENABLE);  // stop watchdog
  PSM->WDSEL = WDT_RESET_SCOPE;                   // set reset scope
  WATCHDOG->SCRATCH[4] = 0;                       // normal boot after watchdog reset
  IWDG_reload(ms);                                // set period
  REG_SET(WATCHDOG->CTRL, WATCHDOG_CTRL_PAUSE_DBG0 | WATCHDOG_CTRL_PAUSE_DBG1
                        | WATCHDOG_CTRL_PAUSE_JTAG | WATCHDOG_CTRL_ENABLE);
}

// ===================================================================================
// Sleep Functions
// ===================================================================================

// Put CPU into sleep, wake up by interrupt
void SLEEP_WFI_now(void) {
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
  __DSB();
  __WFI();
}

// Put CPU into sleep, wake up by event
void SLEEP_WFE_now(void) {
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
  __DSB();
  __WFE();
}

// Put CPU into sleep for n milliseconds (uses TIMER0 alarm 3)
void SLEEP_ms(uint32_t ms) {
  RESET_release(RESETS_RESET_TIMER0);
  NVIC_DisableIRQ(TIMER0_IRQ_3_IRQn);
  REG_SET(TIMER0->INTE, TIMER_INTE_ALARM_3);        // alarm interrupt (for wake-up)
  TIMER0->ALARM[3] = TIMER0->TIMERAWL + ms * 1000;  // arm alarm
  SCB->SCR |= SCB_SCR_SEVONPEND_Msk;                // pending interrupt generates event
  while(TIMER0->ARMED & TIMER_ARMED_ARMED_Msk & (1 << 3)) SLEEP_WFE_now();
  SCB->SCR &= ~SCB_SCR_SEVONPEND_Msk;
  TIMER0->INTR = TIMER_INTR_ALARM_3;                // clear interrupt flag
  REG_CLR(TIMER0->INTE, TIMER_INTE_ALARM_3);
  NVIC_ClearPendingIRQ(TIMER0_IRQ_3_IRQn);
}

// Put device into DORMANT, wake up by pin event (type: see PIN_INT_xxx in gpio.h)
void DORMANT_now(uint8_t pin, uint8_t type) {
  static const uint8_t evt[] = {0, 8, 4, 0, 2, 1};  // edge high, edge low, level high/low
  uint32_t mask = (uint32_t)evt[type < 6 ? type : 0] << ((pin & 7) << 2);
  if(!mask) return;

  // Run from XOSC, stop PLLs
  REG_CLR(CLOCKS->CLK_SYS_CTRL, CLOCKS_CLK_SYS_CTRL_SRC); // clk_sys = clk_ref (XOSC)
  while(!(CLOCKS->CLK_SYS_SELECTED & 1));
  PLL_stop(PLL_SYS);
  PLL_stop(PLL_USB);

  // Setup wake-up event and put XOSC into dormant mode
  REG_WRITE_MASK(PADS_BANK0->GPIO[pin], PADS_BANK0_GPIO_IE,
                 PADS_BANK0_GPIO_IE | PADS_BANK0_GPIO_ISO);
  IO_BANK0->INTR[pin >> 3] = mask;                  // clear edge events
  REG_SET(IO_BANK0->DORMANT_WAKE_INTE[pin >> 3], mask);
  XOSC->DORMANT = 0x636F6D61;                       // "coma": stop XOSC
  while(!XOSC_ready());                             // wait for wake-up

  // Clear wake-up event and restart system clock
  REG_CLR(IO_BANK0->DORMANT_WAKE_INTE[pin >> 3], mask);
  IO_BANK0->INTR[pin >> 3] = mask;
  CLK_init();
}

// ===================================================================================
// Reset and Bootloader Functions
// ===================================================================================

// Conduct software reset via watchdog (resets the whole chip except the oscillators)
void RST_now(void) {
  REG_CLR(WATCHDOG->CTRL, WATCHDOG_CTRL_ENABLE);
  PSM->WDSEL = WDT_RESET_SCOPE;
  WATCHDOG->SCRATCH[4] = 0;                       // normal boot from flash
  REG_SET(WATCHDOG->CTRL, WATCHDOG_CTRL_TRIGGER);
  while(1);
}

// Get pointer to boot ROM function with code c1,c2 (e.g. 'R','B'), Arm secure version
void* ROM_func(char c1, char c2) {
  typedef void* (*rom_lookup_fn)(uint32_t code, uint32_t mask);
  volatile uint16_t* rom = (volatile uint16_t*)0x16;  // pointer to lookup function
  __ASM volatile("" : "+r" (rom));                    // (hide address from compiler)
  rom_lookup_fn lookup = (rom_lookup_fn)(uint32_t)*rom;
  return lookup((uint32_t)c1 | ((uint32_t)c2 << 8), 0x0004);  // RT_FLAG_FUNC_ARM_SEC
}

// Reset and enter USB bootloader (BOOTSEL mode, mass storage and PICOBOOT interface)
void BOOT_now(void) {
  typedef int (*rom_reboot_fn)(uint32_t flags, uint32_t delay_ms, uint32_t p0, uint32_t p1);
  rom_reboot_fn reboot = (rom_reboot_fn)ROM_func('R', 'B');
  reboot(0x0102, 10, 0, 0);                       // type BOOTSEL, no return on success
  while(1);
}

// ===================================================================================
// Cyclic Redundancy Check (CRC) Functions (DMA sniffer)
// ===================================================================================

// Calculate standard CRC-32 (IEEE 802.3, as zlib) of len bytes at address addr
uint32_t CRC_calc32(const void* addr, uint32_t len) {
  uint32_t crc;
  static uint8_t dummy;
  DMA_init();
  DMA->SNIFF_DATA = 0xFFFFFFFF;                     // start value
  DMA->SNIFF_CTRL = DMA_SNIFF_CTRL_EN | ((uint32_t)DMA_CRC_CH << DMA_SNIFF_CTRL_DMACH_Pos)
                  | ((uint32_t)1 << DMA_SNIFF_CTRL_CALC_Pos)  // CRC-32 with reversed data
                  | DMA_SNIFF_CTRL_OUT_REV | DMA_SNIFF_CTRL_OUT_INV;
  DMA_CH(DMA_CRC_CH).READ_ADDR   = (uint32_t)addr;
  DMA_CH(DMA_CRC_CH).WRITE_ADDR  = (uint32_t)&dummy;
  DMA_CH(DMA_CRC_CH).TRANS_COUNT = len;
  DMA_CH(DMA_CRC_CH).CTRL_TRIG   = DMA_CH_CTRL_TRIG_EN | DMA_CH_CTRL_TRIG_INCR_READ
                                 | DMA_CH_CTRL_TRIG_SNIFF_EN
                                 | ((uint32_t)DREQ_FORCE << DMA_CH_CTRL_TRIG_TREQ_SEL_Pos)
                                 | ((uint32_t)DMA_CRC_CH << DMA_CH_CTRL_TRIG_CHAIN_TO_Pos);
  while(DMA_busy(DMA_CRC_CH));
  crc = DMA->SNIFF_DATA;                  // read result while OUT_REV/OUT_INV are set
  DMA->SNIFF_CTRL = 0;                    // (they are applied when reading)
  return crc;
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
// Image Definition Block (IMAGE_DEF, must be within the first 4KB of the flash)
// ===================================================================================
// The boot ROM only starts images that contain a valid IMAGE_DEF block. This minimal
// block marks the image as an executable for the Arm cores in secure mode on the
// RP2350. The vector table at the beginning of the flash is used for the start (see
// RP2350 datasheet, chapter 5 "Bootrom", section "Metadata Block Details").
__attribute__((section(".embedded_block"), used))
const uint32_t SYS_imageDef[5] = {
  0xFFFFDED3,                     // PICOBIN_BLOCK_MARKER_START
  0x10210142,                     // item IMAGE_TYPE (1 word): EXE, secure, Arm, RP2350
  0x000001FF,                     // item LAST: total size of the previous items: 1 word
  0x00000000,                     // relative link to next block (0: only this block)
  0xAB123579                      // PICOBIN_BLOCK_MARKER_END
};

// ===================================================================================
// C version of the RP2350 Startup File
// ===================================================================================
extern uint32_t _sbss, _ebss, _sdata, _edata, _sidata;
extern void _estack(void);

// Prototypes
int main(void)                __attribute__((section(".text.main"), used, noinline));
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
DUMMY_HANDLER void MemManage_Handler(void);
DUMMY_HANDLER void BusFault_Handler(void);
DUMMY_HANDLER void UsageFault_Handler(void);
DUMMY_HANDLER void SecureFault_Handler(void);
DUMMY_HANDLER void SVC_Handler(void);
DUMMY_HANDLER void DebugMon_Handler(void);
DUMMY_HANDLER void PendSV_Handler(void);
DUMMY_HANDLER void SysTick_Handler(void);
DUMMY_HANDLER void TIMER0_IRQ_0_IRQHandler(void);
DUMMY_HANDLER void TIMER0_IRQ_1_IRQHandler(void);
DUMMY_HANDLER void TIMER0_IRQ_2_IRQHandler(void);
DUMMY_HANDLER void TIMER0_IRQ_3_IRQHandler(void);
DUMMY_HANDLER void TIMER1_IRQ_0_IRQHandler(void);
DUMMY_HANDLER void TIMER1_IRQ_1_IRQHandler(void);
DUMMY_HANDLER void TIMER1_IRQ_2_IRQHandler(void);
DUMMY_HANDLER void TIMER1_IRQ_3_IRQHandler(void);
DUMMY_HANDLER void PWM_IRQ_WRAP_0_IRQHandler(void);
DUMMY_HANDLER void PWM_IRQ_WRAP_1_IRQHandler(void);
DUMMY_HANDLER void DMA_IRQ_0_IRQHandler(void);
DUMMY_HANDLER void DMA_IRQ_1_IRQHandler(void);
DUMMY_HANDLER void DMA_IRQ_2_IRQHandler(void);
DUMMY_HANDLER void DMA_IRQ_3_IRQHandler(void);
DUMMY_HANDLER void USBCTRL_IRQ_IRQHandler(void);
DUMMY_HANDLER void PIO0_IRQ_0_IRQHandler(void);
DUMMY_HANDLER void PIO0_IRQ_1_IRQHandler(void);
DUMMY_HANDLER void PIO1_IRQ_0_IRQHandler(void);
DUMMY_HANDLER void PIO1_IRQ_1_IRQHandler(void);
DUMMY_HANDLER void PIO2_IRQ_0_IRQHandler(void);
DUMMY_HANDLER void PIO2_IRQ_1_IRQHandler(void);
DUMMY_HANDLER void IO_IRQ_BANK0_IRQHandler(void);
DUMMY_HANDLER void IO_IRQ_BANK0_NS_IRQHandler(void);
DUMMY_HANDLER void IO_IRQ_QSPI_IRQHandler(void);
DUMMY_HANDLER void IO_IRQ_QSPI_NS_IRQHandler(void);
DUMMY_HANDLER void SIO_IRQ_FIFO_IRQHandler(void);
DUMMY_HANDLER void SIO_IRQ_BELL_IRQHandler(void);
DUMMY_HANDLER void SIO_IRQ_FIFO_NS_IRQHandler(void);
DUMMY_HANDLER void SIO_IRQ_BELL_NS_IRQHandler(void);
DUMMY_HANDLER void SIO_IRQ_MTIMECMP_IRQHandler(void);
DUMMY_HANDLER void CLOCKS_IRQ_IRQHandler(void);
DUMMY_HANDLER void SPI0_IRQ_IRQHandler(void);
DUMMY_HANDLER void SPI1_IRQ_IRQHandler(void);
DUMMY_HANDLER void UART0_IRQ_IRQHandler(void);
DUMMY_HANDLER void UART1_IRQ_IRQHandler(void);
DUMMY_HANDLER void ADC_IRQ_FIFO_IRQHandler(void);
DUMMY_HANDLER void I2C0_IRQ_IRQHandler(void);
DUMMY_HANDLER void I2C1_IRQ_IRQHandler(void);
DUMMY_HANDLER void OTP_IRQ_IRQHandler(void);
DUMMY_HANDLER void TRNG_IRQ_IRQHandler(void);
DUMMY_HANDLER void PROC0_IRQ_CTI_IRQHandler(void);
DUMMY_HANDLER void PROC1_IRQ_CTI_IRQHandler(void);
DUMMY_HANDLER void PLL_SYS_IRQ_IRQHandler(void);
DUMMY_HANDLER void PLL_USB_IRQ_IRQHandler(void);
DUMMY_HANDLER void POWMAN_IRQ_POW_IRQHandler(void);
DUMMY_HANDLER void POWMAN_IRQ_TIMER_IRQHandler(void);
DUMMY_HANDLER void SPARE_IRQ_0_IRQHandler(void);
DUMMY_HANDLER void SPARE_IRQ_1_IRQHandler(void);
DUMMY_HANDLER void SPARE_IRQ_2_IRQHandler(void);
DUMMY_HANDLER void SPARE_IRQ_3_IRQHandler(void);
DUMMY_HANDLER void SPARE_IRQ_4_IRQHandler(void);
DUMMY_HANDLER void SPARE_IRQ_5_IRQHandler(void);
#endif  // SYS_USE_VECTORS > 0

// Interrupt vector table (at the beginning of the flash, used by the boot ROM)
void (*const vectors[])(void) = {
  &_estack,                       //  0 - Initial Stack Pointer Value

  // Cortex-M33 handlers
  Reset_Handler,                  //  1 - Reset
  #if SYS_USE_VECTORS > 0
  NMI_Handler,                    //  2 - NMI
  HardFault_Handler,              //  3 - Hard Fault
  MemManage_Handler,              //  4 - Memory Management Fault (MPU)
  BusFault_Handler,               //  5 - Bus Fault
  UsageFault_Handler,             //  6 - Usage Fault
  SecureFault_Handler,            //  7 - Secure Fault
  0,                              //  8 - Reserved
  0,                              //  9 - Reserved
  0,                              // 10 - Reserved
  SVC_Handler,                    // 11 - SVCall
  DebugMon_Handler,               // 12 - Debug Monitor
  0,                              // 13 - Reserved
  PendSV_Handler,                 // 14 - PendSV
  SysTick_Handler,                // 15 - SysTick

  // Peripheral handlers (both cores use the same table, each core has its own NVIC)
  TIMER0_IRQ_0_IRQHandler,        //  0 - TIMER0 alarm 0
  TIMER0_IRQ_1_IRQHandler,        //  1 - TIMER0 alarm 1
  TIMER0_IRQ_2_IRQHandler,        //  2 - TIMER0 alarm 2
  TIMER0_IRQ_3_IRQHandler,        //  3 - TIMER0 alarm 3
  TIMER1_IRQ_0_IRQHandler,        //  4 - TIMER1 alarm 0
  TIMER1_IRQ_1_IRQHandler,        //  5 - TIMER1 alarm 1
  TIMER1_IRQ_2_IRQHandler,        //  6 - TIMER1 alarm 2
  TIMER1_IRQ_3_IRQHandler,        //  7 - TIMER1 alarm 3
  PWM_IRQ_WRAP_0_IRQHandler,      //  8 - PWM counter wrap (IRQ 0)
  PWM_IRQ_WRAP_1_IRQHandler,      //  9 - PWM counter wrap (IRQ 1)
  DMA_IRQ_0_IRQHandler,           // 10 - DMA IRQ 0
  DMA_IRQ_1_IRQHandler,           // 11 - DMA IRQ 1
  DMA_IRQ_2_IRQHandler,           // 12 - DMA IRQ 2
  DMA_IRQ_3_IRQHandler,           // 13 - DMA IRQ 3
  USBCTRL_IRQ_IRQHandler,         // 14 - USB controller
  PIO0_IRQ_0_IRQHandler,          // 15 - PIO0 IRQ 0
  PIO0_IRQ_1_IRQHandler,          // 16 - PIO0 IRQ 1
  PIO1_IRQ_0_IRQHandler,          // 17 - PIO1 IRQ 0
  PIO1_IRQ_1_IRQHandler,          // 18 - PIO1 IRQ 1
  PIO2_IRQ_0_IRQHandler,          // 19 - PIO2 IRQ 0
  PIO2_IRQ_1_IRQHandler,          // 20 - PIO2 IRQ 1
  IO_IRQ_BANK0_IRQHandler,        // 21 - GPIO (pin interrupts)
  IO_IRQ_BANK0_NS_IRQHandler,     // 22 - GPIO (non-secure)
  IO_IRQ_QSPI_IRQHandler,         // 23 - QSPI GPIO
  IO_IRQ_QSPI_NS_IRQHandler,      // 24 - QSPI GPIO (non-secure)
  SIO_IRQ_FIFO_IRQHandler,        // 25 - SIO inter-core FIFO (of the respective core)
  SIO_IRQ_BELL_IRQHandler,        // 26 - SIO doorbell
  SIO_IRQ_FIFO_NS_IRQHandler,     // 27 - SIO inter-core FIFO (non-secure)
  SIO_IRQ_BELL_NS_IRQHandler,     // 28 - SIO doorbell (non-secure)
  SIO_IRQ_MTIMECMP_IRQHandler,    // 29 - SIO RISC-V platform timer
  CLOCKS_IRQ_IRQHandler,          // 30 - clocks (resus)
  SPI0_IRQ_IRQHandler,            // 31 - SPI0
  SPI1_IRQ_IRQHandler,            // 32 - SPI1
  UART0_IRQ_IRQHandler,           // 33 - UART0
  UART1_IRQ_IRQHandler,           // 34 - UART1
  ADC_IRQ_FIFO_IRQHandler,        // 35 - ADC FIFO
  I2C0_IRQ_IRQHandler,            // 36 - I2C0
  I2C1_IRQ_IRQHandler,            // 37 - I2C1
  OTP_IRQ_IRQHandler,             // 38 - OTP
  TRNG_IRQ_IRQHandler,            // 39 - true random number generator
  PROC0_IRQ_CTI_IRQHandler,       // 40 - core 0 cross trigger interface
  PROC1_IRQ_CTI_IRQHandler,       // 41 - core 1 cross trigger interface
  PLL_SYS_IRQ_IRQHandler,         // 42 - PLL_SYS loss of lock
  PLL_USB_IRQ_IRQHandler,         // 43 - PLL_USB loss of lock
  POWMAN_IRQ_POW_IRQHandler,      // 44 - power manager
  POWMAN_IRQ_TIMER_IRQHandler,    // 45 - AON timer alarm (RTC alarm)
  SPARE_IRQ_0_IRQHandler,         // 46 - software interrupt
  SPARE_IRQ_1_IRQHandler,         // 47 - software interrupt
  SPARE_IRQ_2_IRQHandler,         // 48 - software interrupt
  SPARE_IRQ_3_IRQHandler,         // 49 - software interrupt
  SPARE_IRQ_4_IRQHandler,         // 50 - software interrupt
  SPARE_IRQ_5_IRQHandler,         // 51 - software interrupt
  #endif  // SYS_USE_VECTORS > 0
};

// Reset handler (core 0, called by the boot ROM)
void Reset_Handler(void) {
  uint32_t *src, *dst;

  // The stack pointer has already been set from the vector table by the boot ROM.
  // Remove stack limit, configure vector table location
  __set_MSPLIM(0);
  SCB->VTOR = (uint32_t)vectors;

  // Enable FPU (coprocessors CP10 and CP11)
  #if SYS_FPU_EN > 0
  SCB->CPACR |= (uint32_t)0xF << 20;
  __DSB();
  __ISB();
  #endif

  // Copy data (and RAM functions) from FLASH to RAM
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
