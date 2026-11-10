// ===================================================================================
// Basic Timer Functions using TIM14 for STM32G0xx                            * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "timer.h"

void TIM_callback(void);                  // user function (TIM_ISR)

// Init timer (1MHz counter clock, free running)
void TIM_init(void) {
  RCC->APBENR2  |= RCC_APBENR2_TIM14EN;
  TIM14->PSC    = (F_CPU / 1000000) - 1;        // prescaler: 1MHz
  TIM14->ARR    = 0xFFFF;                       // free running
  TIM14->EGR    = TIM_EGR_UG;                   // load registers
  TIM14->SR     = 0;                            // clear flags
  TIM14->CR1    = TIM_CR1_CEN;                  // start counter
}

// Start periodic interrupt every n microseconds (1..65536)
void TIM_start(uint32_t us) {
  if(!us) us = 1;
  if(us > 65536) us = 65536;
  TIM14->CR1    = 0;                            // stop counter
  TIM14->ARR    = us - 1;                       // set period
  TIM14->CNT    = 0;                            // reset counter
  TIM14->SR     = 0;                            // clear flags
  TIM14->DIER   = TIM_DIER_UIE;                 // enable update interrupt
  NVIC_EnableIRQ(TIM14_IRQn);
  TIM14->CR1    = TIM_CR1_ARPE | TIM_CR1_CEN;   // start counter
}

// Stop periodic interrupt
void TIM_stop(void) {
  NVIC_DisableIRQ(TIM14_IRQn);
  TIM14->DIER   = 0;                            // disable interrupt
  TIM14->SR     = 0;                            // clear flags
}

// Interrupt service routine
void TIM14_IRQHandler(void);
void TIM14_IRQHandler(void) {
  TIM14->SR     = (uint16_t)~TIM_SR_UIF;        // clear interrupt flag
  TIM_callback();                         // call user function
}
