// ===================================================================================
// Basic System Functions for CH558 and CH559                                 * v1.2 *
// ===================================================================================
//
// System clock (CLK) functions available:
// ---------------------------------------
// CLK_config()             set system clock frequency according to F_CPU
// CLK_external()           set external crystal as clock source (waits for start-up)
// CLK_internal()           set internal oscillator as clock source
//
// Watchdog Timer (WDT) functions available:
// -----------------------------------------
// WDT_start()              start watchdog timer with full period
// WDT_stop()               stop watchdog timer
// WDT_reset()              reload watchdog timer with full period
// WDT_set(time)            reload watchdog timer with specified time in ms
// WDT_feed(value)          reload watchdog timer with specified value
//
// Reset (RST) and bootloader functions available:
// -----------------------------------------------
// RST_now()                perform software reset
// RST_keep(value)          keep this value after RESET
// RST_getKeep()            read the keeped value
// RST_wasWDT()             check if last RESET was caused by watchdog timer
// RST_wasPIN()             check if last RESET was caused by RST PIN
// RST_wasPWR()             check if last RESET was caused by power-on
// RST_wasSOFT()            check if last RESET was caused by software
//
// BOOT_prepare()           prepare peripherals (USB, interrupts) before jumping to bootloader
// BOOT_now()               enter bootloader
//
// Sleep and power functions available:
// ------------------------------------
// SLEEP_now()              put device into sleep
//
// WAKE_enable(source)      enable wake-up from sleep source (sources see below)
// WAKE_disable(source)     disable wake-up from sleep source
// WAKE_all_disable()       disable all wake-up sources
//
// WAKE_USB_enable()        enable wake-up by USB event
// WAKE_RXD0_enable()       enable wake-up by RXD0 low level
// WAKE_RXD1_enable()       enable wake-up by RXD1 low level
// WAKE_CAP3_enable()       enable wake-up by Timer3 capture input low level
// WAKE_P03_enable()        enable wake-up by pin P0.3 low level
// WAKE_P14_enable()        enable wake-up by pin P1.4 low level
// WAKE_P15_enable()        enable wake-up by pin P1.5 low level
// WAKE_INT_enable()        enable wake-up by pin P3.2 edge or pin P3.3 low level
// (WAKE_xxx_disable() functions accordingly)
//
// CLKOFF_set(mask)         switch off clocks of unused peripherals (saves power):
//                          CLKOFF_USB, CLKOFF_ADC, CLKOFF_UART1, CLKOFF_SPI0,
//                          CLKOFF_TMR3, CLKOFF_LED, CLKOFF_XRAM (can be or-ed)
// CLKOFF_clear(mask)       switch on clocks of peripherals again
//
// Interrupt (INT) functions available:
// ------------------------------------
// INT_enable()             global interrupt enable
// INT_disable()            global interrupt disable
// INT_ATOMIC_BLOCK { }     execute block without being interrupted (restores EA state;
//                          do not leave the block with break, return or goto)
//
// Chip identification (ID) functions available:
// ---------------------------------------------
// ID_CHIP()                read chip ID (0x58: CH558, 0x59: CH559)
// ID_read()                read 32-bit unique chip ID
//
// Notes:
// ------
// - F_CPU must be 56, 48, 32, 24, 16, 12, 8, 6, 4 or 3 MHz (with internal 12MHz
//   oscillator or 12MHz crystal). USB needs at least 6MHz.
// - Registers CLOCK_CFG, PLL_CFG, GLOBAL_CFG, WAKE_CTRL and SLEEP_CTRL can only be
//   written in safe mode, which ends after a few clock cycles.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch559.h"

// ===================================================================================
// Safe Mode (internal)
// ===================================================================================
#define SAFE_MOD_enter()  do {SAFE_MOD = 0x55; SAFE_MOD = 0xAA;} while(0)
#define SAFE_MOD_exit()   SAFE_MOD = 0x00

// ===================================================================================
// System Clock (CLK) Functions
// ===================================================================================
// Fpll = 12MHz * PLL_MULT, Fusb4x = Fpll / USB_DIV = 48MHz, Fsys = Fpll / SYS_DIV
#if F_CPU == 56000000
  #define CLK_PLL_VAL     ((7 << 5) | 28)     // Fpll = 336MHz, USB: 336 / 7 = 48MHz
  #define CLK_DIV_VAL     6
#else
  #define CLK_PLL_VAL     ((2 << 5) |  8)     // Fpll =  96MHz, USB:  96 / 2 = 48MHz
  #if   F_CPU == 48000000
    #define CLK_DIV_VAL   2
  #elif F_CPU == 32000000
    #define CLK_DIV_VAL   3
  #elif F_CPU == 24000000
    #define CLK_DIV_VAL   4
  #elif F_CPU == 16000000
    #define CLK_DIV_VAL   6
  #elif F_CPU == 12000000
    #define CLK_DIV_VAL   8
  #elif F_CPU ==  8000000
    #define CLK_DIV_VAL   12
  #elif F_CPU ==  6000000
    #define CLK_DIV_VAL   16
  #elif F_CPU ==  4000000
    #define CLK_DIV_VAL   24
  #elif F_CPU ==  3000000
    #define CLK_DIV_VAL   0                   // 0 means 32
  #else
    #error F_CPU invalid or not set (56/48/32/24/16/12/8/6/4/3MHz)
  #endif
#endif

inline void CLK_config(void) {
  uint8_t cfg = (CLOCK_CFG & ~MASK_SYS_CK_DIV) | CLK_DIV_VAL; // keep oscillator bits
  SAFE_MOD_enter();
  PLL_CFG   = (uint8_t)CLK_PLL_VAL;             // set PLL and USB clock
  CLOCK_CFG = cfg;                              // set system clock divider
  SAFE_MOD_exit();
}

inline void CLK_external(void) {
  uint16_t i;
  SAFE_MOD_enter();
  CLOCK_CFG |=  bOSC_EN_XT;                     // enable external crystal
  SAFE_MOD_exit();
  for(i = F_CPU / 2000; i; i--) __asm__("nop"); // wait > 5ms for crystal start-up
  SAFE_MOD_enter();
  CLOCK_CFG &= ~bOSC_EN_INT;                    // switch to external crystal
  SAFE_MOD_exit();
}

inline void CLK_internal(void) {
  SAFE_MOD_enter();
  CLOCK_CFG |=  bOSC_EN_INT;                    // turn on and select internal oscillator
  SAFE_MOD_exit();
  SAFE_MOD_enter();
  CLOCK_CFG &= ~bOSC_EN_XT;                     // disable external crystal
  SAFE_MOD_exit();
}

#define CLK_inernal       CLK_internal          // old (misspelled) name

// ===================================================================================
// Watchdog Timer (WDT) Functions
// ===================================================================================
// Watchdog counter increments with Fsys/262144, timeout = (256 - WDOG_COUNT) * 262144 / Fsys
#define WDT_reset()       WDOG_COUNT = 0
#define WDT_feed(value)   WDOG_COUNT = (value)
#define WDT_set(time)     WDOG_COUNT = (uint8_t)(256 - (((F_CPU / 1000) * (uint32_t)(time)) >> 18))

inline void WDT_start(void) {
  WDOG_COUNT  = 0;
  SAFE_MOD_enter();
  GLOBAL_CFG |= bWDOG_EN;
  SAFE_MOD_exit();
}

inline void WDT_stop(void) {
  SAFE_MOD_enter();
  GLOBAL_CFG &= ~bWDOG_EN;
  SAFE_MOD_exit();
}

// ===================================================================================
// Reset (RST) Functions
// ===================================================================================
#define RST_keep(value)   RESET_KEEP = (value)
#define RST_getKeep()     (RESET_KEEP)
#define RST_wasWDT()      ((PCON & MASK_RST_FLAG) == RST_FLAG_WDOG)
#define RST_wasPIN()      ((PCON & MASK_RST_FLAG) == RST_FLAG_PIN)
#define RST_wasPWR()      ((PCON & MASK_RST_FLAG) == RST_FLAG_POR)
#define RST_wasSOFT()     ((PCON & MASK_RST_FLAG) == RST_FLAG_SW)

inline void RST_now(void) {
  SAFE_MOD_enter();
  GLOBAL_CFG |= bSW_RESET;
}

// ===================================================================================
// Bootloader (BOOT) Functions
// ===================================================================================
inline void BOOT_prepare(void) {
  EA         = 0;                               // disable interrupts
  USB_INT_EN = 0;                               // disable USB interrupts
  USB_CTRL   = 0x06;                            // reset USB module
}

inline void BOOT_now(void) {
  __asm
    ljmp #BOOT_LOAD_ADDR
  __endasm;
}

// ===================================================================================
// Sleep and Power Functions
// ===================================================================================
#define SLEEP_now()   PCON |= PD

#define WAKE_USB      bWAK_BY_USB     // wake-up by USB event
#define WAKE_RXD0     bWAK_RXD0_LO    // wake-up by RXD0 low level
#define WAKE_RXD1     bWAK_RXD1_LO    // wake-up by RXD1 low level
#define WAKE_CAP3     bWAK_CAP3_LO    // wake-up by Timer3 low-level capture
#define WAKE_P03      bWAK_P0_3_LO    // wake-up by pin P0.3 low level
#define WAKE_P14      bWAK_P1_4_LO    // wake-up by pin P1.4 low level
#define WAKE_P15      bWAK_P1_5_LO    // wake-up by pin P1.5 low level
#define WAKE_INT      bWAK_P3_2E_3L   // wake-up by pin P3.2 (INT0) edge or pin P3.3 (INT1) low level

// WAKE_CTRL can only be written in safe mode
#define WAKE_enable(source)     do {SAFE_MOD_enter(); WAKE_CTRL |=  (source); SAFE_MOD_exit();} while(0)
#define WAKE_disable(source)    do {SAFE_MOD_enter(); WAKE_CTRL &= ~(source); SAFE_MOD_exit();} while(0)
#define WAKE_all_disable()      do {SAFE_MOD_enter(); WAKE_CTRL  =  0;        SAFE_MOD_exit();} while(0)

#define WAKE_USB_enable()       WAKE_enable(bWAK_BY_USB)
#define WAKE_RXD0_enable()      WAKE_enable(bWAK_RXD0_LO)
#define WAKE_RXD1_enable()      WAKE_enable(bWAK_RXD1_LO)
#define WAKE_CAP3_enable()      WAKE_enable(bWAK_CAP3_LO)
#define WAKE_P03_enable()       WAKE_enable(bWAK_P0_3_LO)
#define WAKE_P14_enable()       WAKE_enable(bWAK_P1_4_LO)
#define WAKE_P15_enable()       WAKE_enable(bWAK_P1_5_LO)
#define WAKE_INT_enable()       WAKE_enable(bWAK_P3_2E_3L)

#define WAKE_USB_disable()      WAKE_disable(bWAK_BY_USB)
#define WAKE_RXD0_disable()     WAKE_disable(bWAK_RXD0_LO)
#define WAKE_RXD1_disable()     WAKE_disable(bWAK_RXD1_LO)
#define WAKE_CAP3_disable()     WAKE_disable(bWAK_CAP3_LO)
#define WAKE_P03_disable()      WAKE_disable(bWAK_P0_3_LO)
#define WAKE_P14_disable()      WAKE_disable(bWAK_P1_4_LO)
#define WAKE_P15_disable()      WAKE_disable(bWAK_P1_5_LO)
#define WAKE_INT_disable()      WAKE_disable(bWAK_P3_2E_3L)

// Peripheral clock gating (SLEEP_CTRL can only be written in safe mode)
#define CLKOFF_USB              bSLP_OFF_USB
#define CLKOFF_ADC              bSLP_OFF_ADC
#define CLKOFF_UART1            bSLP_OFF_UART1
#define CLKOFF_SPI0             bSLP_OFF_SPI0
#define CLKOFF_TMR3             bSLP_OFF_TMR3
#define CLKOFF_LED              bSLP_OFF_LED
#define CLKOFF_XRAM             bSLP_OFF_XRAM
#define CLKOFF_set(mask)        do {SAFE_MOD_enter(); SLEEP_CTRL |=  (mask); SAFE_MOD_exit();} while(0)
#define CLKOFF_clear(mask)      do {SAFE_MOD_enter(); SLEEP_CTRL &= ~(mask); SAFE_MOD_exit();} while(0)

// ===================================================================================
// Interrupt (INT) Functions
// ===================================================================================
#define INT_enable()        EA = 1
#define INT_disable()       EA = 0
#define INT_ATOMIC_BLOCK    for(uint8_t _ea_save = EA, _ea_once = (EA = 0, 1); \
                                _ea_once; EA = _ea_save, _ea_once = 0)

// ===================================================================================
// Chip Identification (ID) Functions
// ===================================================================================
#define ID_CHIP()           (CHIP_ID)

// Unique ID is located at code address 0x20, readable with E_DIS = 1 and EA = 0
inline uint32_t ID_read(void) {
  uint32_t id;
  uint8_t ea = EA;
  EA    = 0;
  E_DIS = 1;
  id    = *(__code uint32_t*)0x0020;
  E_DIS = 0;
  EA    = ea;
  return id;
}
