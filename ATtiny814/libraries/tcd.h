// ===================================================================================
// PWM Functions using Timer/Counter D (TCD0) for tinyAVR 1-Series            * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// TCD_init()               init TCD0 (one-ramp mode) with parameters below and start
// TCD_start(ch)            enable PWM output on channel ch (sets pin as output)
// TCD_stop(ch)             disable PWM output on channel ch (pin is set LOW)
// TCD_write(ch,v)          set duty cycle of channel ch (0 .. TOP+1)
// TCD_setTOP(v)            set TOP value (period, max 4095)
// TCD_enable()             enable timer
// TCD_disable()            disable timer
//
// TCD_OVF_INT_enable()     enable overflow interrupt (TCD0_OVF_vect)
// TCD_OVF_INT_disable()    disable overflow interrupt
// TCD_OVF_FLAG_read()      read overflow interrupt flag
// TCD_OVF_FLAG_clear()     clear overflow interrupt flag
//
// TCD0 channel pin mapping:
// -------------------------
// Channel        0 (A)  1 (B)  2 (C=A)  3 (D=B)
// Pin            PA4    PA5    PC0      PC1     (C/D: 20-pin devices only)
// 8-pin device   PA6    PA7     -        -
//
// Notes:
// ------
// - TCD0 is only available on 1-series devices.
// - PWM frequency = F_TCD / SYNCPRES / CNTPRES / (TOP + 1).
// - With TCD_CLK = 0, TCD0 is clocked directly by the internal 16/20MHz oscillator
//   (independent of the main clock prescaler). The synchronizer clock
//   (F_TCD / SYNCPRES) must not exceed 8MHz (16MHz at VDD >= 2.7V).
// - Channel A is active at the beginning, channel B at the end of the PWM cycle.
// - Channel C outputs the same signal as A, channel D the same as B.
// - Enabling/disabling outputs restarts the timer (TCD0 must be disabled to change
//   the protected FAULTCTRL register).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// ===================================================================================
// TCD Parameters
// ===================================================================================
#define TCD_CLK           0             // clock (0: OSC20M, 2: EXTCLK, 3: CLK_PER)
#define TCD_SYNCPRES      2             // synchronizer prescaler (0:1, 1:2, 2:4, 3:8)
#define TCD_CNTPRES       0             // counter prescaler (0:1, 1:4, 2:32)
#define TCD_TOP           255           // TOP value (period, max 4095)

// ===================================================================================
// TCD Functions and Macros
// ===================================================================================
void TCD_init(void);                    // init and start TCD0
void TCD_start(uint8_t ch);             // enable PWM output on channel
void TCD_stop(uint8_t ch);              // disable PWM output on channel
void TCD_write(uint8_t ch, uint16_t v); // set duty cycle of channel
void TCD_setTOP(uint16_t v);            // set TOP value
void TCD_enable(void);                  // enable timer
void TCD_disable(void);                 // disable timer

#define TCD_OVF_INT_enable()  TCD0.INTCTRL  |=  TCD_OVF_bm
#define TCD_OVF_INT_disable() TCD0.INTCTRL  &= ~TCD_OVF_bm
#define TCD_OVF_FLAG_read()   (TCD0.INTFLAGS &  TCD_OVF_bm)
#define TCD_OVF_FLAG_clear()  TCD0.INTFLAGS  =  TCD_OVF_bm

#ifdef __cplusplus
};
#endif
