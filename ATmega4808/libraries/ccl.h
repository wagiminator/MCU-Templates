// ===================================================================================
// Configurable Custom Logic (CCL) Functions for megaAVR 0-Series             * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// CCL_enable()             enable CCL (LUTs can only be configured while disabled)
// CCL_disable()            disable CCL
// CCL_STDBY_on()           keep CCL running in STANDBY sleep mode
// CCL_STDBY_off()          disable CCL in STANDBY sleep mode
//
// CCL_LUT_init(n,i0,i1,i2,t) configure LUTn (n = 0..3) with inputs i0..i2 and truth
//                          table t and enable it (also: CCL_LUT0_init(i0,i1,i2,t) ..
//                          CCL_LUT3_init(i0,i1,i2,t))
// CCL_LUT_disable(n)       disable LUTn
// CCL_LUT_OUT_enable(n)    enable LUTn output on pin (sets pin as output)
// CCL_LUT_OUT_disable(n)   disable LUTn output on pin
// CCL_LUT_FILTER(n,f)      set LUTn output filter (0: off, 1: synchronizer, 2: filter)
// CCL_LUT_EDGE_on(n)       enable LUTn edge detector (needs filter or synchronizer)
// CCL_SEQ0_set(s)          set sequencer 0 (LUT0 = input 0, LUT1 = input 1)
// CCL_SEQ1_set(s)          set sequencer 1 (LUT2 = input 0, LUT3 = input 1)
//                          s = 0: off, 1: D flip-flop, 2: JK flip-flop, 3: D latch,
//                              4: RS latch
//
// LUT input sources (i0, i1, i2):
// -------------------------------
// CCL_IN_MASK              input masked (logic 0)
// CCL_IN_FEEDBACK          feedback from sequencer or own LUT output
// CCL_IN_LINK              output of LUT n+1 (LUT3: LUT0)
// CCL_IN_EVENTA            event input A (event system)
// CCL_IN_EVENTB            event input B (event system)
// CCL_IN_IO                I/O pin LUTn-IN0..2 (see below)
// CCL_IN_AC0               analog comparator output
// CCL_IN_USART             USART0 (i0), USART1 (i1), USART2 (i2) TXD
// CCL_IN_SPI0              SPI0 MOSI (i0), MISO (i1), SCK (i2)
// CCL_IN_TCA0              TCA0 waveform output WO0 (i0), WO1 (i1), WO2 (i2)
// CCL_IN_TCB               TCB0 (i0), TCB1 (i1), TCB2 (i2) waveform output
//
// Truth table: bit n of t is the output for input combination n = (i2<<2)|(i1<<1)|i0,
// e.g. AND(i0,i1) with i2 masked: 0x08, OR: 0x0E, XOR: 0x06, NOT(i0): 0x01.
//
// LUT pin mapping (output pin set below in CCL parameters):
// ---------------------------------------------------------
// LUT            0         1         2         3
// IN0..IN2    PA0-PA2   PC0-PC2   PD0-PD2   PF0-PF2
// OUT (ALT 0)    PA3       PC3       PD3       PF3
// OUT (ALT 1)    PA6       PC6       PD6       PF6
//
// Notes:
// ------
// - I/O pins used as LUT inputs must be configured as inputs.
// - PC6 is only available on 40/48-pin devices, PF2/PF3 not on 28-pin devices.
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
#define CCL_ALT           0             // bit n: alternative output pin for LUTn

// ===================================================================================
// CCL Input Sources
// ===================================================================================
#define CCL_IN_MASK       0x00
#define CCL_IN_FEEDBACK   0x01
#define CCL_IN_LINK       0x02
#define CCL_IN_EVENTA     0x03
#define CCL_IN_EVENTB     0x04
#define CCL_IN_IO         0x05
#define CCL_IN_AC0        0x06
#define CCL_IN_USART      0x08
#define CCL_IN_SPI0       0x09
#define CCL_IN_TCA0       0x0A
#define CCL_IN_TCB        0x0C

// ===================================================================================
// CCL Functions and Macros
// ===================================================================================
#define CCL_enable()          CCL.CTRLA |=  CCL_ENABLE_bm
#define CCL_disable()         CCL.CTRLA &= ~CCL_ENABLE_bm
#define CCL_STDBY_on()        CCL.CTRLA |=  CCL_RUNSTDBY_bm
#define CCL_STDBY_off()       CCL.CTRLA &= ~CCL_RUNSTDBY_bm

// LUT registers (internal)
#define _CCL_CTRLA(n)         (&CCL.LUT0CTRLA)[(n) << 2]
#define _CCL_CTRLB(n)         (&CCL.LUT0CTRLB)[(n) << 2]
#define _CCL_CTRLC(n)         (&CCL.LUT0CTRLC)[(n) << 2]
#define _CCL_TRUTH(n)         (&CCL.TRUTH0)[(n) << 2]

#define CCL_LUT_init(n,i0,i1,i2,t) do {                     \
  uint8_t _en = CCL.CTRLA & CCL_ENABLE_bm; CCL_disable();   \
  _CCL_CTRLB(n) = ((i1) << 4) | (i0); _CCL_CTRLC(n) = (i2); \
  _CCL_TRUTH(n) = (t); _CCL_CTRLA(n) |= CCL_ENABLE_bm;      \
  CCL.CTRLA |= _en; } while(0)

#define CCL_LUT0_init(i0,i1,i2,t) CCL_LUT_init(0,i0,i1,i2,t)
#define CCL_LUT1_init(i0,i1,i2,t) CCL_LUT_init(1,i0,i1,i2,t)
#define CCL_LUT2_init(i0,i1,i2,t) CCL_LUT_init(2,i0,i1,i2,t)
#define CCL_LUT3_init(i0,i1,i2,t) CCL_LUT_init(3,i0,i1,i2,t)

// Modify register (CCL must be disabled)
#define _CCL_MOD(reg, clr, set) do {                        \
  uint8_t _en = CCL.CTRLA & CCL_ENABLE_bm; CCL_disable();   \
  reg = (reg & ~(clr)) | (set); CCL.CTRLA |= _en; } while(0)

#define CCL_LUT_disable(n)    _CCL_MOD(_CCL_CTRLA(n), CCL_ENABLE_bm, 0)
#define CCL_LUT_FILTER(n,f)   _CCL_MOD(_CCL_CTRLA(n), CCL_FILTSEL_gm, ((f) & 3) << 4)
#define CCL_LUT_EDGE_on(n)    _CCL_MOD(_CCL_CTRLA(n), 0, CCL_EDGEDET_bm)
#define CCL_SEQ0_set(s)       _CCL_MOD(CCL.SEQCTRL0, 0x07, (s) & 7)
#define CCL_SEQ1_set(s)       _CCL_MOD(CCL.SEQCTRL1, 0x07, (s) & 7)

// LUT output pins: LUT0: PORTA, LUT1: PORTC, LUT2: PORTD, LUT3: PORTF; pin 3 or 6
#define CCL_LUT_PIN(n)        ((((n) == 0) ? PA0 : ((n) == 1) ? PC0 : ((n) == 2) ? PD0 : PF0) \
                              + ((CCL_ALT & (1 << (n))) ? 6 : 3))

#define CCL_LUT_OUT_enable(n) do {                          \
  PORTMUX.CCLROUTEA = (PORTMUX.CCLROUTEA & ~(1 << (n))) | (CCL_ALT & (1 << (n))); \
  PIN_output(CCL_LUT_PIN(n)); _CCL_MOD(_CCL_CTRLA(n), 0, CCL_OUTEN_bm); } while(0)
#define CCL_LUT_OUT_disable(n) _CCL_MOD(_CCL_CTRLA(n), CCL_OUTEN_bm, 0)

#ifdef __cplusplus
}
#endif
