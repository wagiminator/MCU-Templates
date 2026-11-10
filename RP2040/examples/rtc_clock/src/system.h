// ===================================================================================
// Basic System Functions for RP2040                                          * v1.0 *
// ===================================================================================
//
// This file must be included!!! The system configuration and the system clock are
// set up automatically on system start.
//
// System clock functions available:
// ---------------------------------
// CLK_init()               init system clock according to F_CPU (see below)
// CLK_reset()              reset system clock to default state (ROSC, approx. 6.5MHz)
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
// Real-Time Clock (RTC) functions available:
// ------------------------------------------
// RTC_init()               init and start RTC (clock: 46875Hz from PLL_USB)
// RTC_setTime(h,m,s)       set time (hours, minutes, seconds), decimal values
// RTC_setDate(y,m,d)       set date (year since RTC_YEAR_REF, month, day), decimal values
// RTC_readTime()           read time register (must be post-processed, see below)
// RTC_readDate()           read date register (must be post-processed, see below)
// RTC_getSeconds(t)        get seconds of time register value t (0..59)
// RTC_getMinutes(t)        get minutes of time register value t (0..59)
// RTC_getHours(t)          get hours   of time register value t (0..23)
// RTC_getWeekday(t)        get weekday of time register value t (0: Sunday .. 6)
// RTC_getDay(d)            get day     of date register value d (1..31)
// RTC_getMonth(d)          get month   of date register value d (1..12)
// RTC_getYear(d)           get year    of date register value d (e.g. 2026)
// RTC_isInit()             check if RTC is running
//
// RTC_setAlarm(h,m,s)      set daily alarm (hours, minutes, seconds)
// RTC_stopAlarm()          stop alarm
// RTC_readAlarmFlag()      check alarm flag
// RTC_clearAlarmFlag()     clear alarm flag (also stops the alarm, restart with RTC_setAlarm)
// RTC_enableAlarmInt()     enable alarm interrupt (RTC_IRQ_IRQHandler)
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
// IWDG_start(n)            start watchdog timer, n milliseconds, n<=8300
// IWDG_reload(n)           reload watchdog counter with n milliseconds, n<=8300
// IWDG_feed()              feed the dog (reload last time)
// IWDG_stop()              stop watchdog timer
//
// Sleep functions available:
// --------------------------
// SLEEP_WFI_now()          put CPU into sleep, wake up by interrupt
// SLEEP_WFE_now()          put CPU into sleep, wake up by event
// SLEEP_ms(n)              put CPU into sleep for n milliseconds (TIMER alarm 3)
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
// ROM_VERSION              boot ROM version (1: B0, 2: B1, 3: B2)
//
// Cyclic Redundancy Check (CRC) functions available (DMA sniffer):
// ----------------------------------------------------------------
// CRC_calc32(a,n)          calculate standard CRC-32 (as zlib, PKZIP, Ethernet) of n
//                          bytes at address a (uses DMA channel DMA_CRC_CH)
//
// Direct Memory Access Controller (DMA) functions available:
// ----------------------------------------------------------
// DMA_init()               release DMA from reset (can be called multiple times)
// DMA_CH(c)                registers of DMA channel c (0..11), e.g. DMA_CH(0).READ_ADDR
// DMA_busy(c)              check if DMA channel c is busy
// DMA_abort(c)             abort transfer on DMA channel c
//
// Device Electronic Signature (ESIG) functions available:
// -------------------------------------------------------
// ESIG_CHIPID              chip ID (SYSINFO)
// ESIG_REVISION            chip revision (1: B0, 2: B1/B2)
// (the unique ID is stored in the external flash, see flash library)
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
// - The RP2040 executes code from an external QSPI flash (XIP). The first 256 bytes
//   of the flash contain the second stage bootloader (boot2), which configures the
//   flash interface. Select the boot2 matching the flash chip below (SYS_BOOT2).
// - System clock (F_CPU): 12MHz (XOSC without PLL) or 16..133MHz (PLL_SYS), e.g. 125,
//   133, 120, 100, 96, 72, 64, 50, 48, 36, 32, 24 or 16MHz. The frequency must be
//   exactly generated from 12MHz by the PLL (VCO 750..1596MHz, two post dividers
//   1..7); otherwise 125MHz is used. The peripheral clock (clk_peri) is the system
//   clock. USB and ADC are always clocked with 48MHz by PLL_USB, the RTC with
//   46875Hz (48MHz / 1024). The watchdog tick generator (1us for TIMER and WDT) is
//   driven by the 12MHz reference clock.
// - Peripherals are held in reset after startup (except GPIO, see SYS_GPIO_EN). The
//   libraries release the peripherals they use.
// - MCO pins (set below): GP21 (GPOUT0), GP23 (GPOUT1), GP24 (GPOUT2), GP25 (GPOUT3).
// - The watchdog counter of the RP2040 decrements twice per tick (erratum RP2040-E1),
//   which is taken into account here. Maximum period: 8.3s.
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
#include "rp2040.h"

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
#define SYS_BOOT2         0         // boot2 for flash: 0: W25Q080 (Raspberry Pi Pico, W25Qxx,
                                    // fast quad SPI), 1: generic (any flash, slow 03h command)
#define MCO_PIN           GP21      // clock output pin (GP21, GP23, GP24, GP25)
#define MCO_DIV           1         // clock output divider (1..16777215)

// ===================================================================================
// Sytem Clock Defines
// ===================================================================================

// Set system clock frequency
#ifndef F_CPU
  #define F_CPU           125000000 // 125Mhz if not otherwise defined
#endif

#if (F_CPU != 12000000) && ((F_CPU < 16000000) || (F_CPU > 133000000))
  #warning Unsupported system clock frequency, using 125 MHz
  #undef  F_CPU
  #define F_CPU           125000000
#endif

#define F_PERI            F_CPU     // peripheral clock (clk_peri = clk_sys)
#define F_USB             48000000  // USB clock (PLL_USB)
#define F_ADC             48000000  // ADC clock (PLL_USB)
#define F_RTC             46875     // RTC clock (PLL_USB / 1024)
#define F_REF             XOSC_VALUE// reference clock (XOSC, also watchdog tick source)

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
#define CLOCKS_CLK_GPOUT_CTRL_AUXSRC_SYS  ((uint32_t)6 << CLOCKS_CLK_GPOUT_CTRL_AUXSRC_Pos)
#define CLOCKS_CLK_GPOUT_CTRL_AUXSRC_USB  ((uint32_t)7 << CLOCKS_CLK_GPOUT_CTRL_AUXSRC_Pos)
#define CLOCKS_CLK_GPOUT_CTRL_AUXSRC_REF  ((uint32_t)10 << CLOCKS_CLK_GPOUT_CTRL_AUXSRC_Pos)

// ===================================================================================
// Real-Time Clock (RTC) Functions
// ===================================================================================
#define RTC_YEAR_REF        2000    // reference year (RTC_setDate year = 0)

void RTC_init(void);                // init and start RTC
void RTC_setTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void RTC_setDate(uint8_t year, uint8_t month, uint8_t day);
void RTC_setAlarm(uint8_t hours, uint8_t minutes, uint8_t seconds);

#define RTC_readTime()          (RTC->RTC_0)  // must be read before RTC_readDate()
#define RTC_readDate()          (RTC->RTC_1)
#define RTC_getSeconds(t)       (((t) & RTC_RTC_0_SEC_Msk)   >> RTC_RTC_0_SEC_Pos)
#define RTC_getMinutes(t)       (((t) & RTC_RTC_0_MIN_Msk)   >> RTC_RTC_0_MIN_Pos)
#define RTC_getHours(t)         (((t) & RTC_RTC_0_HOUR_Msk)  >> RTC_RTC_0_HOUR_Pos)
#define RTC_getWeekday(t)       (((t) & RTC_RTC_0_DOTW_Msk)  >> RTC_RTC_0_DOTW_Pos)
#define RTC_getDay(d)           (((d) & RTC_RTC_1_DAY_Msk)   >> RTC_RTC_1_DAY_Pos)
#define RTC_getMonth(d)         (((d) & RTC_RTC_1_MONTH_Msk) >> RTC_RTC_1_MONTH_Pos)
#define RTC_getYear(d)          (((d) & RTC_RTC_1_YEAR_Msk)  >> RTC_RTC_1_YEAR_Pos)

#define RTC_isInit()            (RTC->CTRL & RTC_CTRL_RTC_ACTIVE)
#define RTC_stopAlarm()         REG_CLR(RTC->IRQ_SETUP_0, RTC_IRQ_SETUP_0_MATCH_ENA)
#define RTC_readAlarmFlag()     (RTC->INTR & RTC_INTR_RTC)
#define RTC_clearAlarmFlag()    RTC_stopAlarm()
#define RTC_enableAlarmInt()    (RTC->INTE = RTC_INTE_RTC, NVIC_EnableIRQ(RTC_IRQ_IRQn))
#define RTC_disableAlarmInt()   (RTC->INTE = 0)

// ===================================================================================
// SYSTICK Functions
// ===================================================================================
#define STK_enable()      (SysTick->CTRL = SysTick_CTRL_ENABLE | SysTick_CTRL_CLKSOURCE)
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
                              (VREG_AND_CHIP_RESET->CHIP_RESET & VREG_AND_CHIP_RESET_CHIP_RESET_HAD_POR) )
#define RST_wasPin()        ( !WATCHDOG->REASON &&                                     \
                              (VREG_AND_CHIP_RESET->CHIP_RESET & VREG_AND_CHIP_RESET_CHIP_RESET_HAD_RUN) )

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
#define DMA_CRC_CH          11      // DMA channel used for CRC calculation
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
// Imported System Functions from cmsis_gcc.h and core_cm0plus.h
// ===================================================================================
#define NVIC_USER_IRQ_OFFSET       16

// The following EXC_RETURN values are saved the LR on exception entry
#define EXC_RETURN_HANDLER         (0xFFFFFFF1UL)     // return to Handler mode, uses MSP after return
#define EXC_RETURN_THREAD_MSP      (0xFFFFFFF9UL)     // return to Thread mode, uses MSP after return
#define EXC_RETURN_THREAD_PSP      (0xFFFFFFFDUL)     // return to Thread mode, uses PSP after return

// Interrupt Priorities are WORD accessible only under Armv6-M
// The following MACROS handle generation of the register offset and byte masks
#define _BIT_SHIFT(IRQn)         (  ((((uint32_t)(int32_t)(IRQn))         )      &  0x03UL) * 8UL)
#define _SHP_IDX(IRQn)           ( (((((uint32_t)(int32_t)(IRQn)) & 0x0FUL)-8UL) >>    2UL)      )
#define _IP_IDX(IRQn)            (   (((uint32_t)(int32_t)(IRQn))                >>    2UL)      )

#define NVIC_SetPriorityGrouping(X) (void)(X)
#define NVIC_GetPriorityGrouping()  (0U)

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
  int32_t s = 4 /*sizeof(v)*/ * 8 - 1; // extra shift needed at end
  result = value;                      // r will be reversed bits of v; first get LSB of v
  for (value >>= 1U; value; value >>= 1U) {
    result <<= 1U;
    result |= value & 1U;
    s--;
  }
  result <<= s;                        // shift when v's highest bits are zero
  return(result);
}

// Count leading zeros
#define __CLZ             __builtin_clz

#pragma GCC diagnostic pop

// Enable External Interrupt
__STATIC_INLINE void NVIC_EnableIRQ(IRQn_Type IRQn) {
  NVIC->ISER[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

// Disable External Interrupt
__STATIC_INLINE void NVIC_DisableIRQ(IRQn_Type IRQn) {
  NVIC->ICER[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

// Get Pending Interrupt
__STATIC_INLINE uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn) {
  return((uint32_t)(((NVIC->ISPR[0U] & (1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
}

// Set Pending Interrupt
__STATIC_INLINE void NVIC_SetPendingIRQ(IRQn_Type IRQn) {
  NVIC->ISPR[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

// Clear Pending Interrupt
__STATIC_INLINE void NVIC_ClearPendingIRQ(IRQn_Type IRQn) {
  NVIC->ICPR[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

// Set Interrupt Priority
__STATIC_INLINE void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority) {
  if ((int32_t)(IRQn) < 0) {
    SCB->SHP[_SHP_IDX(IRQn)] = ((uint32_t)(SCB->SHP[_SHP_IDX(IRQn)] & ~(0xFFUL << _BIT_SHIFT(IRQn))) |
       (((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(IRQn)));
  }
  else {
    NVIC->IP[_IP_IDX(IRQn)]  = ((uint32_t)(NVIC->IP[_IP_IDX(IRQn)]  & ~(0xFFUL << _BIT_SHIFT(IRQn))) |
       (((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(IRQn)));
  }
}

// Get Interrupt Priority
__STATIC_INLINE uint32_t NVIC_GetPriority(IRQn_Type IRQn) {
  if ((int32_t)(IRQn) < 0) {
    return((uint32_t)(((SCB->SHP[_SHP_IDX(IRQn)] >> _BIT_SHIFT(IRQn) ) & (uint32_t)0xFFUL) >> (8U - __NVIC_PRIO_BITS)));
  }
  else {
    return((uint32_t)(((NVIC->IP[ _IP_IDX(IRQn)] >> _BIT_SHIFT(IRQn) ) & (uint32_t)0xFFUL) >> (8U - __NVIC_PRIO_BITS)));
  }
}

// System Reset
__STATIC_INLINE void NVIC_SystemReset(void) {
  __DSB();  // Ensure all outstanding memory accesses included buffered write are completed before reset
  SCB->AIRCR  = ((0x5FAUL << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk);
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
