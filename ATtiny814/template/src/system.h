// ===================================================================================
// Basic System Functions for tinyAVR 0-Series and 1-Series                   * v0.1 *
// ===================================================================================
//
// System clock functions available:
// ---------------------------------
// CLK_init()               init internal oscillator according to F_CPU
//
// MCO_enable()             enable system clock output on pin PB5
// MCO_disable()            disable system clock output
//
// HSI_ready()              check if high-speed internal oscillator OSC20M is stable
// HSI_STDBY_on()           enable OSC20M in STANDBY sleep mode
// HSI_STDBY_off()          disable OSC20M in STANDBY sleep mode
// LSI_ready()              check if low-speed internal oscillator OSCULP32K is stable
// LSI_STDBY_on()           enable OSCULP32K in STANDBY sleep mode
// LSI_STDBY_off()          disable OSCULP32K in STANDBY sleep mode
// HSE_ready()              check if high-speed external clock EXTCLK has started
// LSE_enable()             enable external 32.768kHz low-speed crystal XOSC32K
// LSE_disable()            disable XOSC32K
// LSE_ready()              check if XOSC32K is stable
// LSE_STDBY_on()           enable XOSC32K in STANDBY sleep mode
// LSE_STDBY_off()          disable XOSC32K in STANDBY sleep mode
//
// Real-Time Counter (RTC) functions available:
// --------------------------------------------
// RTC_enable(p)            enable RTC with clock prescaler p 
//                          (p = 1, 2, 4, 8, ..., 32768)
// RTC_disable()            disable RTC
// RTC_STDBY_on()           enable RTC in STANDBY sleep mode
// RTC_STDBY_off()          disable RTC in STANDBY sleep mode
// RTC_busy()               check if RTC is busy synchronizing
//
// RTC_setLSI()             set internal 32.768kHz (OSCULP32K) as RTC clock source
// RTC_setLSI_1K()          set internal  1.024kHz (OSCULP32K) as RTC clock source
// RTC_setLSE()             set external 32.768kHz (XOSC32K)   as RTC clock source
// RTC_setHSE()             set external clock signal (EXTCLK) as RTC clock source
//
// RTC_CNT_read()           read  RTC counter (16-bit)
// RTC_CNT_write(n)         write RTC counter (16-bit)
// RTC_PER_read()           read  RTC period (16-bit)
// RTC_PER_write(n)         write RTC period (16-bit)
// RTC_CMP_read()           read  RTC compare value (16-bit)
// RTC_CMP_write(n)         write RTC compare value (16-bit)
//
// RTC_CMP_INT_enable()     enable RTC compare match interrupt
// RTC_CMP_INT_disable()    disable RTC compare match interrupt
// RTC_CMP_FLAG_read()      read RTC compare match interrupt flag
// RTC_CMP_FLAG_clear()     clear RTC compare match interrupt flag
//
// RTC_OVF_INT_enable()     enable RTC overflow interrupt
// RTC_OVF_INT_disable()    disable RTC overflow interrupt
// RTC_OVF_FLAG_read()      read RTC overflow interrupt flag
// RTC_OVF_FLAG_clear()     clear RTC overflow interrupt flag
//
// Periodic Interrupt Timer (PIT) functions available:
// ---------------------------------------------------
// PIT_enable(p)            enable PIT with period p in RTC clock cycles 
//                          (p = 4, 8, 16, ..., 32768)
// PIT_disable()            disable PIT
// PIT_INT_enable()         enable PIT interrupt
// PIT_INT_disable()        disable PIT interrupt
// PIT_FLAG_read()          read PIT interrupt flag
// PIT_FLAG_clear()         clear PIT interrupt flag
//
// Watchdog Timer (WDT) functions available:
// -----------------------------------------
// WDT_start(ms)            start WDT with period in ms (8, 16, 32, ..., 8192)
// WDT_stop()               stop watchdog timer
// WDT_reset()              reset watchdog timer
// WDT_feed()               reset watchdog timer (alias)
//
// Delay (DLY) functions available:
// --------------------------------
// DLY_ticks(n)             delay n clock cycles
// DLY_cycles(n)            delay n clock cycles (alias)
// DLY_us(n)                delay n microseconds
// DLY_ms(n)                delay n milliseconds
//
// Sleep functions available:
// --------------------------
// SLEEP_set_IDLE()         set sleep mode to IDLE
// SLEEP_set_STDBY()        set sleep mode to STANDBY
// SLEEP_set_PDOWN()        set sleep mode to POWER-DOWN
// SLEEP_now()              start sleep mode
//
// Reset (RST) functions available:
// --------------------------------
// RST_now()                conduct software reset
// RST_clearFlags()         clear all reset flags
// RST_wasUPDI()            check if last reset was caused by UPDI
// RST_wasSoftware()        check if last reset was caused by software
// RST_wasWDT()             check if last reset was caused by watchdog
// RST_wasExternal()        check if last reset was caused by external
// RST_wasBrownout()        check if last reset was caused by brown-out
// RST_wasPower()           check if last reset was caused by power-on
//
// Interrupt (INT) functions available:
// ------------------------------------
// INT_enable()             global interrupt enable
// INT_disable()            global interrupt disable
// INT_SREG_read()          read status register (SREG)
// INT_SREG_write(v)        write status register (SREG)
// INT_ATOMIC_BLOCK { }     execute block without being interrupted
//
// EEPROM functions available:
// ---------------------------
// EEPROM_read(a)             read  8-bit byte from EEPROM address a
// EEPROM_read_byte(a)        read  8-bit byte from EEPROM address a (alias)
// EEPROM_read_word(a)        read 16-bit word from EEPROM address a
// EEPROM_read_block(p,a,l)   read block of length l from EEPROM address a to pointer p
// EEPROM_write(a,b)          write (update)  8-bit byte b to EEPROM address a
// EEPROM_write_byte(a,b)     write (update)  8-bit byte b to EEPROM address a (alias)
// EEPROM_write_word(a,w)     write (update) 16-bit word w to EEPROM address a
// EEPROM_write_block(p,a,l)  write block of length l to EEPROM address a from pointer p
//
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h> 
#include <util/atomic.h>

// ===================================================================================
// System Clock (CLK) Functions
// ===================================================================================
#ifndef F_CPU
  #define F_CPU               3333333   // default
#endif

#if   F_CPU == 20000000 || F_CPU == 16000000
  #define CLK_init()          _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0)
#elif F_CPU == 10000000 || F_CPU ==  8000000
  #define CLK_init()          _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 1)
#elif F_CPU ==  5000000 || F_CPU ==  4000000
  #define CLK_init()          _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 3)
#elif F_CPU ==  2500000 || F_CPU ==  2000000
  #define CLK_init()          _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 5)
#elif F_CPU ==  1250000 || F_CPU ==  1000000
  #define CLK_init()          _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 7)
#elif F_CPU ==   625000 || F_CPU ==   500000
  #define CLK_init()          _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 9)
#elif F_CPU ==   312500 || F_CPU ==   250000
  #define CLK_init()          _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 11)
#elif F_CPU ==  3333333 || F_CPU ==  2666667
  #define CLK_init()          _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 17)
#elif F_CPU ==  2000000 || F_CPU ==  1600000
  #define CLK_init()          _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 19)
#elif F_CPU ==  1666667 || F_CPU ==  1333333
  #define CLK_init()          _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 21)
#elif F_CPU ==   833333 || F_CPU ==   666667
  #define CLK_init()          _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 23)
#elif F_CPU ==   416667 || F_CPU ==   333333
  #define CLK_init()          _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 23)
#else
  #warning Unsupported system clock frequency, using default.
  #define CLK_init()
#endif

#define MCO_enable()          _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, 0x80)
#define MCO_disable()         _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, 0x00)

#define HSI_ready()           (CLKCTRL.MCLKSTATUS & 0x10)
#define HSI_STDBY_on()        _PROTECTED_WRITE(CLKCTRL.OSC20MCTRLA, 0x02)
#define HSI_STDBY_off()       _PROTECTED_WRITE(CLKCTRL.OSC20MCTRLA, 0x00)

#define HSE_ready()           (CLKCTRL.MCLKSTATUS & 0x80)

#define LSI_ready()           (CLKCTRL.MCLKSTATUS & 0x20)
#define LSI_STDBY_on()        _PROTECTED_WRITE(CLKCTRL.OSC32KCTRLA, 0x02)
#define LSI_STDBY_off()       _PROTECTED_WRITE(CLKCTRL.OSC32KCTRLA, 0x00)

#define LSE_enable()          _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, 0x01)
#define LSE_disable()         _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, 0x00)
#define LSE_ready()           (CLKCTRL.MCLKSTATUS & 0x40)
#define LSE_STDBY_on()        _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, CLKCTRL.OSC32KCTRLA | 0x02)
#define LSE_STDBY_off()       _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, CLKCTRL.OSC32KCTRLA & 0xfe)

// ===================================================================================
// Real-Time Counter (RTC) Functions
// ===================================================================================
#define RTC_enable(p) {                                     \
  while(RTC.STATUS & RTC_CTRLABUSY_bm);                     \
  (p <=     1 ? (RTC.PITCTRLA = RTC_PITEN_bm | ( 0 << 3)) : \
  (p <=     2 ? (RTC.PITCTRLA = RTC_PITEN_bm | ( 1 << 3)) : \
  (p <=     4 ? (RTC.PITCTRLA = RTC_PITEN_bm | ( 2 << 3)) : \
  (p <=     8 ? (RTC.PITCTRLA = RTC_PITEN_bm | ( 3 << 3)) : \
  (p <=    16 ? (RTC.PITCTRLA = RTC_PITEN_bm | ( 4 << 3)) : \
  (p <=    32 ? (RTC.PITCTRLA = RTC_PITEN_bm | ( 5 << 3)) : \
  (p <=    64 ? (RTC.PITCTRLA = RTC_PITEN_bm | ( 6 << 3)) : \
  (p <=   128 ? (RTC.PITCTRLA = RTC_PITEN_bm | ( 7 << 3)) : \
  (p <=   256 ? (RTC.PITCTRLA = RTC_PITEN_bm | ( 8 << 3)) : \
  (p <=   512 ? (RTC.PITCTRLA = RTC_PITEN_bm | ( 9 << 3)) : \
  (p <=  1024 ? (RTC.PITCTRLA = RTC_PITEN_bm | (10 << 3)) : \
  (p <=  2048 ? (RTC.PITCTRLA = RTC_PITEN_bm | (11 << 3)) : \
  (p <=  4096 ? (RTC.PITCTRLA = RTC_PITEN_bm | (12 << 3)) : \
  (p <=  8192 ? (RTC.PITCTRLA = RTC_PITEN_bm | (13 << 3)) : \
  (p <= 16384 ? (RTC.PITCTRLA = RTC_PITEN_bm | (14 << 3)) : \
                (RTC.PITCTRLA = RTC_PITEN_bm | (15 << 3))))))))))))))); \
}

#define RTC_disable()         {while(RTC.STATUS & RTC_CTRLABUSY_bm); RTC.CTRLA &= ~RTC_RTCEN_bm;}
#define RTC_STDBY_on()        {while(RTC.STATUS & RTC_CTRLABUSY_bm); RTC.CTRLA |=  RTC_RUNSTDBY_bm;}
#define RTC_STDBY_off()       {while(RTC.STATUS & RTC_CTRLABUSY_bm); RTC.CTRLA &= ~RTC_RUNSTDBY_bm;}
#define RTC_busy()            (RTC.STATUS & 0x0f)
#define RTC_setLSI()          RTC.CLKSEL = 0
#define RTC_setLSI_1K()       RTC.CLKSEL = 1
#define RTC_setLSE()          RTC.CLKSEL = 2
#define RTC_setHSE()          RTC.CLKSEL = 3
#define RTC_CNT_read()        (RTC.CNT)
#define RTC_CNT_write(n)      {while(RTC.STATUS & RTC_PERBUSY_bm); RTC.CNT = n;}
#define RTC_PER_read()        (RTC.PER)
#define RTC_PER_write(n)      {while(RTC.STATUS & RTC_PERBUSY_bm); RTC.PER = n;}
#define RTC_CMP_read()        (RTC.CMP)
#define RTC_CMP_write(n)      {while(RTC.STATUS & RTC_CMPBUSY_bm); RTC.CMP = n;}

#define RTC_CMP_INT_enable()  RTC.INTCTRL |=  RTC_CMP_bm
#define RTC_CMP_INT_disable() RTC.INTCTRL &= ~RTC_CMP_bm
#define RTC_CMP_FLAG_read()   (RTC.INTFLAGS & RTC_CMP_bm)
#define RTC_CMP_FLAG_clear()  RTC.INTFLAGS =  RTC_CMP_bm

#define RTC_OVF_INT_enable()  RTC.INTCTRL |=  RTC_OVF_bm
#define RTC_OVF_INT_disable() RTC.INTCTRL &= ~RTC_OFV_bm
#define RTC_OVF_FLAG_read()   (RTC.INTFLAGS & RTC_OVF_bm)
#define RTC_OVF_FLAG_clear()  RTC.INTFLAGS =  RTC_OVF_bm

#define PIT_enable(p) {                                     \
  while(RTC.PITSTATUS & RTC_CTRLBUSY_bm);                   \
  (p <=     4 ? (RTC.PITCTRLA = RTC_PITEN_bm | ( 1 << 3)) : \
  (p <=     8 ? (RTC.PITCTRLA = RTC_PITEN_bm | ( 2 << 3)) : \
  (p <=    16 ? (RTC.PITCTRLA = RTC_PITEN_bm | ( 3 << 3)) : \
  (p <=    32 ? (RTC.PITCTRLA = RTC_PITEN_bm | ( 4 << 3)) : \
  (p <=    64 ? (RTC.PITCTRLA = RTC_PITEN_bm | ( 5 << 3)) : \
  (p <=   128 ? (RTC.PITCTRLA = RTC_PITEN_bm | ( 6 << 3)) : \
  (p <=   256 ? (RTC.PITCTRLA = RTC_PITEN_bm | ( 7 << 3)) : \
  (p <=   512 ? (RTC.PITCTRLA = RTC_PITEN_bm | ( 8 << 3)) : \
  (p <=  1024 ? (RTC.PITCTRLA = RTC_PITEN_bm | ( 9 << 3)) : \
  (p <=  2048 ? (RTC.PITCTRLA = RTC_PITEN_bm | (10 << 3)) : \
  (p <=  4096 ? (RTC.PITCTRLA = RTC_PITEN_bm | (11 << 3)) : \
  (p <=  8192 ? (RTC.PITCTRLA = RTC_PITEN_bm | (12 << 3)) : \
  (p <= 16384 ? (RTC.PITCTRLA = RTC_PITEN_bm | (13 << 3)) : \
                (RTC.PITCTRLA = RTC_PITEN_bm | (14 << 3))))))))))))))); \
}
#define PIT_disable()         {while(RTC.PITSTATUS & RTC_CTRLBUSY_bm); RTC.PITCTRLA = 0;}

#define PIT_INT_enable()      RTC.PITINTCTRL = RTC_PI_bm
#define PIT_INT_disable()     RTC.PITINTCTRL = 0
#define PIT_FLAG_read()       (RTC.PITINTFLAGS & RTC_PI_bm)
#define PIT_FLAG_clear()      RTC.PITINTFLAGS = RTC_PI_bm

// ===================================================================================
// Watchdog Timer (WDT) Functions
// ===================================================================================
#define WDT_start(ms) {                                \
  while(WDT_busy());                                   \
  (ms <=    8 ? ({_PROTECTED_WRITE(WDT.CTRLA,  1);}) : \
  (ms <=   16 ? ({_PROTECTED_WRITE(WDT.CTRLA,  2);}) : \
  (ms <=   32 ? ({_PROTECTED_WRITE(WDT.CTRLA,  3);}) : \
  (ms <=   64 ? ({_PROTECTED_WRITE(WDT.CTRLA,  4);}) : \
  (ms <=  128 ? ({_PROTECTED_WRITE(WDT.CTRLA,  5);}) : \
  (ms <=  256 ? ({_PROTECTED_WRITE(WDT.CTRLA,  6);}) : \
  (ms <=  512 ? ({_PROTECTED_WRITE(WDT.CTRLA,  7);}) : \
  (ms <= 1024 ? ({_PROTECTED_WRITE(WDT.CTRLA,  8);}) : \
  (ms <= 2048 ? ({_PROTECTED_WRITE(WDT.CTRLA,  9);}) : \
  (ms <= 4096 ? ({_PROTECTED_WRITE(WDT.CTRLA, 10);}) : \
                ({_PROTECTED_WRITE(WDT.CTRLA, 11);}))))))))))); \
}

#define WDT_stop()            {while(WDT_busy()); _PROTECTED_WRITE(WDT.CTRLA, 0);}
#define WDT_reset()           asm("wdr")
#define WDT_feed()            asm("wdr")
#define WDT_busy()            (WDT.STATUS & WDT_SYNCBUSY_bm)

// ===================================================================================
// Delay (DLY) Functions
// ===================================================================================
#define DLY_ticks(n)          __builtin_avr_delay_cycles(n)
#define DLY_cycles(n)         __builtin_avr_delay_cycles(n)
#define DLY_us(n)             _delay_us(n)
#define DLY_ms(n)             _delay_ms(n)

// ===================================================================================
// Sleep Functions
// ===================================================================================
#define SLEEP_set_IDLE()      SLPCTRL.CTRLA = SLPCTRL_SMODE_IDLE_gc  | SLPCTRL_SEN_bm
#define SLEEP_set_STDBY()     SLPCTRL.CTRLA = SLPCTRL_SMODE_STDBY_gc | SLPCTRL_SEN_bm
#define SLEEP_set_PDOWN()     SLPCTRL.CTRLA = SLPCTRL_SMODE_PDOWN_gc | SLPCTRL_SEN_bm
#define SLEEP_now()           sleep_cpu()

// ===================================================================================
// Reset (RST) Functions
// ===================================================================================
#define RST_now()             _PROTECTED_WRITE(RSTCTRL.SWRR, 0x01)
#define RST_clearFlags()      _PROTECTED_WRITE(RSTCTRL.RSTFR, 0x3f)
#define RST_wasUPDI()         (RSTCTRL.RSTFR & RSTCTRL_UPDIRF_bm)
#define RST_wasSoftware()     (RSTCTRL.RSTFR & RSTCTRL_SWRF_bm)
#define RST_wasWDT()          (RSTCTRL.RSTFR & RSTCTRL_WDRF_bm)
#define RST_wasExternal()     (RSTCTRL.RSTFR & RSTCTRL_EXTRF_bm)
#define RST_wasBrownout()     (RSTCTRL.RSTFR & RSTCTRL_BORF_bm)
#define RST_wasPower()        (RSTCTRL.RSTFR & RSTCTRL_PORF_bm)

// ===================================================================================
// Interrupt (INT) Functions
// ===================================================================================
#define INT_enable()          sei()
#define INT_disable()         cli()
#define INT_SREG_read()       (SREG)
#define INT_SREG_write(v)     SREG = v
#define INT_ATOMIC_BLOCK      ATOMIC_BLOCK(ATOMIC_RESTORESTATE)

// ===================================================================================
// EEPROM Functions
// ===================================================================================
#define EEPROM_read(a)            eeprom_read_byte((const uint8_t*)a)
#define EEPROM_read_byte(a)       eeprom_read_byte((const uint8_t*)a)
#define EEPROM_read_word(a)       eeprom_read_word((const uint16_t*)a)
#define EEPROM_read_block(p,a,l)  eeprom_read_block((void*)&p, (const void*)a, l)
#define EEPROM_write(a,b)         eeprom_update_byte((uint8_t*)a, b)
#define EEPROM_write_byte(a,b)    eeprom_update_byte((uint8_t*)a, b)
#define EEPROM_write_word(a,w)    eeprom_update_word((uint16_t*)a, w)
#define EEPROM_write_block(p,a,l) eeprom_update_block((const void*)&p, (void*)a, l)

#ifdef __cplusplus
};
#endif
