// ===================================================================================
// Basic System Functions for CH573, CH571                                    * v1.0 *
// ===================================================================================
//
// This file must be included!!! The system configuration and the system clock are
// set up automatically on system start.
//
// System clock functions available:
// ---------------------------------
// CLK_init()               init system clock according to F_CPU
// CLK_reset()              reset system clock to default state (HSE/5 = 6.4 MHz)
// CLK_getFreq()            get current system clock frequency in Hz
//
// HSE_enable()             enable external 32 MHz crystal oscillator (HSE)
// HSE_disable()            disable HSE (only if the system clock is not using it!)
// PLL_enable()             enable PLL (480 MHz, from HSE)
// PLL_disable()            disable PLL (only if the system clock is not using it!)
// DCDC_enable()            enable internal DC/DC converter (needs inductor on VSW)
// DCDC_disable()           disable DC/DC converter (*)
//
// Low-speed 32 kHz clock (CLK32K, used by RTC, sleep and Bluetooth) functions:
// ----------------------------------------------------------------------------
// LSI_enable()             enable internal 32 kHz RC oscillator (LSI) (*)
// LSI_disable()            disable LSI
// LSI_calibrate()          calibrate LSI to 32768 Hz (takes some milliseconds)
// LSE_enable()             enable external 32768 Hz crystal (LSE) on PA10/PA11
// LSE_disable()            disable LSE (*)
// CLK32K_selectLSI()       select LSI as 32 kHz clock source (*)
// CLK32K_selectLSE()       select LSE as 32 kHz clock source
// CLK32K_init()            init 32 kHz clock according to SYS_CLK32K setting
//
// Delay (DLY) functions available:
// --------------------------------
// DLY_ticks(n)             delay n system clock cycles
// DLY_us(n)                delay n microseconds
// DLY_ms(n)                delay n milliseconds
//
// Reset (RST) functions available:
// --------------------------------
// RST_now()                conduct software reset
// RST_getFlags()           get reason for last reset (RST_FLAG_SW, RST_FLAG_RPOR, ..)
// RST_wasSoftware()        check if last reset was caused by software
// RST_wasPower()           check if last reset was caused by power on
// RST_wasWDT()             check if last reset was caused by watchdog timeout
// RST_wasPin()             check if last reset was caused by RST pin low
// RST_wasWakeup()          check if last reset was caused by wake-up from shutdown
//
// Watchdog Timer (WDT) functions available:
// -----------------------------------------
// WDT_start(n)             start watchdog timer with n milliseconds, n<=WDT_MAX_MS
// WDT_reload(n)            reload watchdog counter with n milliseconds
// WDT_feed()               feed the dog (reload last time)
// WDT_stop()               stop watchdog reset
//
// Real-Time Clock (RTC) functions available (32 kHz clock):
// ---------------------------------------------------------
// RTC_init()               init 32 kHz clock (SYS_CLK32K) and start RTC
// RTC_setTime(h,m,s)       set time (hours, minutes, seconds)
// RTC_setDay(d)            set day counter (0..16383)
// RTC_getTime()            get time of day as seconds (0..86399)
// RTC_getDay()             get day counter
// RTC_getHours(t)          get hours from time t
// RTC_getMinutes(t)        get minutes from time t
// RTC_getSeconds(t)        get seconds from time t
// RTC_getTicks()           get 32768 Hz ticks since midnight (0..0xA8BFFFFF)
//
// RTC_TRIG_set(n)          set trigger (alarm) n ticks (1/32768 s) from now
// RTC_TRIG_stop()          stop trigger
// RTC_TRIG_flag()          check if trigger was reached
// RTC_TRIG_clear()         clear trigger flag
// RTC_TMR_start(p)         start periodic timer, p: RTC_TMR_125MS, RTC_TMR_250MS,
//                          RTC_TMR_500MS, RTC_TMR_1S, RTC_TMR_2S, .., RTC_TMR_16S
// RTC_TMR_stop()           stop periodic timer
// RTC_TMR_flag()           check if timer period has elapsed
// RTC_TMR_clear()          clear timer flag
// RTC_INT_enable()         enable RTC interrupt (trigger and timer)
// RTC_INT_disable()        disable RTC interrupt
// RTC_ISR { }              RTC interrupt service routine
//
// Sleep functions available:
// --------------------------
// IDLE_now()               stop CPU, wake up by any interrupt
// HALT_now()               stop CPU and clocks (except 32 kHz), RAM and peripherals
//                          keep state, wake up by RTC/GPIO/USB event (see WAKE_..)
// SLEEP_now()              power down peripherals, RAM keeps content, wake up by
//                          RTC/GPIO event, continues after SLEEP_now()
// SHUTDOWN_now()           power down everything, wake up causes a reset
// SLEEP_ms(n)              put device into SLEEP for n milliseconds (uses RTC)
// HALT_ms(n)               put device into HALT for n milliseconds (uses RTC)
//
// WAKE_RTC_enable()        enable wake-up by RTC trigger/timer
// WAKE_GPIO_enable()       enable wake-up by GPIO interrupt (see gpio.h)
// WAKE_USB_enable()        enable wake-up by USB
// WAKE_disable()           disable all wake-up sources
//
// Battery Voltage Detector (BAT) functions available:
// ---------------------------------------------------
// BAT_enable(n)            enable detector, n: BAT_LEVEL_1V95, BAT_LEVEL_2V15,
//                          BAT_LEVEL_2V35, BAT_LEVEL_2V55 (detection level)
// BAT_disable()            disable detector (*)
// BAT_isLow()              check if supply voltage is below detection level
//
// Interrupt (INT) functions available:
// ------------------------------------
// INT_enable()             global interrupt enable
// INT_disable()            global interrupt disable
// INT_ATOMIC_BLOCK { }     execute block without being interrupted
//
// Device Electronic Signature (ESIG) functions available:
// -------------------------------------------------------
// ESIG_CHIPID              chip ID (0x73 for CH573, 0x71 for CH571)
// ESIG_getUID(buf)         read 8-byte unique ID into buffer
// ESIG_getMAC(buf)         read 6-byte factory MAC address into buffer (LSB first)
//
// Notes:
// ------
// - The CH573 has no internal high-speed oscillator, a 32 MHz crystal is required.
//   The system clock is derived from it, either directly (divided) or via the
//   480 MHz PLL (divided). USB and Bluetooth need the PLL/HSE.
// - There is no function to jump into the bootloader from the firmware. The USB
//   bootloader is started, if PB22 (BOOT key) is held low while the board is
//   connected to USB (power-on).
// - Protected registers (marked "SAM" in the datasheet) are accessed with
//   SAFE_ACCESS { }.
// - (*) default state
//
// References:
// -----------
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
// - CH573 datasheet and EVT (StdPeriphDriver)
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ch573.h"

#if defined(__has_include)
  #if __has_include("config.h")
    #include "config.h"                     // optional: may change SYS_CLK32K etc.
  #endif
#endif

// ===================================================================================
// System Options (set "1" to activate)
// ===================================================================================
#define SYS_CLK_INIT      1         // 1: init system clock on startup
#define SYS_TICK_INIT     1         // 1: init and start SYSTICK on startup
#define SYS_CLEAR_BSS     1         // 1: clear uninitialized variables
#define SYS_USE_VECTORS   1         // 1: create interrupt vector table
#define SYS_DCDC_EN       0         // 1: enable DC/DC converter on startup
#ifndef SYS_CLK32K
  #define SYS_CLK32K      0         // 32 kHz clock source: 0: LSI (internal), 1: LSE (crystal)
#endif

// ===================================================================================
// Sytem Clock Defines
// ===================================================================================
// Set system clock frequency
#ifndef F_CPU
  #define F_CPU           60000000  // 60 Mhz if not otherwise defined
#endif

// Calculate system clock settings: mode (HSE or PLL) and divider
// Fsys = 32MHz / div (HSE) or 480MHz / div (PLL), div: 1..31, 0 means 32
#if   F_CPU == 60000000
  #define SYS_CLK_CFG     ((1 << 6) |  8)     // PLL / 8
#elif F_CPU == 48000000
  #define SYS_CLK_CFG     ((1 << 6) | 10)     // PLL / 10
#elif F_CPU == 40000000
  #define SYS_CLK_CFG     ((1 << 6) | 12)     // PLL / 12
#elif F_CPU == 32000000
  #define SYS_CLK_CFG     ((1 << 6) | 15)     // PLL / 15
#elif F_CPU == 30000000
  #define SYS_CLK_CFG     ((1 << 6) | 16)     // PLL / 16
#elif F_CPU == 24000000
  #define SYS_CLK_CFG     ((1 << 6) | 20)     // PLL / 20
#elif F_CPU == 20000000
  #define SYS_CLK_CFG     ((1 << 6) | 24)     // PLL / 24
#elif F_CPU == 16000000
  #define SYS_CLK_CFG     ((1 << 6) | 30)     // PLL / 30
#elif F_CPU == 15000000
  #define SYS_CLK_CFG     ((1 << 6) |  0)     // PLL / 32
#elif F_CPU ==  8000000
  #define SYS_CLK_CFG     ((0 << 6) |  4)     // HSE / 4
#elif F_CPU ==  6400000
  #define SYS_CLK_CFG     ((0 << 6) |  5)     // HSE / 5
#elif F_CPU ==  4000000
  #define SYS_CLK_CFG     ((0 << 6) |  8)     // HSE / 8
#elif F_CPU ==  3200000
  #define SYS_CLK_CFG     ((0 << 6) | 10)     // HSE / 10
#elif F_CPU ==  2000000
  #define SYS_CLK_CFG     ((0 << 6) | 16)     // HSE / 16
#else
  #warning Unsupported system clock frequency, using 60 MHz
  #define SYS_CLK_CFG     ((1 << 6) |  8)
  #undef  F_CPU
  #define F_CPU           60000000
#endif

#define SYS_CLK_PLL       (SYS_CLK_CFG & (1 << 6))   // system clock derived from PLL?

// ===================================================================================
// Protected Register Access (SAFE ACCESS MODE)
// ===================================================================================
#define SAFE_ACCESS_enable()  (R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1, \
                               R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2)
#define SAFE_ACCESS_disable() R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG0
#define SAFE_ACCESS           for(uint8_t __s = (SAFE_ACCESS_enable(), 1); __s; \
                                  __s = 0, SAFE_ACCESS_disable())

// Code which must be executed from RAM (e.g. while the flash is accessed), every
// function gets its own section, so that unused functions are removed by the linker
#define RAM_FUNC          __attribute__((section(".highcode.L" RAM_STR(__LINE__)), noinline))
#define RAM_STR(x)        RAM_STR2(x)
#define RAM_STR2(x)       #x

// ===================================================================================
// System Clock Functions
// ===================================================================================
void     CLK_init(void);                        // init system clock according to F_CPU
void     CLK_reset(void);                       // reset system clock to HSE/5 = 6.4MHz
uint32_t CLK_getFreq(void);                     // get current system clock frequency

#define HSE_enable()      {SAFE_ACCESS R8_HFCK_PWR_CTRL |=  RB_CLK_XT32M_PON;}
#define HSE_disable()     {SAFE_ACCESS R8_HFCK_PWR_CTRL &= ~RB_CLK_XT32M_PON;}
#define PLL_enable()      {SAFE_ACCESS R8_HFCK_PWR_CTRL |=  RB_CLK_PLL_PON;}
#define PLL_disable()     {SAFE_ACCESS R8_HFCK_PWR_CTRL &= ~RB_CLK_PLL_PON;}

void DCDC_enable(void);                         // enable DC/DC converter
#define DCDC_disable()    {SAFE_ACCESS R16_POWER_PLAN &= ~(RB_PWR_DCDC_EN | RB_PWR_DCDC_PRE);}

// ===================================================================================
// Low-Speed 32kHz Clock (CLK32K) Functions
// ===================================================================================
void CLK32K_init(void);                         // init 32kHz clock (SYS_CLK32K)
void LSI_calibrate(void);                       // calibrate LSI to 32768Hz

#define LSI_enable()      {SAFE_ACCESS R8_CK32K_CONFIG |=  RB_CLK_INT32K_PON;}
#define LSI_disable()     {SAFE_ACCESS R8_CK32K_CONFIG &= ~RB_CLK_INT32K_PON;}
#define LSE_enable()      {SAFE_ACCESS R8_CK32K_CONFIG |=  RB_CLK_XT32K_PON;}
#define LSE_disable()     {SAFE_ACCESS R8_CK32K_CONFIG &= ~RB_CLK_XT32K_PON;}
#define CLK32K_selectLSI() {SAFE_ACCESS R8_CK32K_CONFIG &= ~RB_CLK_OSC32K_XT;}
#define CLK32K_selectLSE() {SAFE_ACCESS R8_CK32K_CONFIG |=  RB_CLK_OSC32K_XT;}
#define CLK32K_isLSE()    (R8_CK32K_CONFIG & RB_CLK_OSC32K_XT)

extern uint16_t LSI_tune;                       // calibration value of LSI

// ===================================================================================
// Delay (DLY) Functions (SysTick, 64-bit down-counter with HCLK)
// ===================================================================================
void DLY_ticks(uint32_t n);                     // delay n system ticks

#define STK_init()        {STK->CMPLR = 0xFFFFFFFF; STK->CMPHR = 0xFFFFFFFF; \
                           STK->CTLR  = STK_CTLR_STRELOAD | STK_CTLR_STCLK | STK_CTLR_STE;}
#define STK_getTicks()    (~STK->CNTL)          // system ticks since start (lower 32 bits)
#define DLY_US_TIME       (F_CPU / 1000000)                   // system ticks per us
#define DLY_MS_TIME       (F_CPU / 1000)                      // system ticks per ms
#if F_CPU % 1000000 == 0
  #define DLY_us(n)       DLY_ticks((n) * DLY_US_TIME)        // delay n microseconds
#else
  #define DLY_us(n)       DLY_ticks((n) * DLY_MS_TIME / 1000) // delay n microseconds
#endif
#define DLY_ms(n)         DLY_ticks((n) * DLY_MS_TIME)        // delay n milliseconds
#define DLY_cycles        DLY_ticks                           // alias

// ===================================================================================
// Reset (RST) Functions
// ===================================================================================
void RST_now(void);                                 // conduct software reset

#define RST_getFlags()    (R8_RESET_STATUS & RB_RESET_FLAG)
#define RST_wasSoftware() (RST_getFlags() == RST_FLAG_SW)
#define RST_wasPower()    (RST_getFlags() == RST_FLAG_RPOR)
#define RST_wasWDT()      (RST_getFlags() == RST_FLAG_WTR)
#define RST_wasPin()      (RST_getFlags() == RST_FLAG_MR)
#define RST_wasWakeup()   (RST_getFlags() == RST_FLAG_GPWSM)

// ===================================================================================
// Watchdog Timer (WDT) Functions
// ===================================================================================
// The 8-bit watchdog counter is clocked with F_CPU/131072 and causes a reset when it
// overflows from 0xFF to 0x00. The maximum period therefore depends on F_CPU.
#define WDT_TICK_US       (131072000000ULL / F_CPU)              // us per WDT tick
#define WDT_MAX_MS        ((uint32_t)(255 * WDT_TICK_US / 1000)) // max period in ms
#define WDT_COUNT(n)      ((uint8_t)(256 - ((uint64_t)(n) * 1000 / WDT_TICK_US)))

extern uint8_t WDT_value;                       // reload value of watchdog counter

#define WDT_reload(n)     {WDT_value = WDT_COUNT(n); R8_WDOG_COUNT = WDT_value;}
#define WDT_start(n)      {WDT_reload(n); SAFE_ACCESS R8_RST_WDOG_CTRL |= RB_WDOG_RST_EN;}
#define WDT_stop()        {SAFE_ACCESS R8_RST_WDOG_CTRL &= ~RB_WDOG_RST_EN;}
#define WDT_feed()        R8_WDOG_COUNT = WDT_value
#define WDT_reset         WDT_feed              // alias

// ===================================================================================
// Real-Time Clock (RTC) Functions
// ===================================================================================
// The RTC counts 32768Hz ticks (low word, wraps every 2 seconds), 2-second steps
// (high word, wraps at midnight) and days. It requires the 32kHz clock (LSI or LSE).
void     RTC_init(void);                        // init 32kHz clock and start RTC
void     RTC_setTime(uint8_t h, uint8_t m, uint8_t s); // set time of day
void     RTC_setDay(uint16_t d);                // set day counter
uint32_t RTC_getTime(void);                     // get seconds of the day
uint32_t RTC_getTicks(void);                    // get 32768Hz ticks since midnight
void     RTC_TRIG_set(uint32_t n);              // set trigger n ticks from now

#define RTC_getDay()      ((uint16_t)(R32_RTC_CNT_DAY & 0x3FFF))
#define RTC_getHours(t)   ((t) / 3600)
#define RTC_getMinutes(t) ((t) / 60 % 60)
#define RTC_getSeconds(t) ((t) % 60)

#define RTC_TICKS_DAY     0xA8C00000            // 32768Hz ticks per day
#define RTC_MS_TICKS(n)   ((uint32_t)(((uint64_t)(n) * 32768 + 500) / 1000))

#define RTC_TRIG_stop()   {SAFE_ACCESS R8_RTC_MODE_CTRL &= ~RB_RTC_TRIG_EN;}
#define RTC_TRIG_flag()   (R8_RTC_FLAG_CTRL & RB_RTC_TRIG_FLAG)
#define RTC_TRIG_clear()  R8_RTC_FLAG_CTRL = RB_RTC_TRIG_CLR

#define RTC_TMR_125MS     0
#define RTC_TMR_250MS     1
#define RTC_TMR_500MS     2
#define RTC_TMR_1S        3
#define RTC_TMR_2S        4
#define RTC_TMR_4S        5
#define RTC_TMR_8S        6
#define RTC_TMR_16S       7

#define RTC_TMR_start(p)  {SAFE_ACCESS R8_RTC_MODE_CTRL = (R8_RTC_MODE_CTRL & \
                           ~(RB_RTC_TMR_MODE | RB_RTC_LOAD_LO | RB_RTC_LOAD_HI)) | RB_RTC_TMR_EN | (p);}
#define RTC_TMR_stop()    {SAFE_ACCESS R8_RTC_MODE_CTRL &= ~RB_RTC_TMR_EN;}
#define RTC_TMR_flag()    (R8_RTC_FLAG_CTRL & RB_RTC_TMR_FLAG)
#define RTC_TMR_clear()   R8_RTC_FLAG_CTRL = RB_RTC_TMR_CLR

#define RTC_INT_enable()  NVIC_EnableIRQ(RTC_IRQn)
#define RTC_INT_disable() NVIC_DisableIRQ(RTC_IRQn)
#define RTC_ISR           void RTC_IRQHandler(void) __attribute__((interrupt)); \
                          void RTC_IRQHandler(void)

// ===================================================================================
// Sleep Functions
// ===================================================================================
void IDLE_now(void);                            // stop CPU, wake up by interrupt
void HALT_now(void);                            // stop clocks, wake up by event
void SLEEP_now(void);                           // power down periph, wake by event
void SHUTDOWN_now(void);                        // power down all, wake-up -> reset
void SLEEP_ms(uint32_t n);                      // SLEEP for n ms (RTC trigger)
void HALT_ms(uint32_t n);                       // HALT for n ms (RTC trigger)

#define WAKE_RTC_enable()   {SAFE_ACCESS R8_SLP_WAKE_CTRL |= RB_SLP_RTC_WAKE  | RB_WAKE_EV_MODE;}
#define WAKE_GPIO_enable()  {SAFE_ACCESS R8_SLP_WAKE_CTRL |= RB_SLP_GPIO_WAKE | RB_WAKE_EV_MODE;}
#define WAKE_USB_enable()   {SAFE_ACCESS R8_SLP_WAKE_CTRL |= RB_SLP_USB_WAKE  | RB_WAKE_EV_MODE;}
#define WAKE_disable()      {SAFE_ACCESS R8_SLP_WAKE_CTRL &= ~(RB_SLP_RTC_WAKE | RB_SLP_GPIO_WAKE | \
                                                               RB_SLP_USB_WAKE | RB_SLP_BAT_WAKE);}

// ===================================================================================
// Battery Voltage Detector (BAT) Functions
// ===================================================================================
#define BAT_LEVEL_1V95    0                     // detection level (low voltage)
#define BAT_LEVEL_2V15    1
#define BAT_LEVEL_2V35    2
#define BAT_LEVEL_2V55    3

#define BAT_enable(n)     {SAFE_ACCESS {R8_BAT_DET_CFG = (n); R8_BAT_DET_CTRL = RB_BAT_DET_EN;}}
#define BAT_disable()     {SAFE_ACCESS R8_BAT_DET_CTRL = 0;}
#define BAT_isLow()       (R8_BAT_STATUS & RB_BAT_STAT_LOW)

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
  __asm volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrrci %0, mstatus, 0x08" : "=r" (result) : : "memory"
  );
  return result;
}

// Restore interrupt status
static inline void __iRestore(const uint32_t *__s) {
  if(*__s & 0x08) {
    __asm volatile(
      #if __GNUC__ > 10
      ".option arch, +zicsr \n"
      #endif
      "csrsi mstatus, 0x08" : : : "memory"
    );
  }
}

// ===================================================================================
// Device Electronic Signature (ESIG)
// ===================================================================================
#define ESIG_CHIPID         R8_CHIP_ID          // chip ID (0x73: CH573, 0x71: CH571)
#define ESIG_TEMP25         (*(__I uint32_t*)0x7F014) // temperature sensor calibration

void ESIG_getUID(uint8_t* buf);                 // read 8-byte unique ID
void ESIG_getMAC(uint8_t* buf);                 // read 6-byte MAC address

// ===================================================================================
// Low-Level Flash ROM Access (used by ESIG, sleep and flash library)
// ===================================================================================
void    FLASH_ROM_begin(uint8_t cmd);           // start flash command
void    FLASH_ROM_end(void);                    // end flash command
uint8_t FLASH_ROM_in(void);                     // read byte from flash
void    FLASH_ROM_out(uint8_t data);            // write byte to flash
void    FLASH_ROM_addr(uint8_t cmd, uint32_t addr); // send command with address
uint8_t FLASH_ROM_wait(void);                   // wait for erase/write, 0 = timeout
void    FLASH_ROM_open(uint8_t mode);           // open flash access (disables interrupts)
void    FLASH_ROM_close(void);                  // close flash access (restores interrupts)
void    FLASH_ROM_reset(void);                  // software reset of flash ROM
void    FLASH_ROM_info(uint32_t addr, uint32_t* buf); // read chip information

// ===================================================================================
// Imported System Functions
// ===================================================================================
// Enable Global Interrupt
__attribute__((always_inline)) static inline void __enable_irq(void) {
  __asm volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrsi mstatus, 0x08" : : : "memory"
  );
}

// Disable Global Interrupt
__attribute__((always_inline)) static inline void __disable_irq(void) {
  __asm volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrci mstatus, 0x08" : : : "memory"
  );
}

// Return the Machine Status Register
__attribute__((always_inline)) static inline uint32_t __get_MSTATUS(void) {
  uint32_t result;
  __asm volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrr %0, mstatus" : "=r"(result)
  );
  return (result);
}

// Set the Machine Status Register
__attribute__((always_inline)) static inline void __set_MSTATUS(uint32_t value) {
  __asm volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrw mstatus, %0" : : "r"(value)
  );
}

// Return the Machine Exception Program Register
__attribute__((always_inline)) static inline uint32_t __get_MEPC(void) {
  uint32_t result;
  __asm volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrr %0, mepc" : "=r"(result)
  );
  return (result);
}

// Return the Machine Cause Register
__attribute__((always_inline)) static inline uint32_t __get_MCAUSE(void) {
  uint32_t result;
  __asm volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrr %0, mcause": "=r"(result)
  );
  return (result);
}

// Return Machine Architecture ID Register
__attribute__((always_inline)) static inline uint32_t __get_MARCHID(void) {
  uint32_t result;
  __asm volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrr %0, marchid" : "=r"(result)
  );
  return (result);
}

// Return Machine Implementation ID Register
__attribute__((always_inline)) static inline uint32_t __get_MIMPID(void) {
  uint32_t result;
  __asm volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrr %0, mimpid" : "=r"(result)
  );
  return (result);
}

// Return SP Register
__attribute__((always_inline)) static inline uint32_t __get_SP(void) {
  uint32_t result;
  __asm volatile("mv %0, sp" : "=r"(result):);
  return (result);
}

// No OPeration
__attribute__((always_inline)) static inline void __NOP(void) {
  __asm volatile("nop");
}

// Enable NVIC interrupt (interrupt numbers)
__attribute__((always_inline)) static inline void NVIC_EnableIRQ(IRQn_Type IRQn) {
  NVIC->IENR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

// Disable NVIC interrupt (interrupt numbers)
__attribute__((always_inline)) static inline void NVIC_DisableIRQ(IRQn_Type IRQn) {
  uint32_t t = NVIC->ITHRESDR;
  NVIC->ITHRESDR = 0x10;
  NVIC->IRER[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
  NVIC->ITHRESDR = t;
  __NOP(); __NOP();
}

// Get Interrupt Enable State
__attribute__((always_inline)) static inline uint32_t NVIC_GetStatusIRQ(IRQn_Type IRQn) {
  return((uint32_t) ((NVIC->ISR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0));
}

// Get Interrupt Pending State
__attribute__((always_inline)) static inline uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn) {
  return((uint32_t) ((NVIC->IPR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0));
}

// Set Interrupt Pending
__attribute__((always_inline)) static inline void NVIC_SetPendingIRQ(IRQn_Type IRQn) {
  NVIC->IPSR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

// Clear Interrupt Pending
__attribute__((always_inline)) static inline void NVIC_ClearPendingIRQ(IRQn_Type IRQn) {
  NVIC->IPRR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

// Get Interrupt Active State
__attribute__((always_inline)) static inline uint32_t NVIC_GetActive(IRQn_Type IRQn) {
  return((uint32_t)((NVIC->IACTR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0));
}

// Set Interrupt Priority (bit 7..6, lower value = higher priority)
__attribute__((always_inline)) static inline void NVIC_SetPriority(IRQn_Type IRQn, uint8_t priority) {
  NVIC->IPRIOR[(uint32_t)(IRQn)] = priority;
}

// Wait for Interrupt
__attribute__((always_inline)) static inline void __WFI(void) {
  NVIC->SCTLR &= ~PFIC_WFITOWFE;
  __asm volatile ("wfi");
}

// Wait for Events
__attribute__((always_inline)) static inline void __WFE(void) {
  NVIC->SCTLR |= PFIC_WFITOWFE | PFIC_SETEVENT;   // (wfi->wfe) + (__sev)
  __asm volatile ("wfi");
  NVIC->SCTLR |= PFIC_WFITOWFE;
  __asm volatile ("wfi");
}

// Initiate a system reset request
__attribute__((always_inline)) static inline void NVIC_SystemReset(void) {
  NVIC->CFGR = NVIC_KEY3 | PFIC_RESETSYS;
}

#ifdef __cplusplus
};
#endif
