// ===================================================================================
// Basic PWM Functions using Timer TIM1 for STM32C0xx                         * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "pwm.h"

// Init timer for PWM (PWM mode 1, all channels, outputs disabled)
void PWM_init(void) {
  RCC->IOPENR   |= RCC_IOPENR_GPIOAEN;
  RCC->APBENR2  |= RCC_APBENR2_TIM1EN;
  TIM1->PSC     = PWM_PRESC;                    // set prescaler
  TIM1->ARR     = PWM_TOP;                      // set TOP value
  TIM1->CCMR1   = (0b110 << 4) | (1 << 3)       // CH1: PWM mode 1, preload
                 | (0b110 << 12) | (1 << 11);   // CH2: PWM mode 1, preload
  TIM1->CCMR2   = (0b110 << 4) | (1 << 3)       // CH3: PWM mode 1, preload
                 | (0b110 << 12) | (1 << 11);   // CH4: PWM mode 1, preload
  TIM1->CCER    = 0;                            // all outputs disabled
  TIM1->BDTR    = TIM_BDTR_MOE;                 // main output enable
  TIM1->EGR     = TIM_EGR_UG;                   // load registers
  TIM1->CR1     = TIM_CR1_ARPE | TIM_CR1_CEN;   // auto-reload preload, start
}

// Start PWM output on channel ch (1..4)
void PWM_start(uint8_t ch) {
  switch(ch) {
    case 1:
      GPIOA->MODER   = (GPIOA->MODER & ~((uint32_t)0b11<<(0<<1))) | ((uint32_t)0b10<<(0<<1));
      GPIOA->AFR[0]  = (GPIOA->AFR[0] & ~((uint32_t)0xf<<(0<<2))) | ((uint32_t)5<<(0<<2));
      break;
    case 2:
      GPIOA->MODER   = (GPIOA->MODER & ~((uint32_t)0b11<<(1<<1))) | ((uint32_t)0b10<<(1<<1));
      GPIOA->AFR[0]  = (GPIOA->AFR[0] & ~((uint32_t)0xf<<(1<<2))) | ((uint32_t)5<<(1<<2));
      break;
    case 3:
      GPIOA->MODER   = (GPIOA->MODER & ~((uint32_t)0b11<<(2<<1))) | ((uint32_t)0b10<<(2<<1));
      GPIOA->AFR[0]  = (GPIOA->AFR[0] & ~((uint32_t)0xf<<(2<<2))) | ((uint32_t)5<<(2<<2));
      break;
    case 4:
      GPIOA->MODER   = (GPIOA->MODER & ~((uint32_t)0b11<<(3<<1))) | ((uint32_t)0b10<<(3<<1));
      GPIOA->AFR[0]  = (GPIOA->AFR[0] & ~((uint32_t)0xf<<(3<<2))) | ((uint32_t)5<<(3<<2));
      break;
    default: break;
  }
  if(ch && (ch <= 4)) TIM1->CCER |= (uint16_t)1 << ((ch - 1) << 2); // enable output
}

// Stop PWM output on channel ch (1..4), pin becomes LOW output
void PWM_stop(uint8_t ch) {
  if(ch && (ch <= 4)) TIM1->CCER &= ~((uint16_t)1 << ((ch - 1) << 2)); // disable output
  switch(ch) {
    case 1:
      GPIOA->BRR     = (uint32_t)1<<0;
      GPIOA->MODER   = (GPIOA->MODER & ~((uint32_t)0b11<<(0<<1))) | ((uint32_t)0b01<<(0<<1));
      break;
    case 2:
      GPIOA->BRR     = (uint32_t)1<<1;
      GPIOA->MODER   = (GPIOA->MODER & ~((uint32_t)0b11<<(1<<1))) | ((uint32_t)0b01<<(1<<1));
      break;
    case 3:
      GPIOA->BRR     = (uint32_t)1<<2;
      GPIOA->MODER   = (GPIOA->MODER & ~((uint32_t)0b11<<(2<<1))) | ((uint32_t)0b01<<(2<<1));
      break;
    case 4:
      GPIOA->BRR     = (uint32_t)1<<3;
      GPIOA->MODER   = (GPIOA->MODER & ~((uint32_t)0b11<<(3<<1))) | ((uint32_t)0b01<<(3<<1));
      break;
    default: break;
  }
}

// Set duty cycle of channel ch (0 .. PWM_TOP + 1)
void PWM_write(uint8_t ch, uint16_t val) {
  switch(ch) {
    case 1: TIM1->CCR1 = val; break;
    case 2: TIM1->CCR2 = val; break;
    case 3: TIM1->CCR3 = val; break;
    case 4: TIM1->CCR4 = val; break;
    default: break;
  }
}
