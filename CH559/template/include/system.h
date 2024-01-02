// ===================================================================================
// Basic System Functions for CH559                                           * v1.1 *
// ===================================================================================
//
// System clock (CLK)functions available:
// --------------------------------------
// CLK_config()             set system clock frequency according to F_CPU
// CLK_external()           set external crystal as clock source
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
// BOOT_now()               enter bootloader
//
// Sleep functions available:
// --------------------------
// SLEEP_now()              put device into sleep
//
// WAKE_enable(source)      enable wake-up from sleep source (sources see below)
// WAKE_disable(source)     disable wake-up from sleep source
// WAKE_all_disable()       disable all wake-up sources
//
// WAKE_USB_enable()        enable wake-up by USB event
// WAKE_RXD0_enable()       enable wake-up by RXD0 low level
// WAKE_RXD1_enable()       enable wake-up by RXD1 low level
// WAKE_P13_enable()        enable wake-up by pin P1.3 low level
// WAKE_P14_enable()        enable wake-up by pin P1.4 low level
// WAKE_P15_enable()        enable wake-up by pin P1.5 low level
// WAKE_RST_enable()        enable wake-up by pin RST high level
// WAKE_INT_enable()        enable wake-up by pin P3.2 edge or pin P3.3 low level
//
// WAKE_USB_disable()       disable wake-up by USB event
// WAKE_RXD0_disable()      disable wake-up by RXD0 low level
// WAKE_RXD1_disable()      disable wake-up by RXD1 low level
// WAKE_P13_disable()       disable wake-up by pin P1.3 low level
// WAKE_P14_disable()       disable wake-up by pin P1.4 low level
// WAKE_P15_disable()       disable wake-up by pin P1.5 low level
// WAKE_RST_disable()       disable wake-up by pin RST high level
// WAKE_INT_disable()       disable wake-up by pin P3.2 edge or pin P3.3 low level
//
// Wake-up from SLEEP sources:
// ---------------------------
// WAKE_USB                 wake-up by USB event
// WAKE_RXD0                wake-up by RXD0 low level
// WAKE_RXD1                wake-up by RXD1 low level
// WAKE_P13                 wake-up by pin P1.3 low level
// WAKE_P14                 wake-up by pin P1.4 low level
// WAKE_P15                 wake-up by pin P1.5 low level
// WAKE_RST                 wake-up by pin RST high level
// WAKE_INT                 wake-up by pin P3.2 edge or pin P3.3 low level
//
// Interrupt (INT) functions available:
// ------------------------------------
// INT_enable()             global interrupt enable
// INT_disable()            global interrupt disable
// INT_ATOMIC_BLOCK { }     execute block without being interrupted
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch559.h"

// ===================================================================================
// System Clock (CLK) Functions
// ===================================================================================
inline void CLK_config(void) {
  SAFE_MOD = 0x55;
  SAFE_MOD = 0xAA;                              // enter safe mode

  #if F_CPU == 56000000
    PLL_CFG  = 0b11111100;                     // USB: 48MHz, PLL: 336MHz
  #else
    PLL_CFG  = 0b01001000;                     // USB: 48MHz, PLL:  96MHz
  #endif

  #if F_CPU   == 56000000
    CLOCK_CFG = bOSC_EN_INT | 6;
  #elif F_CPU == 48000000
    CLOCK_CFG = bOSC_EN_INT | 2;
  #elif F_CPU == 32000000
    CLOCK_CFG = bOSC_EN_INT | 3;
  #elif F_CPU == 24000000
    CLOCK_CFG = bOSC_EN_INT | 4;
  #elif F_CPU == 16000000
    CLOCK_CFG = bOSC_EN_INT | 6;
  #elif F_CPU == 12000000
    CLOCK_CFG = bOSC_EN_INT | 8;
  #elif F_CPU ==  8000000
    CLOCK_CFG = bOSC_EN_INT | 12;
  #elif F_CPU ==  6000000
    CLOCK_CFG = bOSC_EN_INT | 16;
  #elif F_CPU ==  4000000
    CLOCK_CFG = bOSC_EN_INT | 24;
  #elif F_CPU ==  3000000
    CLOCK_CFG = bOSC_EN_INT | 0;
  #else
    #warning F_CPU invalid or not set
  #endif

  SAFE_MOD = 0x00;                              // terminate safe mode
}

inline void CLK_external(void) {
  SAFE_MOD = 0x55;
  SAFE_MOD = 0xAA;                              // enter safe mode
  CLOCK_CFG |=  bOSC_EN_XT;                     // enable external crystal
  CLOCK_CFG &= ~bOSC_EN_INT;                    // turn off internal oscillator
  SAFE_MOD = 0x00;                              // terminate safe mode
}

inline void CLK_inernal(void) {
  SAFE_MOD = 0x55;
  SAFE_MOD = 0xAA;                              // enter safe mode
  CLOCK_CFG |=  bOSC_EN_INT;                    // turn on internal oscillator
  CLOCK_CFG &= ~bOSC_EN_XT;                     // disable external crystal
  SAFE_MOD = 0x00;                              // terminate safe mode
}

// ===================================================================================
// Watchdog Timer (WDT) Functions
// ===================================================================================
#define WDT_reset()       WDOG_COUNT = 0
#define WDT_feed(value)   WDOG_COUNT = value
#define WDT_set(time)     WDOG_COUNT = (uint8_t)(256 - ((F_CPU / 1000) * time / 262144))

inline void WDT_start(void) {
  WDOG_COUNT  = 0;
  SAFE_MOD    = 0x55;
  SAFE_MOD    = 0xAA;
  GLOBAL_CFG |= bWDOG_EN;
  SAFE_MOD    = 0x00;
}

inline void WDT_stop(void) {
  SAFE_MOD    = 0x55;
  SAFE_MOD    = 0xAA; 
  GLOBAL_CFG &= ~bWDOG_EN;
  SAFE_MOD    = 0x00;
}

// ===================================================================================
// Reset (RST) Functions
// ===================================================================================
#define RST_keep(value)   RESET_KEEP = value
#define RST_getKeep()     (RESET_KEEP)
#define RST_wasWDT()      ((PCON & MASK_RST_FLAG) == RST_FLAG_WDOG)
#define RST_wasPIN()      ((PCON & MASK_RST_FLAG) == RST_FLAG_PIN)
#define RST_wasPWR()      ((PCON & MASK_RST_FLAG) == RST_FLAG_POR)
#define RST_wasSOFT()     ((PCON & MASK_RST_FLAG) == RST_FLAG_SW)

inline void RST_now(void) {
  SAFE_MOD    = 0x55;
  SAFE_MOD    = 0xAA;
  GLOBAL_CFG |= bSW_RESET;
}

// ===================================================================================
// Bootloader (BOOT) Functions
// ===================================================================================
inline void BOOT_now(void) {
  __asm
    ljmp #BOOT_LOAD_ADDR
  __endasm;
}

// ===================================================================================
// Sleep Functions
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

#define WAKE_enable(source)     WAKE_CTRL |=  source
#define WAKE_disable(source)    WAKE_CTRL &= ~source
#define WAKE_all_disable()      WAKE_CTRL  =  0

#define WAKE_USB_enable()       WAKE_CTRL |=  bWAK_BY_USB
#define WAKE_RXD0_enable()      WAKE_CTRL |=  bWAK_RXD0_LO
#define WAKE_RXD1_enable()      WAKE_CTRL |=  bWAK_RXD1_LO
#define WAKE_CAP3_enable()      WAKE_CTRL |=  bWAK_CAP3_LO
#define WAKE_P03_enable()       WAKE_CTRL |=  bWAK_P0_3_LO
#define WAKE_P14_enable()       WAKE_CTRL |=  bWAK_P1_4_LO
#define WAKE_P15_enable()       WAKE_CTRL |=  bWAK_P1_5_LO
#define WAKE_INT_enable()       WAKE_CTRL |=  bWAK_P3_2E_3L

#define WAKE_USB_disable()      WAKE_CTRL &= ~bWAK_BY_USB
#define WAKE_RXD0_disable()     WAKE_CTRL &= ~bWAK_RXD0_LO
#define WAKE_RXD1_disable()     WAKE_CTRL &= ~bWAK_RXD1_LO
#define WAKE_CAP3_disable()     WAKE_CTRL &= ~bWAK_CAP3_LO
#define WAKE_P03_disable()      WAKE_CTRL &= ~bWAK_P0_3_LO
#define WAKE_P14_disable()      WAKE_CTRL &= ~bWAK_P1_4_LO
#define WAKE_P15_disable()      WAKE_CTRL &= ~bWAK_P1_5_LO
#define WAKE_INT_disable()      WAKE_CTRL &= ~bWAK_P3_2E_3L

// ===================================================================================
// Interrupt (INT) Functions
// ===================================================================================
#define INT_enable()        EA = 1
#define INT_disable()       EA = 0
#define INT_ATOMIC_BLOCK    for(EA=0;!EA;EA=1)
