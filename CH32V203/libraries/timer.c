// ===================================================================================
// Basic Timer Functions using TIM3 for CH32V203                              * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "timer.h"

void TIM_callback(void);                  // user function (TIM_ISR)

// Init timer (1MHz counter clock, free running)
void TIM_init(void) {
  RCC->APB1PCENR |= RCC_TIM3EN;
  TIM3->PSC     = (F_CPU / 1000000) - 1;        // prescaler: 1MHz
  TIM3->ATRLR   = 0xFFFF;                       // free running
  TIM3->SWEVGR  = TIM_UG;                       // load registers
  TIM3->INTFR   = 0;                            // clear flags
  TIM3->CTLR1   = TIM_CEN;                      // start counter
}

// Start periodic interrupt every n microseconds (1..65536)
void TIM_start(uint32_t us) {
  if(!us) us = 1;
  if(us > 65536) us = 65536;
  TIM3->CTLR1   = 0;                            // stop counter
  TIM3->ATRLR   = us - 1;                       // set period
  TIM3->CNT     = 0;                            // reset counter
  TIM3->INTFR   = 0;                            // clear flags
  TIM3->DMAINTENR = TIM_UIE;                    // enable update interrupt
  NVIC_EnableIRQ(TIM3_IRQn);
  TIM3->CTLR1   = TIM_ARPE | TIM_CEN;           // start counter
}

// Stop periodic interrupt
void TIM_stop(void) {
  NVIC_DisableIRQ(TIM3_IRQn);
  TIM3->DMAINTENR = 0;                          // disable interrupt
  TIM3->INTFR   = 0;                            // clear flags
}

// Interrupt service routine
void TIM3_IRQHandler(void) __attribute__((interrupt));
void TIM3_IRQHandler(void) {
  TIM3->INTFR   = (uint16_t)~TIM_UIF;           // clear interrupt flag
  TIM_callback();                         // call user function
}
