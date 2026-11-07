// ===================================================================================
// Configurable Custom Logic (CCL) Functions for tinyAVR 0/1-Series           * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// CCL_enable()             enable CCL (LUTs can only be configured while disabled)
// CCL_disable()            disable CCL
// CCL_STDBY_on()           keep CCL running in STANDBY sleep mode
// CCL_STDBY_off()          disable CCL in STANDBY sleep mode
//
// CCL_LUT0_init(i0,i1,i2,t) configure LUT0 with inputs i0..i2 and truth table t, enable it
// CCL_LUT1_init(i0,i1,i2,t) configure LUT1 with inputs i0..i2 and truth table t, enable it
// CCL_LUT0_disable()       disable LUT0
// CCL_LUT1_disable()       disable LUT1
// CCL_LUT0_OUT_enable()    enable LUT0 output on pin (sets pin as output)
// CCL_LUT1_OUT_enable()    enable LUT1 output on pin (sets pin as output)
// CCL_LUT0_OUT_disable()   disable LUT0 output on pin
// CCL_LUT1_OUT_disable()   disable LUT1 output on pin
// CCL_LUT0_FILTER(n)       set LUT0 output filter (0: off, 1: synchronizer, 2: filter)
// CCL_LUT1_FILTER(n)       set LUT1 output filter (0: off, 1: synchronizer, 2: filter)
// CCL_LUT0_EDGE_on()       enable LUT0 edge detector (needs filter or synchronizer)
// CCL_LUT1_EDGE_on()       enable LUT1 edge detector (needs filter or synchronizer)
// CCL_SEQ_set(n)           set sequencer (0: off, 1: D flip-flop, 2: JK flip-flop,
//                          3: D latch, 4: RS latch); LUT0 = input 0, LUT1 = input 1
//
// LUT input sources (i0, i1, i2):
// -------------------------------
// CCL_IN_MASK              input masked (logic 0)
// CCL_IN_FEEDBACK          feedback from sequencer or own LUT output
// CCL_IN_LINK              output of the other LUT
// CCL_IN_EVENT0            event input 0 (event system)
// CCL_IN_EVENT1            event input 1 (event system)
// CCL_IN_IO                I/O pin (LUT0: PA0/PA1/PA2, LUT1: PC3/PC4/PC5)
// CCL_IN_AC0               analog comparator output
// CCL_IN_TCB0              TCB0 waveform output
// CCL_IN_TCA0              TCA0 waveform output WO0/WO1/WO2
// CCL_IN_TCD0              TCD0 waveform output WOA/WOB/WOA (1-series)
// CCL_IN_USART0            USART0 XCK (i0), TXD (i1)
// CCL_IN_SPI0              SPI0 SCK (i0), MOSI (i1), MISO (i2)
//
// Truth table: bit n of t is the output for input combination n = (i2<<2)|(i1<<1)|i0,
// e.g. AND(i0,i1) with i2 masked: 0x08, OR: 0x0E, XOR: 0x06, NOT(i0): 0x01.
//
// LUT output pin mapping (set below in CCL parameters):
// -----------------------------------------------------
// CCL_ALT        0     1
// LUT0-OUT      PA4   PB4    (8-pin devices: PA6)
// LUT1-OUT      PA7   PC1    (8-pin devices: PA7)
//
// Notes:
// ------
// - I/O pins used as LUT inputs must be configured as inputs.
// - PB4 and PC1 are only available on 20-pin devices.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "gpio.h"

// ===================================================================================
// CCL Parameters
// ===================================================================================
#define CCL_ALT           0             // bit0: alternative pin for LUT0, bit1: for LUT1

// ===================================================================================
// CCL Input Sources
// ===================================================================================
#define CCL_IN_MASK       0x00
#define CCL_IN_FEEDBACK   0x01
#define CCL_IN_LINK       0x02
#define CCL_IN_EVENT0     0x03
#define CCL_IN_EVENT1     0x04
#define CCL_IN_IO         0x05
#define CCL_IN_AC0        0x06
#define CCL_IN_TCB0       0x07
#define CCL_IN_TCA0       0x08
#define CCL_IN_TCD0       0x09
#define CCL_IN_USART0     0x0A
#define CCL_IN_SPI0       0x0B

// ===================================================================================
// CCL Functions and Macros
// ===================================================================================
#define CCL_enable()          CCL.CTRLA |=  CCL_ENABLE_bm
#define CCL_disable()         CCL.CTRLA &= ~CCL_ENABLE_bm
#define CCL_STDBY_on()        CCL.CTRLA |=  CCL_RUNSTDBY_bm
#define CCL_STDBY_off()       CCL.CTRLA &= ~CCL_RUNSTDBY_bm

#define CCL_LUT0_init(i0,i1,i2,t) do {                      \
  uint8_t _en = CCL.CTRLA & CCL_ENABLE_bm; CCL_disable();   \
  CCL.LUT0CTRLB = ((i1) << 4) | (i0); CCL.LUT0CTRLC = (i2); \
  CCL.TRUTH0 = (t); CCL.LUT0CTRLA |= CCL_ENABLE_bm;         \
  CCL.CTRLA |= _en; } while(0)
#define CCL_LUT1_init(i0,i1,i2,t) do {                      \
  uint8_t _en = CCL.CTRLA & CCL_ENABLE_bm; CCL_disable();   \
  CCL.LUT1CTRLB = ((i1) << 4) | (i0); CCL.LUT1CTRLC = (i2); \
  CCL.TRUTH1 = (t); CCL.LUT1CTRLA |= CCL_ENABLE_bm;         \
  CCL.CTRLA |= _en; } while(0)

// Modify LUT control register A (CCL must be disabled)
#define _CCL_LUTA(reg, clr, set) do {                       \
  uint8_t _en = CCL.CTRLA & CCL_ENABLE_bm; CCL_disable();   \
  reg = (reg & ~(clr)) | (set); CCL.CTRLA |= _en; } while(0)

#define CCL_LUT0_disable()    _CCL_LUTA(CCL.LUT0CTRLA, CCL_ENABLE_bm, 0)
#define CCL_LUT1_disable()    _CCL_LUTA(CCL.LUT1CTRLA, CCL_ENABLE_bm, 0)
#define CCL_LUT0_FILTER(n)    _CCL_LUTA(CCL.LUT0CTRLA, CCL_FILTSEL_gm, ((n) & 3) << 4)
#define CCL_LUT1_FILTER(n)    _CCL_LUTA(CCL.LUT1CTRLA, CCL_FILTSEL_gm, ((n) & 3) << 4)
#define CCL_LUT0_EDGE_on()    _CCL_LUTA(CCL.LUT0CTRLA, 0, CCL_EDGEDET_bm)
#define CCL_LUT1_EDGE_on()    _CCL_LUTA(CCL.LUT1CTRLA, 0, CCL_EDGEDET_bm)
#define CCL_SEQ_set(n)        _CCL_LUTA(CCL.SEQCTRL0, CCL_SEQSEL_gm, (n) & 7)

// LUT output pins
#ifndef PORTB
  #define CCL_LUT0_PIN        PA6
  #define CCL_LUT1_PIN        PA7
#else
  #define CCL_LUT0_PIN        ((CCL_ALT & 1) ? PB4 : PA4)
  #define CCL_LUT1_PIN        ((CCL_ALT & 2) ? PC1 : PA7)
#endif

#define CCL_LUT0_OUT_enable() do {                          \
  PORTMUX.CTRLA = (PORTMUX.CTRLA & ~PORTMUX_LUT0_bm) | ((CCL_ALT & 1) ? PORTMUX_LUT0_bm : 0); \
  PIN_output(CCL_LUT0_PIN); _CCL_LUTA(CCL.LUT0CTRLA, 0, CCL_OUTEN_bm); } while(0)
#define CCL_LUT1_OUT_enable() do {                          \
  PORTMUX.CTRLA = (PORTMUX.CTRLA & ~PORTMUX_LUT1_bm) | ((CCL_ALT & 2) ? PORTMUX_LUT1_bm : 0); \
  PIN_output(CCL_LUT1_PIN); _CCL_LUTA(CCL.LUT1CTRLA, 0, CCL_OUTEN_bm); } while(0)
#define CCL_LUT0_OUT_disable() _CCL_LUTA(CCL.LUT0CTRLA, CCL_OUTEN_bm, 0)
#define CCL_LUT1_OUT_disable() _CCL_LUTA(CCL.LUT1CTRLA, CCL_OUTEN_bm, 0)

#ifdef __cplusplus
};
#endif
