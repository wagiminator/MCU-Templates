// ===================================================================================
// Basic Timer Functions using TIM1 for CH32V003                              * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "timer.h"

void TIM_callback(void);                  // user function (TIM_ISR)

// Init timer (1MHz counter clock, free running)
void TIM_init(void) {
  RCC->APB2PCENR |= RCC_TIM1EN;
  TIM1->PSC     = (F_CPU / 1000000) - 1;        // prescaler: 1MHz
  TIM1->ATRLR   = 0xFFFF;                       // free running
  TIM1->SWEVGR  = TIM_UG;                       // load registers
  TIM1->INTFR   = 0;                            // clear flags
  TIM1->CTLR1   = TIM_CEN;                      // start counter
}

// Start periodic interrupt every n microseconds (1..65536)
void TIM_start(uint32_t us) {
  if(!us) us = 1;
  if(us > 65536) us = 65536;
  TIM1->CTLR1   = 0;                            // stop counter
  TIM1->ATRLR   = us - 1;                       // set period
  TIM1->CNT     = 0;                            // reset counter
  TIM1->INTFR   = 0;                            // clear flags
  TIM1->DMAINTENR = TIM_UIE;                    // enable update interrupt
  NVIC_EnableIRQ(TIM1_UP_IRQn);
  TIM1->CTLR1   = TIM_ARPE | TIM_CEN;           // start counter
}

// Stop periodic interrupt
void TIM_stop(void) {
  NVIC_DisableIRQ(TIM1_UP_IRQn);
  TIM1->DMAINTENR = 0;                          // disable interrupt
  TIM1->INTFR   = 0;                            // clear flags
}

// Interrupt service routine
void TIM1_UP_IRQHandler(void) __attribute__((interrupt));
void TIM1_UP_IRQHandler(void) {
  TIM1->INTFR   = (uint16_t)~TIM_UIF;           // clear interrupt flag
  TIM_callback();                         // call user function
}
