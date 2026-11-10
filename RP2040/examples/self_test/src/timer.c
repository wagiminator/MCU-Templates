// ===================================================================================
// Basic Timer Functions for RP2350 (TIMER0 alarm 0)                          * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "timer.h"

static uint32_t TIM_period;               // period in microseconds
void TIM_callback(void);                  // user function

// Init timer
void TIM_init(void) {
  RESET_release(RESETS_RESET_TIMER0);      // release TIMER0 from reset (if not done)
}

// Read 64-bit timer value
uint64_t TIM_read(void) {
  uint32_t hi, lo;
  do {
    hi = TIMER0->TIMERAWH;
    lo = TIMER0->TIMERAWL;
  } while(hi != TIMER0->TIMERAWH);
  return ((uint64_t)hi << 32) | lo;
}

// Reset counter to zero (write low word first, high word latches)
void TIM_reset(void) {
  TIMER0->TIMELW = 0;
  TIMER0->TIMEHW = 0;
}

// Start periodic interrupt every n microseconds
void TIM_start(uint32_t us) {
  TIM_period = us;
  TIMER0->INTR = TIMER_INTR_ALARM_0;       // clear interrupt flag
  REG_SET(TIMER0->INTE, TIMER_INTE_ALARM_0);
  NVIC_EnableIRQ(TIMER0_IRQ_0_IRQn);
  TIMER0->ALARM[0] = TIMER0->TIMERAWL + us; // arm alarm
}

// Stop periodic interrupt
void TIM_stop(void) {
  NVIC_DisableIRQ(TIMER0_IRQ_0_IRQn);
  REG_CLR(TIMER0->INTE, TIMER_INTE_ALARM_0);
  TIMER0->ARMED = 1;                       // disarm alarm 0
  TIMER0->INTR  = TIMER_INTR_ALARM_0;
}

// Interrupt service routine
void TIMER0_IRQ_0_IRQHandler(void) {
  uint32_t next = TIMER0->ALARM[0] + TIM_period;
  TIMER0->INTR = TIMER_INTR_ALARM_0;       // clear interrupt flag
  if((int32_t)(next - TIMER0->TIMERAWL) <= 0) next = TIMER0->TIMERAWL + TIM_period;
  TIMER0->ALARM[0] = next;                 // next alarm (exact period, no lost alarm)
  TIM_callback();                         // call user function
}
