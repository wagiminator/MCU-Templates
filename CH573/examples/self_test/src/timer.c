// ===================================================================================
// Basic Timer Functions (TMR0..TMR3, 26-bit) for CH573/CH571                 * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "timer.h"

// Start PWM output on TMR0 pin (period n ticks, high time d ticks)
void TMR0_PWM_start(uint32_t n, uint32_t d) {
  #if TMR0_REMAP == 0
  R16_PIN_ALTERNATE &= ~RB_PIN_TMR0;
  PIN_output(PA9);
  #else
  R16_PIN_ALTERNATE |=  RB_PIN_TMR0;
  PIN_output(PB23);
  #endif
  R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
  R32_TMR0_CNT_END = n;
  R32_TMR0_FIFO    = d;
  R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN | RB_TMR_OUT_EN;   // PWM, default low, high active
}

// Start PWM output on TMR1 pin (period n ticks, high time d ticks)
void TMR1_PWM_start(uint32_t n, uint32_t d) {
  #if TMR1_REMAP == 0
  R16_PIN_ALTERNATE &= ~RB_PIN_TMR1;
  PIN_output(PA10);
  #else
  R16_PIN_ALTERNATE |=  RB_PIN_TMR1;
  PIN_output(PB10);
  #endif
  R8_TMR1_CTRL_MOD = RB_TMR_ALL_CLEAR;
  R32_TMR1_CNT_END = n;
  R32_TMR1_FIFO    = d;
  R8_TMR1_CTRL_MOD = RB_TMR_COUNT_EN | RB_TMR_OUT_EN;   // PWM, default low, high active
}

// Start PWM output on TMR2 pin (period n ticks, high time d ticks)
void TMR2_PWM_start(uint32_t n, uint32_t d) {
  #if TMR2_REMAP == 0
  R16_PIN_ALTERNATE &= ~RB_PIN_TMR2;
  PIN_output(PA11);
  #else
  R16_PIN_ALTERNATE |=  RB_PIN_TMR2;
  PIN_output(PB11);
  #endif
  R8_TMR2_CTRL_MOD = RB_TMR_ALL_CLEAR;
  R32_TMR2_CNT_END = n;
  R32_TMR2_FIFO    = d;
  R8_TMR2_CTRL_MOD = RB_TMR_COUNT_EN | RB_TMR_OUT_EN;   // PWM, default low, high active
}

// Start PWM output on TMR3 pin (period n ticks, high time d ticks)
void TMR3_PWM_start(uint32_t n, uint32_t d) {
  PIN_output(PB22);
  R8_TMR3_CTRL_MOD = RB_TMR_ALL_CLEAR;
  R32_TMR3_CNT_END = n;
  R32_TMR3_FIFO    = d;
  R8_TMR3_CTRL_MOD = RB_TMR_COUNT_EN | RB_TMR_OUT_EN;   // PWM, default low, high active
}
