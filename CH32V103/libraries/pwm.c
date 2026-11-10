// ===================================================================================
// Basic PWM Functions using Timer TIM2 for CH32V103                          * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "pwm.h"

// Init timer for PWM (PWM mode 1, all channels, outputs disabled)
void PWM_init(void) {
  RCC->APB2PCENR |= RCC_IOPAEN;
  RCC->APB1PCENR |= RCC_TIM2EN;
  TIM2->PSC     = PWM_PRESC;                    // set prescaler
  TIM2->ATRLR   = PWM_TOP;                      // set TOP value
  TIM2->CHCTLR1 = (0b110 << 4) | (1 << 3)       // CH1: PWM mode 1, preload
                 | (0b110 << 12) | (1 << 11);   // CH2: PWM mode 1, preload
  TIM2->CHCTLR2 = (0b110 << 4) | (1 << 3)       // CH3: PWM mode 1, preload
                 | (0b110 << 12) | (1 << 11);   // CH4: PWM mode 1, preload
  TIM2->CCER    = 0;                            // all outputs disabled
  TIM2->SWEVGR  = TIM_UG;                       // load registers
  TIM2->CTLR1   = TIM_ARPE | TIM_CEN;           // auto-reload preload, start
}

// Start PWM output on channel ch (1..4)
void PWM_start(uint8_t ch) {
  switch(ch) {
    case 1:
      GPIOA->CFGLR = (GPIOA->CFGLR & ~((uint32_t)0b1111<<(0<<2))) | ((uint32_t)0b1011<<(0<<2));
      break;
    case 2:
      GPIOA->CFGLR = (GPIOA->CFGLR & ~((uint32_t)0b1111<<(1<<2))) | ((uint32_t)0b1011<<(1<<2));
      break;
    case 3:
      GPIOA->CFGLR = (GPIOA->CFGLR & ~((uint32_t)0b1111<<(2<<2))) | ((uint32_t)0b1011<<(2<<2));
      break;
    case 4:
      GPIOA->CFGLR = (GPIOA->CFGLR & ~((uint32_t)0b1111<<(3<<2))) | ((uint32_t)0b1011<<(3<<2));
      break;
    default: break;
  }
  if(ch && (ch <= 4)) TIM2->CCER |= (uint16_t)1 << ((ch - 1) << 2); // enable output
}

// Stop PWM output on channel ch (1..4), pin becomes LOW output
void PWM_stop(uint8_t ch) {
  if(ch && (ch <= 4)) TIM2->CCER &= ~((uint16_t)1 << ((ch - 1) << 2)); // disable output
  switch(ch) {
    case 1:
      GPIOA->BCR   = (uint32_t)1<<0;
      GPIOA->CFGLR = (GPIOA->CFGLR & ~((uint32_t)0b1111<<(0<<2))) | ((uint32_t)0b0011<<(0<<2));
      break;
    case 2:
      GPIOA->BCR   = (uint32_t)1<<1;
      GPIOA->CFGLR = (GPIOA->CFGLR & ~((uint32_t)0b1111<<(1<<2))) | ((uint32_t)0b0011<<(1<<2));
      break;
    case 3:
      GPIOA->BCR   = (uint32_t)1<<2;
      GPIOA->CFGLR = (GPIOA->CFGLR & ~((uint32_t)0b1111<<(2<<2))) | ((uint32_t)0b0011<<(2<<2));
      break;
    case 4:
      GPIOA->BCR   = (uint32_t)1<<3;
      GPIOA->CFGLR = (GPIOA->CFGLR & ~((uint32_t)0b1111<<(3<<2))) | ((uint32_t)0b0011<<(3<<2));
      break;
    default: break;
  }
}

// Set duty cycle of channel ch (0 .. PWM_TOP + 1)
void PWM_write(uint8_t ch, uint16_t val) {
  switch(ch) {
    case 1: TIM2->CH1CVR = val; break;
    case 2: TIM2->CH2CVR = val; break;
    case 3: TIM2->CH3CVR = val; break;
    case 4: TIM2->CH4CVR = val; break;
    default: break;
  }
}
