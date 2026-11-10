// ===================================================================================
// Basic Timer Functions for RP2040 (TIMER alarm 0)                           * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// TIM_init()               init timer (counts microseconds, 64-bit)
// TIM_start(n)             start periodic interrupt every n microseconds
// TIM_stop()               stop periodic interrupt
// TIM_read()               read timer counter value in microseconds (64-bit)
// TIM_reset()              reset timer counter to zero
// TIM_ISR { }              timer interrupt service routine (called periodically)
//
// Notes:
// ------
// - The 64-bit microsecond counter of the TIMER peripheral and alarm 0 are used
//   (interrupt TIMER_IRQ_0). The period is kept exact by advancing the alarm time.
// - The interrupt flag is cleared automatically, TIM_ISR is an ordinary function.
// - TIM_reset() changes the time base of the TIMER, which is also used by the millis
//   library and SLEEP_ms().
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

void TIM_init(void);                // init timer
void TIM_start(uint32_t us);        // start periodic interrupt
void TIM_stop(void);                // stop periodic interrupt
void TIM_reset(void);               // reset counter to zero
uint64_t TIM_read(void);            // read counter value

#define TIM_ISR   void TIM_callback(void)

#ifdef __cplusplus
};
#endif
