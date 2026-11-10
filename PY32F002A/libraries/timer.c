// ===================================================================================
// Basic Timer Functions using TIM16 for PY32F0xx                             * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "timer.h"

void TIM_callback(void);                  // user function (TIM_ISR)

// Init timer (1MHz counter clock, free running)
void TIM_init(void) {
  RCC->APBENR2  |= RCC_APBENR2_TIM16EN;
  TIM16->PSC    = (F_CPU / 1000000) - 1;        // prescaler: 1MHz
  TIM16->ARR    = 0xFFFF;                       // free running
  TIM16->EGR    = TIM_EGR_UG;                   // load registers
  TIM16->SR     = 0;                            // clear flags
  TIM16->CR1    = TIM_CR1_CEN;                  // start counter
}

// Start periodic interrupt every n microseconds (1..65536)
void TIM_start(uint32_t us) {
  if(!us) us = 1;
  if(us > 65536) us = 65536;
  TIM16->CR1    = 0;                            // stop counter
  TIM16->ARR    = us - 1;                       // set period
  TIM16->CNT    = 0;                            // reset counter
  TIM16->SR     = 0;                            // clear flags
  TIM16->DIER   = TIM_DIER_UIE;                 // enable update interrupt
  NVIC_EnableIRQ(TIM16_IRQn);
  TIM16->CR1    = TIM_CR1_ARPE | TIM_CR1_CEN;   // start counter
}

// Stop periodic interrupt
void TIM_stop(void) {
  NVIC_DisableIRQ(TIM16_IRQn);
  TIM16->DIER   = 0;                            // disable interrupt
  TIM16->SR     = 0;                            // clear flags
}

// Interrupt service routine
void TIM16_IRQHandler(void);
void TIM16_IRQHandler(void) {
  TIM16->SR     = (uint16_t)~TIM_SR_UIF;        // clear interrupt flag
  TIM_callback();                         // call user function
}
