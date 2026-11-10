// ===================================================================================
// Basic Timer Functions for RP2040 (TIMER alarm 0)                           * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "timer.h"

static uint32_t TIM_period;               // period in microseconds
void TIM_callback(void);                  // user function

// Init timer
void TIM_init(void) {
  RESET_release(RESETS_RESET_TIMER);      // release TIMER from reset (if not done)
}

// Read 64-bit timer value
uint64_t TIM_read(void) {
  uint32_t hi, lo;
  do {
    hi = TIMER->TIMERAWH;
    lo = TIMER->TIMERAWL;
  } while(hi != TIMER->TIMERAWH);
  return ((uint64_t)hi << 32) | lo;
}

// Reset counter to zero (write low word first, high word latches)
void TIM_reset(void) {
  TIMER->TIMELW = 0;
  TIMER->TIMEHW = 0;
}

// Start periodic interrupt every n microseconds
void TIM_start(uint32_t us) {
  TIM_period = us;
  TIMER->INTR = TIMER_INTR_ALARM_0;       // clear interrupt flag
  REG_SET(TIMER->INTE, TIMER_INTE_ALARM_0);
  NVIC_EnableIRQ(TIMER_IRQ_0_IRQn);
  TIMER->ALARM[0] = TIMER->TIMERAWL + us; // arm alarm
}

// Stop periodic interrupt
void TIM_stop(void) {
  NVIC_DisableIRQ(TIMER_IRQ_0_IRQn);
  REG_CLR(TIMER->INTE, TIMER_INTE_ALARM_0);
  TIMER->ARMED = 1;                       // disarm alarm 0
  TIMER->INTR  = TIMER_INTR_ALARM_0;
}

// Interrupt service routine
void TIMER_IRQ_0_IRQHandler(void) {
  uint32_t next = TIMER->ALARM[0] + TIM_period;
  TIMER->INTR = TIMER_INTR_ALARM_0;       // clear interrupt flag
  if((int32_t)(next - TIMER->TIMERAWL) <= 0) next = TIMER->TIMERAWL + TIM_period;
  TIMER->ALARM[0] = next;                 // next alarm (exact period, no lost alarm)
  TIM_callback();                         // call user function
}
