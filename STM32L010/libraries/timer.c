// ===================================================================================
// Basic Timer Functions using TIM21 for STM32L0xx                            * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "timer.h"

void TIM_callback(void);                  // user function (TIM_ISR)

// Init timer (1MHz counter clock, free running)
void TIM_init(void) {
  RCC->APB2ENR  |= RCC_APB2ENR_TIM21EN;
  TIM21->PSC    = (F_CPU / 1000000) - 1;        // prescaler: 1MHz
  TIM21->ARR    = 0xFFFF;                       // free running
  TIM21->EGR    = TIM_EGR_UG;                   // load registers
  TIM21->SR     = 0;                            // clear flags
  TIM21->CR1    = TIM_CR1_CEN;                  // start counter
}

// Start periodic interrupt every n microseconds (1..65536)
void TIM_start(uint32_t us) {
  if(!us) us = 1;
  if(us > 65536) us = 65536;
  TIM21->CR1    = 0;                            // stop counter
  TIM21->ARR    = us - 1;                       // set period
  TIM21->CNT    = 0;                            // reset counter
  TIM21->SR     = 0;                            // clear flags
  TIM21->DIER   = TIM_DIER_UIE;                 // enable update interrupt
  NVIC_EnableIRQ(TIM21_IRQn);
  TIM21->CR1    = TIM_CR1_ARPE | TIM_CR1_CEN;   // start counter
}

// Stop periodic interrupt
void TIM_stop(void) {
  NVIC_DisableIRQ(TIM21_IRQn);
  TIM21->DIER   = 0;                            // disable interrupt
  TIM21->SR     = 0;                            // clear flags
}

// Interrupt service routine
void TIM21_IRQHandler(void);
void TIM21_IRQHandler(void) {
  TIM21->SR     = (uint16_t)~TIM_SR_UIF;        // clear interrupt flag
  TIM_callback();                         // call user function
}
