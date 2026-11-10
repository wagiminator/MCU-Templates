// ===================================================================================
// Basic System Functions for RP2040                                          * v1.0 *
// ===================================================================================
//
// This file must be included!!! The system configuration and the system clock are
// set up automatically on system start.

#include "system.h"

// ===================================================================================
// Setup Microcontroller (this function is called automatically at startup)
// ===================================================================================
void SYS_init(void) {
  // Hold all peripherals in reset except those needed for running from flash
  RESET_assert(~(RESETS_RESET_IO_QSPI | RESETS_RESET_PADS_QSPI | RESETS_RESET_SYSCFG
               | RESETS_RESET_PLL_SYS | RESETS_RESET_PLL_USB) & 0x01FFFFFF);

  // Init system clock
  #if SYS_CLK_INIT > 0
  CLK_init();
  #endif

  // Init SYSTICK
  #if SYS_TICK_INIT > 0
  SysTick->CTRL = SysTick_CTRL_ENABLE | SysTick_CTRL_CLKSOURCE;
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
  for(fbdiv = 1596000000 / XOSC_VALUE; fbdiv * XOSC_VALUE >= 750000000; fbdiv--) {
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

// Configure auxiliary clock (clk_peri, clk_usb, clk_adc, clk_rtc, clk_gpout):
// ctrl points to the CTRL register, the DIV register follows directly
static void CLK_setAux(volatile uint32_t* ctrl, uint32_t auxsrc, uint32_t div) {
  volatile uint32_t i;
  REG_CLR(*ctrl, CLOCKS_CLK_PERI_CTRL_ENABLE);      // stop clock
  for(i = 0; i < 500; i++);                         // wait for clock to stop (3 cycles)
  ctrl[1] = div << 8;                               // set integer divider
  *ctrl   = auxsrc;                                 // select clock source
  REG_SET(*ctrl, CLOCKS_CLK_PERI_CTRL_ENABLE);      // start clock
}

// Init system clock according to F_CPU
void CLK_init(void) {
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
  if(!PLL_init(PLL_SYS, F_CPU)) PLL_init(PLL_SYS, 125000000);
  #else
  PLL_stop(PLL_SYS);
  #endif

  // clk_ref = XOSC (12MHz)
  CLOCKS->CLK_REF_DIV  = 1 << 8;
  CLOCKS->CLK_REF_CTRL = CLOCKS_CLK_REF_CTRL_SRC_XOSC_CLKSRC;
  while(!(CLOCKS->CLK_REF_SELECTED & (1 << 2)));

  // clk_sys = PLL_SYS (or clk_ref = XOSC for 12MHz)
  CLOCKS->CLK_SYS_DIV  = 1 << 8;
  #if F_CPU != 12000000
  CLOCKS->CLK_SYS_CTRL = CLOCKS_CLK_SYS_CTRL_AUXSRC_CLKSRC_PLL_SYS;
  REG_SET(CLOCKS->CLK_SYS_CTRL, CLOCKS_CLK_SYS_CTRL_SRC);
  while(!(CLOCKS->CLK_SYS_SELECTED & 2));
  #endif

  // clk_peri = clk_sys, clk_usb = clk_adc = PLL_USB (48MHz), clk_rtc = 46875Hz
  CLK_setAux(&CLOCKS->CLK_PERI_CTRL, CLOCKS_CLK_PERI_CTRL_AUXSRC_CLK_SYS, 1);
  CLK_setAux(&CLOCKS->CLK_USB_CTRL,  CLOCKS_CLK_USB_CTRL_AUXSRC_CLKSRC_PLL_USB, 1);
  CLK_setAux(&CLOCKS->CLK_ADC_CTRL,  CLOCKS_CLK_ADC_CTRL_AUXSRC_CLKSRC_PLL_USB, 1);
  CLK_setAux(&CLOCKS->CLK_RTC_CTRL,  CLOCKS_CLK_RTC_CTRL_AUXSRC_CLKSRC_PLL_USB, F_USB / F_RTC);

  // Watchdog tick generator: 1us ticks for TIMER and watchdog (clk_ref = 12MHz)
  WATCHDOG->TICK = (F_REF / 1000000) | WATCHDOG_TICK_ENABLE;
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
#define MCO_GPOUT   ((MCO_PIN == GP21) ? 0 : (MCO_PIN - GP22))

void MCO_set(uint32_t src) {
  RESET_release(RESETS_RESET_IO_BANK0 | RESETS_RESET_PADS_BANK0);
  CLK_setAux(&CLOCKS->CLK_GPOUT[MCO_GPOUT].CTRL, src, MCO_DIV);
  REG_CLR(PADS_BANK0->GPIO[MCO_PIN], PADS_BANK0_GPIO_OD);
  IO_BANK0->GPIO[MCO_PIN].CTRL = GPIO_FUNC_GPCK;
}

// Disable clock output
void MCO_stop(void) {
  IO_BANK0->GPIO[MCO_PIN].CTRL = GPIO_FUNC_NULL;
  REG_CLR(CLOCKS->CLK_GPOUT[MCO_GPOUT].CTRL, CLOCKS_CLK_GPOUT_CTRL_ENABLE);
}

// ===================================================================================
// Real-Time Clock (RTC) Functions
// ===================================================================================

// Load SETUP registers into RTC (RTC is stopped during loading)
static void RTC_load(uint32_t date, uint32_t time) {
  REG_CLR(RTC->CTRL, RTC_CTRL_RTC_ENABLE);          // stop RTC
  while(RTC->CTRL & RTC_CTRL_RTC_ACTIVE);
  RTC->SETUP_0 = date;                              // year, month, day
  RTC->SETUP_1 = time;                              // weekday, hours, minutes, seconds
  REG_SET(RTC->CTRL, RTC_CTRL_LOAD);                // load values
  REG_SET(RTC->CTRL, RTC_CTRL_RTC_ENABLE);          // start RTC
  while(!RTC_isInit());
}

// Init and start RTC (clk_rtc = 46875Hz), date 2000-01-01, time 00:00:00
void RTC_init(void) {
  RESET_cycle(RESETS_RESET_RTC);
  RTC->CLKDIV_M1 = F_RTC - 1;                       // 1Hz RTC tick
  RTC_load(((uint32_t)RTC_YEAR_REF << RTC_SETUP_0_YEAR_Pos)
         | ((uint32_t)1 << RTC_SETUP_0_MONTH_Pos) | 1, (uint32_t)6 << RTC_SETUP_1_DOTW_Pos);
}

// Set time (hours, minutes, seconds), decimal values
void RTC_setTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
  uint32_t time = RTC_readTime();                   // RTC_0 must be read first
  uint32_t date = RTC_readDate();
  RTC_load(date, (time & RTC_RTC_0_DOTW_Msk)
               | ((uint32_t)hours   << RTC_SETUP_1_HOUR_Pos)
               | ((uint32_t)minutes << RTC_SETUP_1_MIN_Pos)
               | ((uint32_t)seconds << RTC_SETUP_1_SEC_Pos));
}

// Set date (year since RTC_YEAR_REF, month, day), decimal values. The day of the week
// is calculated automatically.
void RTC_setDate(uint8_t year, uint8_t month, uint8_t day) {
  static const uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  uint16_t y = RTC_YEAR_REF + year;
  uint8_t  dotw;
  uint32_t time = RTC_readTime();                   // RTC_0 must be read first
  (void)RTC_readDate();
  if(month < 3) y--;                                // day of the week (Sakamoto)
  dotw = (y + y/4 - y/100 + y/400 + t[(month - 1) % 12] + day) % 7;
  RTC_load(((uint32_t)(RTC_YEAR_REF + year) << RTC_SETUP_0_YEAR_Pos)
         | ((uint32_t)month << RTC_SETUP_0_MONTH_Pos) | day,
           (time & ~RTC_RTC_0_DOTW_Msk) | ((uint32_t)dotw << RTC_SETUP_1_DOTW_Pos));
}

// Set daily alarm (hours, minutes, seconds)
void RTC_setAlarm(uint8_t hours, uint8_t minutes, uint8_t seconds) {
  RTC_stopAlarm();
  while(RTC->IRQ_SETUP_0 & RTC_IRQ_SETUP_0_MATCH_ACTIVE);
  RTC->IRQ_SETUP_0 = 0;
  RTC->IRQ_SETUP_1 = RTC_IRQ_SETUP_1_HOUR_ENA | RTC_IRQ_SETUP_1_MIN_ENA | RTC_IRQ_SETUP_1_SEC_ENA
                   | ((uint32_t)hours   << RTC_IRQ_SETUP_1_HOUR_Pos)
                   | ((uint32_t)minutes << RTC_IRQ_SETUP_1_MIN_Pos)
                   | ((uint32_t)seconds << RTC_IRQ_SETUP_1_SEC_Pos);
  REG_SET(RTC->IRQ_SETUP_0, RTC_IRQ_SETUP_0_MATCH_ENA);
  while(!(RTC->IRQ_SETUP_0 & RTC_IRQ_SETUP_0_MATCH_ACTIVE));
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
  SysTick->LOAD = n;
  SysTick->VAL  = 0;
  while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG));
}

// ===================================================================================
// Watchdog Timer (IWDG/WDT) Functions
// ===================================================================================
uint32_t IWDG_loadValue;

// Watchdog resets everything except the oscillators
#define WDT_RESET_SCOPE   (0x0001FFFF & ~(PSM_WDSEL_ROSC | PSM_WDSEL_XOSC))

// Reload watchdog counter with n milliseconds (max 8300ms)
void IWDG_reload(uint16_t ms) {
  if(ms > 8300) ms = 8300;
  IWDG_loadValue = (uint32_t)ms * 2000;           // counter decrements twice per us
  IWDG_feed();
}

// Start watchdog timer with given time in milliseconds (max 8300ms)
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

// Put CPU into sleep for n milliseconds (uses TIMER alarm 3)
void SLEEP_ms(uint32_t ms) {
  RESET_release(RESETS_RESET_TIMER);
  NVIC_DisableIRQ(TIMER_IRQ_3_IRQn);
  REG_SET(TIMER->INTE, TIMER_INTE_ALARM_3);         // alarm interrupt (for wake-up)
  TIMER->ALARM[3] = TIMER->TIMERAWL + ms * 1000;    // arm alarm
  SCB->SCR |= SCB_SCR_SEVONPEND_Msk;                // pending interrupt generates event
  while(TIMER->ARMED & TIMER_ARMED_ARMED_Msk & (1 << 3)) SLEEP_WFE_now();
  SCB->SCR &= ~SCB_SCR_SEVONPEND_Msk;
  TIMER->INTR = TIMER_INTR_ALARM_3;                 // clear interrupt flag
  REG_CLR(TIMER->INTE, TIMER_INTE_ALARM_3);
  NVIC_ClearPendingIRQ(TIMER_IRQ_3_IRQn);
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
  REG_SET(PADS_BANK0->GPIO[pin], PADS_BANK0_GPIO_IE);
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

// Get pointer to boot ROM function with code c1,c2 (e.g. 'U','B')
void* ROM_func(char c1, char c2) {
  typedef void* (*rom_lookup_fn)(uint16_t* table, uint32_t code);
  volatile uint16_t* rom = (volatile uint16_t*)0x14;  // pointers to table and lookup
  __ASM volatile("" : "+r" (rom));                    // (hide address from compiler)
  rom_lookup_fn lookup = (rom_lookup_fn)(uint32_t)rom[2];
  return lookup((uint16_t*)(uint32_t)rom[0], (uint32_t)c1 | ((uint32_t)c2 << 8));
}

// Reset and enter USB bootloader (BOOTSEL mode, mass storage and PICOBOOT interface)
void BOOT_now(void) {
  typedef void (*reset_usb_boot_fn)(uint32_t gpio_mask, uint32_t disable_mask);
  reset_usb_boot_fn reset_usb_boot = (reset_usb_boot_fn)ROM_func('U', 'B');
  reset_usb_boot(0, 0);
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
                                 | ((uint32_t)0x3F << DMA_CH_CTRL_TRIG_TREQ_SEL_Pos)
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
// Second Stage Bootloader (boot2, first 256 bytes of the flash)
// ===================================================================================
// Configures the SSI for fast flash access (XIP) and jumps to the vector table at
// 0x10000100. Assembled from the Raspberry Pi Pico SDK (src/rp2040/boot_stage2,
// BSD-3-Clause, Copyright (c) 2020 Raspberry Pi (Trading) Ltd.), the last 4 bytes
// are the CRC32 checked by the boot ROM.
__attribute__((section(".boot2"), used))
const uint8_t SYS_boot2[256] = {
#if SYS_BOOT2 == 0
  // boot2_w25q080.S: W25Q080/W25Q16JV (Raspberry Pi Pico), quad I/O fast read (EBh),
  // continuous read mode, SPI clock = clk_sys / 4
  0x00, 0xB5, 0x32, 0x4B, 0x21, 0x20, 0x58, 0x60, 0x98, 0x68, 0x02, 0x21, 0x88, 0x43, 0x98, 0x60,
  0xD8, 0x60, 0x18, 0x61, 0x58, 0x61, 0x2E, 0x4B, 0x00, 0x21, 0x99, 0x60, 0x04, 0x21, 0x59, 0x61,
  0x01, 0x21, 0xF0, 0x22, 0x99, 0x50, 0x2B, 0x49, 0x19, 0x60, 0x01, 0x21, 0x99, 0x60, 0x35, 0x20,
  0x00, 0xF0, 0x44, 0xF8, 0x02, 0x22, 0x90, 0x42, 0x14, 0xD0, 0x06, 0x21, 0x19, 0x66, 0x00, 0xF0,
  0x34, 0xF8, 0x19, 0x6E, 0x01, 0x21, 0x19, 0x66, 0x00, 0x20, 0x18, 0x66, 0x1A, 0x66, 0x00, 0xF0,
  0x2C, 0xF8, 0x19, 0x6E, 0x19, 0x6E, 0x19, 0x6E, 0x05, 0x20, 0x00, 0xF0, 0x2F, 0xF8, 0x01, 0x21,
  0x08, 0x42, 0xF9, 0xD1, 0x00, 0x21, 0x99, 0x60, 0x1B, 0x49, 0x19, 0x60, 0x00, 0x21, 0x59, 0x60,
  0x1A, 0x49, 0x1B, 0x48, 0x01, 0x60, 0x01, 0x21, 0x99, 0x60, 0xEB, 0x21, 0x19, 0x66, 0xA0, 0x21,
  0x19, 0x66, 0x00, 0xF0, 0x12, 0xF8, 0x00, 0x21, 0x99, 0x60, 0x16, 0x49, 0x14, 0x48, 0x01, 0x60,
  0x01, 0x21, 0x99, 0x60, 0x01, 0xBC, 0x00, 0x28, 0x00, 0xD0, 0x00, 0x47, 0x12, 0x48, 0x13, 0x49,
  0x08, 0x60, 0x03, 0xC8, 0x80, 0xF3, 0x08, 0x88, 0x08, 0x47, 0x03, 0xB5, 0x99, 0x6A, 0x04, 0x20,
  0x01, 0x42, 0xFB, 0xD0, 0x01, 0x20, 0x01, 0x42, 0xF8, 0xD1, 0x03, 0xBD, 0x02, 0xB5, 0x18, 0x66,
  0x18, 0x66, 0xFF, 0xF7, 0xF2, 0xFF, 0x18, 0x6E, 0x18, 0x6E, 0x02, 0xBD, 0x00, 0x00, 0x02, 0x40,
  0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x07, 0x00, 0x00, 0x03, 0x5F, 0x00, 0x21, 0x22, 0x00, 0x00,
  0xF4, 0x00, 0x00, 0x18, 0x22, 0x20, 0x00, 0xA0, 0x00, 0x01, 0x00, 0x10, 0x08, 0xED, 0x00, 0xE0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0B, 0x8F, 0xD5,
#else
  // boot2_generic_03h.S: any 25-series flash, standard read (03h), SPI clock = clk_sys / 4
  0x00, 0xB5, 0x0C, 0x4B, 0x00, 0x21, 0x99, 0x60, 0x04, 0x21, 0x59, 0x61, 0x0A, 0x49, 0x19, 0x60,
  0x0A, 0x49, 0x0B, 0x48, 0x01, 0x60, 0x00, 0x21, 0x59, 0x60, 0x01, 0x21, 0x99, 0x60, 0x01, 0xBC,
  0x00, 0x28, 0x00, 0xD0, 0x00, 0x47, 0x07, 0x48, 0x07, 0x49, 0x08, 0x60, 0x03, 0xC8, 0x80, 0xF3,
  0x08, 0x88, 0x08, 0x47, 0x00, 0x00, 0x00, 0x18, 0x00, 0x03, 0x1F, 0x00, 0x18, 0x02, 0x00, 0x03,
  0xF4, 0x00, 0x00, 0x18, 0x00, 0x01, 0x00, 0x10, 0x08, 0xED, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0xEC, 0x21, 0x0D,
#endif
};

// ===================================================================================
// C version of the RP2040 Startup File
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
DUMMY_HANDLER void SVC_Handler(void);
DUMMY_HANDLER void PendSV_Handler(void);
DUMMY_HANDLER void SysTick_Handler(void);
DUMMY_HANDLER void TIMER_IRQ_0_IRQHandler(void);
DUMMY_HANDLER void TIMER_IRQ_1_IRQHandler(void);
DUMMY_HANDLER void TIMER_IRQ_2_IRQHandler(void);
DUMMY_HANDLER void TIMER_IRQ_3_IRQHandler(void);
DUMMY_HANDLER void PWM_IRQ_WRAP_IRQHandler(void);
DUMMY_HANDLER void USBCTRL_IRQ_IRQHandler(void);
DUMMY_HANDLER void XIP_IRQ_IRQHandler(void);
DUMMY_HANDLER void PIO0_IRQ_0_IRQHandler(void);
DUMMY_HANDLER void PIO0_IRQ_1_IRQHandler(void);
DUMMY_HANDLER void PIO1_IRQ_0_IRQHandler(void);
DUMMY_HANDLER void PIO1_IRQ_1_IRQHandler(void);
DUMMY_HANDLER void DMA_IRQ_0_IRQHandler(void);
DUMMY_HANDLER void DMA_IRQ_1_IRQHandler(void);
DUMMY_HANDLER void IO_IRQ_BANK0_IRQHandler(void);
DUMMY_HANDLER void IO_IRQ_QSPI_IRQHandler(void);
DUMMY_HANDLER void SIO_IRQ_PROC0_IRQHandler(void);
DUMMY_HANDLER void SIO_IRQ_PROC1_IRQHandler(void);
DUMMY_HANDLER void CLOCKS_IRQ_IRQHandler(void);
DUMMY_HANDLER void SPI0_IRQ_IRQHandler(void);
DUMMY_HANDLER void SPI1_IRQ_IRQHandler(void);
DUMMY_HANDLER void UART0_IRQ_IRQHandler(void);
DUMMY_HANDLER void UART1_IRQ_IRQHandler(void);
DUMMY_HANDLER void ADC_IRQ_FIFO_IRQHandler(void);
DUMMY_HANDLER void I2C0_IRQ_IRQHandler(void);
DUMMY_HANDLER void I2C1_IRQ_IRQHandler(void);
DUMMY_HANDLER void RTC_IRQ_IRQHandler(void);
DUMMY_HANDLER void SPARE_IRQ_0_IRQHandler(void);
DUMMY_HANDLER void SPARE_IRQ_1_IRQHandler(void);
DUMMY_HANDLER void SPARE_IRQ_2_IRQHandler(void);
DUMMY_HANDLER void SPARE_IRQ_3_IRQHandler(void);
DUMMY_HANDLER void SPARE_IRQ_4_IRQHandler(void);
DUMMY_HANDLER void SPARE_IRQ_5_IRQHandler(void);
#endif  // SYS_USE_VECTORS > 0

// Interrupt vector table (at flash offset 0x100, behind boot2)
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

  // Peripheral handlers (both cores use the same table, each core has its own NVIC)
  TIMER_IRQ_0_IRQHandler,         //  0 - TIMER alarm 0
  TIMER_IRQ_1_IRQHandler,         //  1 - TIMER alarm 1
  TIMER_IRQ_2_IRQHandler,         //  2 - TIMER alarm 2
  TIMER_IRQ_3_IRQHandler,         //  3 - TIMER alarm 3
  PWM_IRQ_WRAP_IRQHandler,        //  4 - PWM counter wrap
  USBCTRL_IRQ_IRQHandler,         //  5 - USB controller
  XIP_IRQ_IRQHandler,             //  6 - XIP
  PIO0_IRQ_0_IRQHandler,          //  7 - PIO0 IRQ 0
  PIO0_IRQ_1_IRQHandler,          //  8 - PIO0 IRQ 1
  PIO1_IRQ_0_IRQHandler,          //  9 - PIO1 IRQ 0
  PIO1_IRQ_1_IRQHandler,          // 10 - PIO1 IRQ 1
  DMA_IRQ_0_IRQHandler,           // 11 - DMA IRQ 0
  DMA_IRQ_1_IRQHandler,           // 12 - DMA IRQ 1
  IO_IRQ_BANK0_IRQHandler,        // 13 - GPIO (pin interrupts)
  IO_IRQ_QSPI_IRQHandler,         // 14 - QSPI GPIO
  SIO_IRQ_PROC0_IRQHandler,       // 15 - SIO core 0 (inter-core FIFO)
  SIO_IRQ_PROC1_IRQHandler,       // 16 - SIO core 1 (inter-core FIFO)
  CLOCKS_IRQ_IRQHandler,          // 17 - clocks (resus)
  SPI0_IRQ_IRQHandler,            // 18 - SPI0
  SPI1_IRQ_IRQHandler,            // 19 - SPI1
  UART0_IRQ_IRQHandler,           // 20 - UART0
  UART1_IRQ_IRQHandler,           // 21 - UART1
  ADC_IRQ_FIFO_IRQHandler,        // 22 - ADC FIFO
  I2C0_IRQ_IRQHandler,            // 23 - I2C0
  I2C1_IRQ_IRQHandler,            // 24 - I2C1
  RTC_IRQ_IRQHandler,             // 25 - RTC
  SPARE_IRQ_0_IRQHandler,         // 26 - software interrupt
  SPARE_IRQ_1_IRQHandler,         // 27 - software interrupt
  SPARE_IRQ_2_IRQHandler,         // 28 - software interrupt
  SPARE_IRQ_3_IRQHandler,         // 29 - software interrupt
  SPARE_IRQ_4_IRQHandler,         // 30 - software interrupt
  SPARE_IRQ_5_IRQHandler,         // 31 - software interrupt
  #endif  // SYS_USE_VECTORS > 0
};

// Reset handler (core 0, called by boot2)
void Reset_Handler(void) {
  uint32_t *src, *dst;

  // The stack pointer has already been set from the vector table by boot2.
  // Configure vector table location
  SCB->VTOR = (uint32_t)vectors;

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
