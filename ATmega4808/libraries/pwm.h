// ===================================================================================
// PWM Functions using Timer/Counter A (TCA0) for megaAVR 0-Series            * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// PWM_init()               init TCA0 for PWM with parameters below and start timer
// PWM_start(ch)            enable PWM output on channel ch (sets pin as output)
// PWM_stop(ch)             disable PWM output on channel ch (pin is set LOW)
// PWM_write(ch,v)          set duty cycle of channel ch (0 .. PWM_TOP)
// PWM_setTOP(v)            set TOP value (period), PWM frequency = F_CPU/PRESC/(TOP+1)
// PWM_setPRESC(n)          set prescaler (0:1, 1:2, 2:4, 3:8, 4:16, 5:64, 6:256, 7:1024)
// PWM_enable()             enable timer
// PWM_disable()            disable timer (outputs keep their current state)
//
// PWM_OVF_INT_enable()     enable overflow interrupt (TCA0_OVF_vect / TCA0_LUNF_vect)
// PWM_OVF_INT_disable()    disable overflow interrupt
// PWM_OVF_FLAG_read()      read overflow interrupt flag
// PWM_OVF_FLAG_clear()     clear overflow interrupt flag
//
// PWM modes (set below in PWM parameters):
// ----------------------------------------
// PWM_MODE 0: single mode, 16-bit, 3 channels (0..2), PWM_TOP up to 65535
// PWM_MODE 1: split mode,   8-bit, 6 channels (0..5), PWM_TOP up to 255
//
// PWM channel pin mapping (port selected by PWM_PORT, channel n = pin n of the port):
// ------------------------------------------------------------------------------------
// PWM_PORT       0     1     2     3     4     5
// Port         PORTA PORTB PORTC PORTD PORTE PORTF
// Channel 0..5 PA0-5 PB0-5 PC0-5 PD0-5 PE0-3 PF0-5
//
// Notes:
// ------
// - Channels 3..5 are only available in split mode.
// - PORTB, PORTE and PC4/PC5 are only available on 40/48-pin devices, PF2..PF5 not
//   on 28-pin devices.
// - TCA0 is not available for other purposes when this library is used.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// ===================================================================================
// PWM Parameters
// ===================================================================================
#define PWM_MODE          0             // 0: single mode (16-bit, 3 ch), 1: split mode (8-bit, 6 ch)
#define PWM_PRESC         0             // prescaler (0:1, 1:2, 2:4, 3:8, 4:16, 5:64, 6:256, 7:1024)
#define PWM_TOP           255           // TOP value (period), freq = F_CPU/PRESC/(TOP+1)
#define PWM_PORT          0             // port of the PWM pins (0:A, 1:B, 2:C, 3:D, 4:E, 5:F)

// ===================================================================================
// PWM Functions and Macros
// ===================================================================================
void PWM_init(void);                    // init and start PWM timer
void PWM_start(uint8_t ch);             // enable PWM output on channel
void PWM_stop(uint8_t ch);              // disable PWM output on channel
void PWM_write(uint8_t ch, uint16_t v); // set duty cycle of channel

#define PWM_enable()          TCA0.SINGLE.CTRLA |=  TCA_SINGLE_ENABLE_bm
#define PWM_disable()         TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm
#define PWM_setPRESC(n)       TCA0.SINGLE.CTRLA = (TCA0.SINGLE.CTRLA & ~0x0e) | (((n) & 7) << 1)

#if PWM_MODE == 0
  #define PWM_setTOP(v)       TCA0.SINGLE.PERBUF = (v)
  #define PWM_OVF_INT_enable()  TCA0.SINGLE.INTCTRL  |=  TCA_SINGLE_OVF_bm
  #define PWM_OVF_INT_disable() TCA0.SINGLE.INTCTRL  &= ~TCA_SINGLE_OVF_bm
  #define PWM_OVF_FLAG_read()   (TCA0.SINGLE.INTFLAGS &  TCA_SINGLE_OVF_bm)
  #define PWM_OVF_FLAG_clear()  TCA0.SINGLE.INTFLAGS  =  TCA_SINGLE_OVF_bm
#else
  #define PWM_setTOP(v)       do {TCA0.SPLIT.LPER = (v); TCA0.SPLIT.HPER = (v);} while(0)
  #define PWM_OVF_INT_enable()  TCA0.SPLIT.INTCTRL   |=  TCA_SPLIT_LUNF_bm
  #define PWM_OVF_INT_disable() TCA0.SPLIT.INTCTRL   &= ~TCA_SPLIT_LUNF_bm
  #define PWM_OVF_FLAG_read()   (TCA0.SPLIT.INTFLAGS  &  TCA_SPLIT_LUNF_bm)
  #define PWM_OVF_FLAG_clear()  TCA0.SPLIT.INTFLAGS   =  TCA_SPLIT_LUNF_bm
#endif

#ifdef __cplusplus
}
#endif
