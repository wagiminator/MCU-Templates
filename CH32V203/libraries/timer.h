// ===================================================================================
// Basic Timer Functions using TIM3 for CH32V203                              * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// TIM_init()               init timer TIM3 (counts microseconds)
// TIM_start(n)             start periodic interrupt every n microseconds (1..65536)
// TIM_stop()               stop periodic interrupt
// TIM_read()               read timer counter value (microseconds within the period)
// TIM_reset()              reset timer counter to zero
// TIM_ISR { }              timer interrupt service routine (called periodically)
//
// Notes:
// ------
// - Timer TIM3 is used with a prescaler for 1MHz (F_CPU must be a multiple of 1MHz).
// - The interrupt flag is cleared automatically, TIM_ISR is an ordinary function.
// - The interrupt vector table must be enabled (SYS_USE_VECTORS = 1 in system.h).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// Interrupt enable check
#if SYS_USE_VECTORS == 0
  #error Interrupt vector table must be enabled (SYS_USE_VECTORS in system.h)!
#endif
#if (F_CPU % 1000000) || (F_CPU / 1000000 > 65536)
  #error F_CPU must be a multiple of 1MHz for the timer library
#endif

void TIM_init(void);                // init timer
void TIM_start(uint32_t us);        // start periodic interrupt
void TIM_stop(void);                // stop periodic interrupt
#define TIM_read()        (TIM3->CNT)        // read counter value
#define TIM_reset()       (TIM3->CNT = 0)    // reset counter to zero

#define TIM_ISR   void TIM_callback(void)

#ifdef __cplusplus
};
#endif
