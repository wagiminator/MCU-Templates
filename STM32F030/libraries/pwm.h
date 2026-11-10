// ===================================================================================
// Basic PWM Functions using Timer TIM3 for STM32F03x                         * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// PWM_init()               init TIM3 for PWM with frequency and resolution set below
// PWM_start(ch)            start PWM output on channel ch (1..4, see below)
// PWM_stop(ch)             stop PWM output on channel ch (pin becomes LOW output)
// PWM_write(ch, val)       set PWM duty cycle of channel ch (0 .. PWM_TOP+1 = 0% .. 100%)
// PWM_setTOP(n)            set PWM TOP value (period = n + 1 timer clock cycles)
// PWM_setPRESC(n)          set PWM clock prescaler (timer clock = F_CPU / (n + 1))
// PWM_enable()             enable TIM3 (start counter)
// PWM_disable()            disable TIM3 (stop counter)
//
// PWM channels and pins:
// ----------------------
// TIM3_CH1  PA6
// TIM3_CH2  PA7
// TIM3_CH3  PB0
// TIM3_CH4  PB1
//
// Notes:
// ------
// - PWM frequency = F_CPU / (PWM_PRESC + 1) / (PWM_TOP + 1), e.g. with default values
//   F_CPU / 1 / 256. All channels have the same frequency, but individual duty cycles.
// - TIM3 is not available on all packages (pins PB0/PB1).
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

#define PWM_setTOP(n)     (TIM3->ARR = (n))
#define PWM_setPRESC(n)   (TIM3->PSC = (n))
#define PWM_enable()      (TIM3->CR1 |=  TIM_CR1_CEN)
#define PWM_disable()     (TIM3->CR1 &= ~TIM_CR1_CEN)

#ifdef __cplusplus
};
#endif
