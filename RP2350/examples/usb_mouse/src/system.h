// ===================================================================================
// Basic System Functions for RP2350                                          * v1.0 *
// ===================================================================================
//
// This file must be included!!! The system configuration and the system clock are
// set up automatically on system start.
//
// System clock functions available:
// ---------------------------------
// CLK_init()               init system clock according to F_CPU (see below)
// CLK_reset()              reset system clock to default state (ROSC, approx. 11MHz)
//
// XOSC_enable()            enable crystal oscillator (XOSC, 12MHz)
// XOSC_disable()           disable XOSC
// XOSC_ready()             check if XOSC is stable
//
// PLL_init(pll,f)          init PLL (PLL_SYS or PLL_USB) with output frequency f in Hz,
//                          returns 0 if frequency cannot be generated exactly
// PLL_stop(pll)            power down PLL
// PLL_locked(pll)          check if PLL is locked
//
// MCO_setSYS()             output system clock / MCO_DIV on MCO pin (see below)
// MCO_setUSB()             output USB clock (48MHz) / MCO_DIV on MCO pin
// MCO_setREF()             output reference clock (12MHz) / MCO_DIV on MCO pin
// MCO_stop()               disable clock output (MCO)
//
// Real-Time Clock (RTC) functions available (calendar based on the AON timer):
// ----------------------------------------------------------------------------
// RTC_init()               init and start RTC (AON timer of POWMAN, clocked by XOSC)
// RTC_setTime(h,m,s)       set time (hours, minutes, seconds), decimal values
// RTC_setDate(y,m,d)       set date (year since RTC_YEAR_REF, month, day), decimal values
// RTC_readTime()           read time (must be post-processed, see below)
// RTC_readDate()           read date of the last RTC_readTime() call (post-process!)
// RTC_getSeconds(t)        get seconds of time value t (0..59)
// RTC_getMinutes(t)        get minutes of time value t (0..59)
// RTC_getHours(t)          get hours   of time value t (0..23)
// RTC_getWeekday(t)        get weekday of time value t (0: Sunday .. 6)
// RTC_getDay(d)            get day     of date value d (1..31)
// RTC_getMonth(d)          get month   of date value d (1..12)
// RTC_getYear(d)           get year    of date value d (e.g. 2026)
// RTC_isInit()             check if RTC (AON timer) is running
// RTC_getMillis()          get milliseconds since RTC_YEAR_REF-01-01 00:00:00 (64 bit)
//
// RTC_setAlarm(h,m,s)      set alarm for the next time h:m:s (hours, minutes, seconds)
// RTC_stopAlarm()          stop alarm
// RTC_readAlarmFlag()      check alarm flag
// RTC_clearAlarmFlag()     clear alarm flag (also stops the alarm, restart with RTC_setAlarm)
// RTC_enableAlarmInt()     enable alarm interrupt (POWMAN_IRQ_TIMER_IRQHandler)
// RTC_disableAlarmInt()    disable alarm interrupt
//
// Delay (DLY) and SysTick (STK) functions available:
// --------------------------------------------------
// DLY_ticks(n)             delay n+1 clock cycles (n = 0: no delay)
// DLY_us(n)                delay n microseconds
// DLY_ms(n)                delay n milliseconds
//
// STK_enable()             enable SYSTICK at system frequency
// STK_disable()            disable SYSTICK
//
// Watchdog Timer (IWDG/WDT) functions available:
// ----------------------------------------------
// IWDG_start(n)            start watchdog timer, n milliseconds, n<=16700
// IWDG_reload(n)           reload watchdog counter with n milliseconds, n<=16700
// IWDG_feed()              feed the dog (reload last time)
// IWDG_stop()              stop watchdog timer
//
// Sleep functions available:
// --------------------------
// SLEEP_WFI_now()          put CPU into sleep, wake up by interrupt
// SLEEP_WFE_now()          put CPU into sleep, wake up by event
// SLEEP_ms(n)              put CPU into sleep for n milliseconds (TIMER0 alarm 3)
// DORMANT_now(PIN,TYPE)    put device into DORMANT (all clocks stopped), wake up by
//                          PIN_INT_RISING, PIN_INT_FALLING, PIN_INT_HIGH or PIN_INT_LOW
//                          event on PIN; the system clock is restarted afterwards
//
// Reset (RST) and Bootloader (BOOT) functions available:
// ------------------------------------------------------
// BOOT_now()               reset and enter USB bootloader (BOOTSEL mode)
// RST_now()                conduct software reset (via watchdog, whole chip)
// RST_clearFlags()         clear reset flags
// RST_wasIWDG()            check if last reset was caused by watchdog timeout
// RST_wasSoftware()        check if last reset was caused by software (RST_now)
// RST_wasPower()           check if last reset was caused by power-on/brown-out
// RST_wasPin()             check if last reset was caused by RUN pin low
//
// RESET_assert(mask)       hold peripherals in reset (e.g. RESETS_RESET_UART0)
// RESET_release(mask)      release peripherals from reset and wait until done
// RESET_cycle(mask)        reset and release peripherals (clean state)
//
// Boot ROM (ROM) functions available:
// -----------------------------------
// ROM_func(c1,c2)          get pointer to boot ROM function with code c1,c2
// ROM_VERSION              boot ROM version
//
// Cyclic Redundancy Check (CRC) functions available (DMA sniffer):
// ----------------------------------------------------------------
// CRC_calc32(a,n)          calculate standard CRC-32 (as zlib, PKZIP, Ethernet) of n
//                          bytes at address a (uses DMA channel DMA_CRC_CH)
//
// Direct Memory Access Controller (DMA) functions available:
// ----------------------------------------------------------
// DMA_init()               release DMA from reset (can be called multiple times)
// DMA_CH(c)                registers of DMA channel c (0..15), e.g. DMA_CH(0).READ_ADDR
// DMA_busy(c)              check if DMA channel c is busy
// DMA_abort(c)             abort transfer on DMA channel c
//
// Device Electronic Signature (ESIG) functions available:
// -------------------------------------------------------
// ESIG_CHIPID              chip ID (SYSINFO)
// ESIG_REVISION            chip revision (stepping, see datasheet)
// ESIG_PACKAGE             package (0: QFN80 = RP2350B, 1: QFN60 = RP2350A)
// ESIG_UID64               64-bit unique chip ID (OTP rows CHIPID0..3)
//
// Interrupt (INT) functions available:
// ------------------------------------
// INT_enable()             global interrupt enable
// INT_disable()            global interrupt disable
// INT_ATOMIC_BLOCK { }     execute block without being interrupted
// CPU_ID                   number of the core running the code (0 or 1)
//
// Notes:
// ------
// - The RP2350 executes code from an external QSPI flash (XIP). In contrast to the
//   RP2040, no second stage bootloader is required: the boot ROM searches the first
//   4KB of the flash for an IMAGE_DEF block (included in system.c), sets up the flash
//   interface (XIP) and starts the firmware via the vector table at the beginning
//   of the flash. Only the Arm cores (Cortex-M33, secure mode) are supported.
// - System clock (F_CPU): 12MHz (XOSC without PLL) or 16..150MHz (PLL_SYS), e.g. 150,
//   144, 133, 125, 120, 100, 96, 72, 64, 50, 48, 36, 32 or 24MHz. The frequency must
//   be exactly generated from 12MHz by the PLL (VCO 750..1600MHz, two post dividers
//   1..7); otherwise 150MHz is used. The peripheral clock (clk_peri) and the HSTX clock
//   are the system clock. USB and ADC are always clocked with 48MHz by PLL_USB. The
//   tick generators (1us for TIMER0, TIMER1 and watchdog) are driven by the 12MHz
//   reference clock (XOSC).
// - The FPU (single precision) is enabled at startup for both cores.
// - Peripherals are held in reset after startup (except GPIO, see SYS_GPIO_EN). The
//   libraries release the peripherals they use.
// - MCO pins (set below): GP13 (GPOUT0), GP15 (GPOUT1), GP21 (GPOUT0), GP23 (GPOUT1),
//   GP24 (GPOUT2), GP25 (GPOUT3).
// - The RP2350 has no RTC peripheral. The RTC functions implement a calendar on the
//   64-bit millisecond counter of the always-on (AON) timer in the power manager
//   (POWMAN), which keeps running during resets other than power-on. The calendar
//   covers the years RTC_YEAR_REF..RTC_YEAR_REF+135.
// - Code that must run while the flash is not accessible (e.g. flash programming)
//   must be placed in SRAM with the RAM_FUNC attribute.
// - The functions of this file must only be used by core 0 (except DLY, INT, CPU_ID).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "rp2350.h"

// ===================================================================================
// Enumerate PIN Designators (use these designators to define pins)
// ===================================================================================
enum{
  GP0,  GP1,  GP2,  GP3,  GP4,  GP5,  GP6,  GP7,  GP8,  GP9,  GP10, GP11, GP12, GP13,
  GP14, GP15, GP16, GP17, GP18, GP19, GP20, GP21, GP22, GP23, GP24, GP25, GP26, GP27,
  GP28, GP29
};

// ===================================================================================
// System Options (set "1" to activate)
// ===================================================================================
#define SYS_CLK_INIT      1         // 1: init system clock on startup
#define SYS_TICK_INIT     1         // 1: init and start SYSTICK on startup
#define SYS_GPIO_EN       1         // 1: release GPIO (IO_BANK0, PADS_BANK0) from reset
#define SYS_CLEAR_BSS     1         // 1: clear uninitialized variables
#define SYS_USE_VECTORS   1         // 1: create interrupt vector table
#define SYS_FPU_EN        1         // 1: enable floating point unit (FPU)
#define MCO_PIN           GP21      // clock output pin (GP13, GP15, GP21, GP23, GP24, GP25)
#define MCO_DIV           1         // clock output divider (1..65535)

// ===================================================================================
// Sytem Clock Defines
// ===================================================================================

// Set system clock frequency
#ifndef F_CPU
  #define F_CPU           150000000 // 150Mhz if not otherwise defined
#endif

#if (F_CPU != 12000000) && ((F_CPU < 16000000) || (F_CPU > 150000000))
  #warning Unsupported system clock frequency, using 150 MHz
  #undef  F_CPU
  #define F_CPU           150000000
#endif

#define F_PERI            F_CPU     // peripheral clock (clk_peri = clk_sys)
#define F_HSTX            F_CPU     // HSTX clock (clk_hstx = clk_sys)
#define F_USB             48000000  // USB clock (PLL_USB)
#define F_ADC             48000000  // ADC clock (PLL_USB)
#define F_REF             XOSC_VALUE// reference clock (XOSC, also tick generator source)

// ===================================================================================
// System Clock Functions
// ===================================================================================
void CLK_init(void);          // init system clock (called automatically on system start)
void CLK_reset(void);         // reset system clock to default state (ROSC)

// Crystal oscillator (XOSC, 12MHz)
void XOSC_enable(void);
#define XOSC_disable()      (XOSC->CTRL = (XOSC->CTRL & ~XOSC_CTRL_ENABLE_Msk) | XOSC_CTRL_ENABLE_DISABLE)
#define XOSC_ready()        (XOSC->STATUS & XOSC_STATUS_STABLE)

// Phase locked loops (PLL_SYS, PLL_USB)
uint8_t PLL_init(PLL_TypeDef* pll, uint32_t freq);
#define PLL_stop(pll)       ((pll)->PWR = 0x2D)   // power down PLL (PD, DSMPD, POSTDIVPD, VCOPD)
#define PLL_locked(pll)     ((pll)->CS & PLL_CS_LOCK)

// Clock output (MCO) on MCO_PIN (GPOUT0..3)
void MCO_set(uint32_t src);
#define MCO_setSYS()        MCO_set(CLOCKS_CLK_GPOUT_CTRL_AUXSRC_SYS)
#define MCO_setUSB()        MCO_set(CLOCKS_CLK_GPOUT_CTRL_AUXSRC_USB)
#define MCO_setREF()        MCO_set(CLOCKS_CLK_GPOUT_CTRL_AUXSRC_REF)
void MCO_stop(void);

// Clock sources for clock outputs (GPOUT AUXSRC values)
#define CLOCKS_CLK_GPOUT_CTRL_AUXSRC_SYS  ((uint32_t)8  << CLOCKS_CLK_GPOUT_CTRL_AUXSRC_Pos)
#define CLOCKS_CLK_GPOUT_CTRL_AUXSRC_USB  ((uint32_t)9  << CLOCKS_CLK_GPOUT_CTRL_AUXSRC_Pos)
#define CLOCKS_CLK_GPOUT_CTRL_AUXSRC_REF  ((uint32_t)11 << CLOCKS_CLK_GPOUT_CTRL_AUXSRC_Pos)

// ===================================================================================
// Real-Time Clock (RTC) Functions (calendar on the AON timer of POWMAN)
// ===================================================================================
#define RTC_YEAR_REF        2000    // reference year (RTC_setDate year = 0), AON timer = 0

void RTC_init(void);                // init and start RTC
void RTC_setTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void RTC_setDate(uint8_t year, uint8_t month, uint8_t day);
void RTC_setAlarm(uint8_t hours, uint8_t minutes, uint8_t seconds);
uint32_t RTC_readTime(void);        // must be called before RTC_readDate()
uint64_t RTC_getMillis(void);       // milliseconds since RTC_YEAR_REF-01-01 00:00:00
extern uint32_t RTC_date;           // date of the last RTC_readTime() call

// Time value: weekday (bits 26..24), hours (20..16), minutes (13..8), seconds (5..0)
// Date value: year (bits 23..12), month (11..8), day (4..0) - same layout as on RP2040
#define RTC_readDate()          (RTC_date)
#define RTC_getSeconds(t)       ( (t)        & 0x3F)
#define RTC_getMinutes(t)       (((t) >>  8) & 0x3F)
#define RTC_getHours(t)         (((t) >> 16) & 0x1F)
#define RTC_getWeekday(t)       (((t) >> 24) & 0x07)
#define RTC_getDay(d)           ( (d)        & 0x1F)
#define RTC_getMonth(d)         (((d) >>  8) & 0x0F)
#define RTC_getYear(d)          (((d) >> 12) & 0xFFF)

#define RTC_isInit()            (POWMAN->TIMER & POWMAN_TIMER_RUN)
#define RTC_stopAlarm()         POWMAN_CLR(POWMAN->TIMER, POWMAN_TIMER_ALARM_ENAB)
#define RTC_readAlarmFlag()     (POWMAN->TIMER & POWMAN_TIMER_ALARM)
#define RTC_clearAlarmFlag()    POWMAN_CLR(POWMAN->TIMER, POWMAN_TIMER_ALARM_ENAB | POWMAN_TIMER_ALARM)
#define RTC_enableAlarmInt()    (POWMAN_SET(POWMAN->INTE, POWMAN_INTE_TIMER), NVIC_EnableIRQ(POWMAN_IRQ_TIMER_IRQn))
#define RTC_disableAlarmInt()   POWMAN_CLR(POWMAN->INTE, POWMAN_INTE_TIMER)

// ===================================================================================
// SYSTICK Functions
// ===================================================================================
#define STK_enable()      (SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk)
#define STK_disable()     (SysTick->CTRL = 0)

// ===================================================================================
// Delay (DLY) Functions (using SYSTICK)
// ===================================================================================
#define DLY_US_TIME       (F_CPU / 1000000)             // system ticks per us
#define DLY_MS_TIME       (F_CPU / 1000)                // system ticks per ms
void DLY_ticks(uint32_t n);                             // delay n+1 clock cycles
static inline void DLY_us(uint32_t us) {                // delay n microseconds
  uint32_t t;
  while(us >= 1000) { DLY_ticks(DLY_MS_TIME - 1); us -= 1000; } // no 32-bit overflow
  t = us * DLY_MS_TIME / 1000;
  if(t) DLY_ticks(t - 1);
}
static inline void DLY_ms(uint32_t ms) {                // delay n milliseconds
  while(ms--) DLY_ticks(DLY_MS_TIME - 1);
}

// ===================================================================================
// Watchdog Timer (IWDG/WDT) Functions
// ===================================================================================
void IWDG_start(uint16_t ms);                           // start WDT with time in ms
void IWDG_reload(uint16_t ms);                          // reload WDT with time in ms
extern uint32_t IWDG_loadValue;                         // last reload value
#define IWDG_feed()       (WATCHDOG->LOAD = IWDG_loadValue)
#define IWDG_stop()       REG_CLR(WATCHDOG->CTRL, WATCHDOG_CTRL_ENABLE)

// ===================================================================================
// Sleep Functions
// ===================================================================================
void SLEEP_WFI_now(void);         // put CPU into sleep, wake up by interrupt
void SLEEP_WFE_now(void);         // put CPU into sleep, wake up by event
void SLEEP_ms(uint32_t ms);       // put CPU into sleep for n milliseconds
void DORMANT_now(uint8_t pin, uint8_t type);  // put device into DORMANT, wake up by PIN

// ===================================================================================
// Reset (RST) Functions
// ===================================================================================
void RST_now(void);               // conduct software reset (via watchdog)
#define RST_clearFlags()          // reset flags are updated automatically on every reset
#define RST_wasIWDG()       (WATCHDOG->REASON & WATCHDOG_REASON_TIMER)
#define RST_wasWDT()        RST_wasIWDG()
#define RST_wasSoftware()   (WATCHDOG->REASON & WATCHDOG_REASON_FORCE)
#define RST_wasPower()      ( !WATCHDOG->REASON &&                                     \
                              (POWMAN->CHIP_RESET & (POWMAN_CHIP_RESET_HAD_POR | POWMAN_CHIP_RESET_HAD_BOR)) )
#define RST_wasPin()        ( !WATCHDOG->REASON &&                                     \
                              (POWMAN->CHIP_RESET & POWMAN_CHIP_RESET_HAD_RUN_LOW) )

// Peripheral reset control (mask: RESETS_RESET_xxx, e.g. RESETS_RESET_UART0)
#define RESET_assert(mask)  REG_SET(RESETS->RESET, mask)
static inline void RESET_release(uint32_t mask) {
  REG_CLR(RESETS->RESET, mask);
  while(~RESETS->RESET_DONE & mask);
}
static inline void RESET_cycle(uint32_t mask) {
  RESET_assert(mask);
  RESET_release(mask);
}

// ===================================================================================
// Bootloader (BOOT) and Boot ROM (ROM) Functions
// ===================================================================================
void BOOT_now(void);              // reset and enter USB bootloader (BOOTSEL mode)
void* ROM_func(char c1, char c2); // get pointer to boot ROM function with code c1,c2
#define ROM_VERSION         (*(volatile uint8_t*)0x00000013)

// ===================================================================================
// Cyclic Redundancy Check (CRC) Functions (using the DMA sniffer)
// ===================================================================================
#define DMA_CRC_CH          15      // DMA channel used for CRC calculation
uint32_t CRC_calc32(const void* addr, uint32_t len);

// ===================================================================================
// Direct Memory Access Controller (DMA) Functions
// ===================================================================================
#define DMA_init()          RESET_release(RESETS_RESET_DMA)
#define DMA_CH(c)           (DMA->CH[c])
#define DMA_busy(c)         (DMA->CH[c].AL1_CTRL & DMA_CH_CTRL_TRIG_BUSY)
static inline void DMA_abort(uint8_t ch) {
  DMA->CHAN_ABORT = (uint32_t)1 << ch;
  while(DMA->CHAN_ABORT & ((uint32_t)1 << ch));
}

// ===================================================================================
// Device Electronic Signature (ESIG)
// ===================================================================================
#define ESIG_CHIPID         (SYSINFO->CHIP_ID)
#define ESIG_REVISION       ((SYSINFO->CHIP_ID & SYSINFO_CHIP_ID_REVISION_Msk) >> SYSINFO_CHIP_ID_REVISION_Pos)
#define ESIG_PACKAGE        (SYSINFO->PACKAGE_SEL & 1)
#define ESIG_UID64          ( ((uint64_t)OTP_ROW(3) << 48) | ((uint64_t)OTP_ROW(2) << 32) \
                            | ((uint32_t)OTP_ROW(1) << 16) | OTP_ROW(0) )

// ===================================================================================
// Code and Data Placement
// ===================================================================================
#define RAM_FUNC            __attribute__((section(".RamFunc"), noinline, long_call))
#define CPU_ID              (SIO->CPUID)  // number of the core running the code (0 or 1)

// ===================================================================================
// Interrupt (INT) Functions
// ===================================================================================
#define INT_enable()          __enable_irq()
#define INT_disable()         __disable_irq()
#define INT_ATOMIC_BLOCK      for(INT_ATOMIC_RESTORE, __ToDo = 1; __ToDo; __ToDo = 0)
#define INT_ATOMIC_RESTORE    uint32_t __reg_save __attribute__((__cleanup__(__iRestore))) = __iSave()

// Save interrupt status and disable interrupts
static inline uint32_t __iSave(void) {
  uint32_t result;
  __ASM volatile("MRS %0, primask" : "=r" (result));
  __ASM volatile("cpsid i" ::: "memory");
  return result;
}

// Restore interrupt status
static inline void __iRestore(const uint32_t *__s) {
  __ASM volatile("MSR primask, %0" :: "r" (*__s) : "memory");
}

// ===================================================================================
// Imported System Functions from cmsis_gcc.h and core_cm33.h
// ===================================================================================
#define NVIC_USER_IRQ_OFFSET       16
#define __COMPILER_BARRIER()       __ASM volatile("":::"memory")

// The following EXC_RETURN values are saved the LR on exception entry
#define EXC_RETURN_HANDLER         (0xFFFFFFF1UL)     // return to Handler mode, uses MSP after return
#define EXC_RETURN_THREAD_MSP      (0xFFFFFFF9UL)     // return to Thread mode, uses MSP after return
#define EXC_RETURN_THREAD_PSP      (0xFFFFFFFDUL)     // return to Thread mode, uses PSP after return
#define FNC_RETURN                 (0xFEFFFFFFUL)     // bit [0] ignored when processing a branch

// Ignore some GCC warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#if defined (__thumb__) && !defined (__thumb2__)
#define __CMSIS_GCC_OUT_REG(r) "=l" (r)
#define __CMSIS_GCC_USE_REG(r) "l" (r)
#else
#define __CMSIS_GCC_OUT_REG(r) "=r" (r)
#define __CMSIS_GCC_USE_REG(r) "r" (r)
#endif

// Enable Interrupts
__attribute__( ( always_inline ) ) __STATIC_INLINE void __enable_irq(void) {
  __ASM volatile ("cpsie i" : : : "memory");
}

// Disable Interrupts
__attribute__( ( always_inline ) ) __STATIC_INLINE void __disable_irq(void) {
  __ASM volatile ("cpsid i" : : : "memory");
}

// Get Control Register
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_CONTROL(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, control" : "=r" (result) );
  return(result);
}

// Set Control Register
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_CONTROL(uint32_t control) {
  __ASM volatile ("MSR control, %0" : : "r" (control) : "memory");
}

// Get IPSR Register
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_IPSR(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, ipsr" : "=r" (result) );
  return(result);
}

// Get APSR Register
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_APSR(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, apsr" : "=r" (result) );
  return(result);
}

// Get xPSR Register
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_xPSR(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, xpsr" : "=r" (result) );
  return(result);
}

// Get Process Stack Pointer
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_PSP(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, psp\n"  : "=r" (result) );
  return(result);
}

// Set Process Stack Pointer
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_PSP(uint32_t topOfProcStack) {
  __ASM volatile ("MSR psp, %0\n" : : "r" (topOfProcStack) : );
}

// Get Main Stack Pointer
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_MSP(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, msp\n" : "=r" (result) );
  return(result);
}

// Set Main Stack Pointer
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_MSP(uint32_t topOfMainStack) {
  __ASM volatile ("MSR msp, %0\n" : : "r" (topOfMainStack) : );
}

// Get Priority Mask
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_PRIMASK(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, primask" : "=r" (result) );
  return(result);
}

// Set Priority Mask
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_PRIMASK(uint32_t priMask) {
  __ASM volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}

// Get Base Priority
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_BASEPRI(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, basepri" : "=r" (result) );
  return(result);
}

// Set Base Priority
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_BASEPRI(uint32_t basePri) {
  __ASM volatile ("MSR basepri, %0" : : "r" (basePri) : "memory");
}

// Get Fault Mask
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_FAULTMASK(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, faultmask" : "=r" (result) );
  return(result);
}

// Set Fault Mask
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_FAULTMASK(uint32_t faultMask) {
  __ASM volatile ("MSR faultmask, %0" : : "r" (faultMask) : "memory");
}

// Enable FIQ (clears FAULTMASK)
__attribute__( ( always_inline ) ) __STATIC_INLINE void __enable_fault_irq(void) {
  __ASM volatile ("cpsie f" : : : "memory");
}

// Disable FIQ (sets FAULTMASK)
__attribute__( ( always_inline ) ) __STATIC_INLINE void __disable_fault_irq(void) {
  __ASM volatile ("cpsid f" : : : "memory");
}

// Get Main Stack Pointer Limit
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_MSPLIM(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, msplim" : "=r" (result) );
  return(result);
}

// Set Main Stack Pointer Limit
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_MSPLIM(uint32_t MainStackPtrLimit) {
  __ASM volatile ("MSR msplim, %0" : : "r" (MainStackPtrLimit));
}

// Get FPSCR (Floating Point Status/Control Register)
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_FPSCR(void) {
#if (defined(__ARM_FP) && (__ARM_FP >= 1))
  return __builtin_arm_get_fpscr();
#else
  return(0U);
#endif
}

// Set FPSCR (Floating Point Status/Control Register)
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_FPSCR(uint32_t fpscr) {
#if (defined(__ARM_FP) && (__ARM_FP >= 1))
  __builtin_arm_set_fpscr(fpscr);
#else
  (void)fpscr;
#endif
}

// No Operation
__attribute__((always_inline)) __STATIC_INLINE void __NOP(void) {
  __ASM volatile ("nop");
}

// Wait For Interrupt
__attribute__((always_inline)) __STATIC_INLINE void __WFI(void) {
  __ASM volatile ("wfi");
}

// Wait For Event
__attribute__((always_inline)) __STATIC_INLINE void __WFE(void) {
  __ASM volatile ("wfe");
}

// Send Event
__attribute__((always_inline)) __STATIC_INLINE void __SEV(void) {
  __ASM volatile ("sev");
}

// Instruction Synchronization Barrier
__attribute__((always_inline)) __STATIC_INLINE void __ISB(void) {
  __ASM volatile ("isb 0xF":::"memory");
}

// Data Synchronization Barrier
__attribute__((always_inline)) __STATIC_INLINE void __DSB(void) {
  __ASM volatile ("dsb 0xF":::"memory");
}

// Data Memory Barrier
__attribute__((always_inline)) __STATIC_INLINE void __DMB(void) {
  __ASM volatile ("dmb 0xF":::"memory");
}

// Reverse byte order (32 bit)
__attribute__((always_inline)) __STATIC_INLINE uint32_t __REV(uint32_t value) {
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
  return __builtin_bswap32(value);
#else
  uint32_t result;
  __ASM volatile ("rev %0, %1" : __CMSIS_GCC_OUT_REG (result) : __CMSIS_GCC_USE_REG (value) );
  return(result);
#endif
}

// Reverse byte order (16 bit)
__attribute__((always_inline)) __STATIC_INLINE uint32_t __REV16(uint32_t value) {
  uint32_t result;
  __ASM volatile ("rev16 %0, %1" : __CMSIS_GCC_OUT_REG (result) : __CMSIS_GCC_USE_REG (value) );
  return(result);
}

// Reverse byte order in signed short value
__attribute__((always_inline)) __STATIC_INLINE int32_t __REVSH(int32_t value) {
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
  return (short)__builtin_bswap16(value);
#else
  int32_t result;
  __ASM volatile ("revsh %0, %1" : __CMSIS_GCC_OUT_REG (result) : __CMSIS_GCC_USE_REG (value) );
  return(result);
#endif
}

// Rotate Right in unsigned value (32 bit)
__attribute__((always_inline)) __STATIC_INLINE uint32_t __ROR(uint32_t op1, uint32_t op2) {
  return (op1 >> op2) | (op1 << (32U - op2));
}

// Breakpoint
#define __BKPT(value)     __ASM volatile ("bkpt "#value)

// Reverse bit order of value
__attribute__((always_inline)) __STATIC_INLINE uint32_t __RBIT(uint32_t value) {
  uint32_t result;
  __ASM ("rbit %0, %1" : "=r" (result) : "r" (value) );
  return(result);
}

// Exclusive load (8/16/32 bit)
__attribute__((always_inline)) __STATIC_INLINE uint8_t __LDREXB(volatile uint8_t *addr) {
  uint32_t result;
  __ASM volatile ("ldrexb %0, %1" : "=r" (result) : "Q" (*addr) );
  return ((uint8_t) result);
}
__attribute__((always_inline)) __STATIC_INLINE uint16_t __LDREXH(volatile uint16_t *addr) {
  uint32_t result;
  __ASM volatile ("ldrexh %0, %1" : "=r" (result) : "Q" (*addr) );
  return ((uint16_t) result);
}
__attribute__((always_inline)) __STATIC_INLINE uint32_t __LDREXW(volatile uint32_t *addr) {
  uint32_t result;
  __ASM volatile ("ldrex %0, %1" : "=r" (result) : "Q" (*addr) );
  return(result);
}

// Exclusive store (8/16/32 bit), returns 0 on success
__attribute__((always_inline)) __STATIC_INLINE uint32_t __STREXB(uint8_t value, volatile uint8_t *addr) {
  uint32_t result;
  __ASM volatile ("strexb %0, %2, %1" : "=&r" (result), "=Q" (*addr) : "r" ((uint32_t)value) );
  return(result);
}
__attribute__((always_inline)) __STATIC_INLINE uint32_t __STREXH(uint16_t value, volatile uint16_t *addr) {
  uint32_t result;
  __ASM volatile ("strexh %0, %2, %1" : "=&r" (result), "=Q" (*addr) : "r" ((uint32_t)value) );
  return(result);
}
__attribute__((always_inline)) __STATIC_INLINE uint32_t __STREXW(uint32_t value, volatile uint32_t *addr) {
  uint32_t result;
  __ASM volatile ("strex %0, %2, %1" : "=&r" (result), "=Q" (*addr) : "r" (value) );
  return(result);
}

// Remove the exclusive lock
__attribute__((always_inline)) __STATIC_INLINE void __CLREX(void) {
  __ASM volatile ("clrex" ::: "memory");
}

// Signed and unsigned saturation
#define __SSAT(ARG1, ARG2) \
__extension__ ({ int32_t __RES, __ARG1 = (ARG1); \
  __ASM volatile ("ssat %0, %1, %2" : "=r" (__RES) :  "I" (ARG2), "r" (__ARG1) : "cc" ); __RES; })
#define __USAT(ARG1, ARG2) \
__extension__ ({ uint32_t __RES, __ARG1 = (ARG1); \
  __ASM volatile ("usat %0, %1, %2" : "=r" (__RES) :  "I" (ARG2), "r" (__ARG1) : "cc" ); __RES; })

// Count leading zeros
#define __CLZ             (uint8_t)__builtin_clz

#pragma GCC diagnostic pop

// Set Priority Grouping
__STATIC_INLINE void NVIC_SetPriorityGrouping(uint32_t PriorityGroup) {
  uint32_t reg_value;
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);
  reg_value  =  SCB->AIRCR;
  reg_value &= ~((uint32_t)(SCB_AIRCR_VECTKEY_Msk | SCB_AIRCR_PRIGROUP_Msk));
  reg_value  =  (reg_value | ((uint32_t)0x5FAUL << SCB_AIRCR_VECTKEY_Pos) | (PriorityGroupTmp << SCB_AIRCR_PRIGROUP_Pos));
  SCB->AIRCR =  reg_value;
}

// Get Priority Grouping
__STATIC_INLINE uint32_t NVIC_GetPriorityGrouping(void) {
  return ((uint32_t)((SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) >> SCB_AIRCR_PRIGROUP_Pos));
}

// Enable External Interrupt
__STATIC_INLINE void NVIC_EnableIRQ(IRQn_Type IRQn) {
  if ((int32_t)(IRQn) >= 0) {
    __COMPILER_BARRIER();
    NVIC->ISER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    __COMPILER_BARRIER();
  }
}

// Get Interrupt Enable status
__STATIC_INLINE uint32_t NVIC_GetEnableIRQ(IRQn_Type IRQn) {
  if ((int32_t)(IRQn) >= 0) {
    return((uint32_t)(((NVIC->ISER[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  return(0U);
}

// Disable External Interrupt
__STATIC_INLINE void NVIC_DisableIRQ(IRQn_Type IRQn) {
  if ((int32_t)(IRQn) >= 0) {
    NVIC->ICER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    __DSB();
    __ISB();
  }
}

// Get Pending Interrupt
__STATIC_INLINE uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn) {
  if ((int32_t)(IRQn) >= 0) {
    return((uint32_t)(((NVIC->ISPR[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  return(0U);
}

// Set Pending Interrupt
__STATIC_INLINE void NVIC_SetPendingIRQ(IRQn_Type IRQn) {
  if ((int32_t)(IRQn) >= 0) {
    NVIC->ISPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}

// Clear Pending Interrupt
__STATIC_INLINE void NVIC_ClearPendingIRQ(IRQn_Type IRQn) {
  if ((int32_t)(IRQn) >= 0) {
    NVIC->ICPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}

// Get Active Interrupt
__STATIC_INLINE uint32_t NVIC_GetActive(IRQn_Type IRQn) {
  if ((int32_t)(IRQn) >= 0) {
    return((uint32_t)(((NVIC->IABR[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  return(0U);
}

// Set Interrupt Priority
__STATIC_INLINE void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority) {
  if ((int32_t)(IRQn) >= 0) {
    NVIC->IPR[((uint32_t)IRQn)]               = (uint8_t)((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL);
  }
  else {
    SCB->SHPR[(((uint32_t)IRQn) & 0xFUL)-4UL] = (uint8_t)((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL);
  }
}

// Get Interrupt Priority
__STATIC_INLINE uint32_t NVIC_GetPriority(IRQn_Type IRQn) {
  if ((int32_t)(IRQn) >= 0) {
    return(((uint32_t)NVIC->IPR[((uint32_t)IRQn)]               >> (8U - __NVIC_PRIO_BITS)));
  }
  else {
    return(((uint32_t)SCB->SHPR[(((uint32_t)IRQn) & 0xFUL)-4UL] >> (8U - __NVIC_PRIO_BITS)));
  }
}

// System Reset
__STATIC_INLINE void NVIC_SystemReset(void) {
  __DSB();  // Ensure all outstanding memory accesses included buffered write are completed before reset
  SCB->AIRCR  = (uint32_t)((0x5FAUL << SCB_AIRCR_VECTKEY_Pos) |
                           (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
                            SCB_AIRCR_SYSRESETREQ_Msk);
  __DSB();                                                          // Ensure completion of memory access
  for(;;) __NOP();                                                  // wait until reset
}

// System Tick Configuration
__STATIC_INLINE uint32_t SysTick_Config(uint32_t ticks) {
  if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk) return (1UL);        // Reload value impossible
  SysTick->LOAD  = (uint32_t)(ticks - 1UL);                         // set reload register
  NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); // set Priority for Systick Interrupt
  SysTick->VAL   = 0UL;                                             // Load the SysTick Counter Value
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk;                         // Enable SysTick IRQ and SysTick Timer
  return (0UL);                                                     // Function successful
}

#ifdef __cplusplus
};
#endif
