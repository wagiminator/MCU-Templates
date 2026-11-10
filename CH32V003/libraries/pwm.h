// ===================================================================================
// Basic PWM Functions using Timer TIM2 for CH32V003                          * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// PWM_init()               init TIM2 for PWM with frequency and resolution set below
// PWM_start(ch)            start PWM output on channel ch (1..4, see below)
// PWM_stop(ch)             stop PWM output on channel ch (pin becomes LOW output)
// PWM_write(ch, val)       set PWM duty cycle of channel ch (0 .. PWM_TOP+1 = 0% .. 100%)
// PWM_setTOP(n)            set PWM TOP value (period = n + 1 timer clock cycles)
// PWM_setPRESC(n)          set PWM clock prescaler (timer clock = F_CPU / (n + 1))
// PWM_enable()             enable TIM2 (start counter)
// PWM_disable()            disable TIM2 (stop counter)
//
// PWM channels and pins:
// ----------------------
// TIM2_CH1  PD4
// TIM2_CH2  PD3
// TIM2_CH3  PC0
// TIM2_CH4  PD7
//
// Notes:
// ------
// - PWM frequency = F_CPU / (PWM_PRESC + 1) / (PWM_TOP + 1), e.g. with default values
//   F_CPU / 1 / 256. All channels have the same frequency, but individual duty cycles.
// - PD7 is the RESET pin by default (see flash library to make it a GPIO).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// PWM parameters
#define PWM_PRESC         0             // timer clock prescaler (0..65535)
#define PWM_TOP           255           // TOP value (resolution - 1, 1..65535)

// PWM functions
void PWM_init(void);                    // init timer for PWM
void PWM_start(uint8_t ch);             // start PWM output on channel ch (1..4)
void PWM_stop(uint8_t ch);              // stop PWM output on channel ch (1..4)
void PWM_write(uint8_t ch, uint16_t val); // set duty cycle of channel ch

#define PWM_setTOP(n)     (TIM2->ATRLR = (n))
#define PWM_setPRESC(n)   (TIM2->PSC = (n))
#define PWM_enable()      (TIM2->CTLR1 |=  TIM_CEN)
#define PWM_disable()     (TIM2->CTLR1 &= ~TIM_CEN)

#ifdef __cplusplus
};
#endif
