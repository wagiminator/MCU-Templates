// ===================================================================================
// Basic PWM Functions for RP2040 (PWM slices 0..7)                          * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "pwm.h"

// Init all PWM slices (free-running, duty cycle 0%) and start counters
void PWM_init(void) {
  uint8_t i;
  RESET_cycle(RESETS_RESET_PWM);                  // reset and release PWM
  for(i = 0; i < 8; i++) {
    PWM->CH[i].DIV = (uint32_t)PWM_PRESC << PWM_CH_DIV_INT_Pos;
    PWM->CH[i].TOP = PWM_TOP;
    PWM->CH[i].CC  = 0;
    PWM->CH[i].CSR = PWM_CH_CSR_EN;
  }
}

// Set TOP value of all slices (period = top + 1 clock cycles)
void PWM_setTOP(uint16_t top) {
  uint8_t i;
  for(i = 0; i < 8; i++) PWM->CH[i].TOP = top;
}

// Set clock prescaler of all slices (1..255)
void PWM_setPRESC(uint8_t presc) {
  uint8_t i;
  for(i = 0; i < 8; i++) PWM->CH[i].DIV = (uint32_t)presc << PWM_CH_DIV_INT_Pos;
}
