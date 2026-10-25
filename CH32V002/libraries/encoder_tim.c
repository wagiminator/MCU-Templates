// ===================================================================================
// Basic Rotary Encoder Functions using Timer                                 * v1.0 *
// for CH32V002, V004, V005, V006, V007, M007
// ===================================================================================
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#include "encoder_tim.h"

// Pin assignment for the selected mappings (CH32V00X RM: AFIO_PCFR1 TIM1_RM / TIM2_RM)
#if   ENC1_MAP == 0                   // A: PD2, B: PA1
  #define ENC1_A_GPIO GPIOD
  #define ENC1_A_PIN  2
  #define ENC1_B_GPIO GPIOA
  #define ENC1_B_PIN  1
  #define ENC1_RM     0
  #define ENC1_PORT_EN (RCC_IOPAEN | RCC_IOPDEN)
#elif ENC1_MAP == 1                   // A: PC6, B: PC7
  #define ENC1_A_GPIO GPIOC
  #define ENC1_A_PIN  6
  #define ENC1_B_GPIO GPIOC
  #define ENC1_B_PIN  7
  #define ENC1_RM     2
  #define ENC1_PORT_EN (RCC_IOPCEN)
#elif ENC1_MAP == 2                   // A: PC4, B: PC7
  #define ENC1_A_GPIO GPIOC
  #define ENC1_A_PIN  4
  #define ENC1_B_GPIO GPIOC
  #define ENC1_B_PIN  7
  #define ENC1_RM     3
  #define ENC1_PORT_EN (RCC_IOPCEN)
#elif ENC1_MAP == 3                   // A: PA3, B: PB0
  #define ENC1_A_GPIO GPIOA
  #define ENC1_A_PIN  3
  #define ENC1_B_GPIO GPIOB
  #define ENC1_B_PIN  0
  #define ENC1_RM     4
  #define ENC1_PORT_EN (RCC_IOPAEN | RCC_IOPBEN)
#elif ENC1_MAP == 4                   // A: PC4, B: PC5
  #define ENC1_A_GPIO GPIOC
  #define ENC1_A_PIN  4
  #define ENC1_B_GPIO GPIOC
  #define ENC1_B_PIN  5
  #define ENC1_RM     7
  #define ENC1_PORT_EN (RCC_IOPCEN)
#elif ENC1_MAP == 5                   // A: PA0, B: PA1
  #define ENC1_A_GPIO GPIOA
  #define ENC1_A_PIN  0
  #define ENC1_B_GPIO GPIOA
  #define ENC1_B_PIN  1
  #define ENC1_RM     9
  #define ENC1_PORT_EN (RCC_IOPAEN)
#else
  #error Wrong ENC1_MAP (0..5)
#endif

#if   ENC2_MAP == 0                   // A: PD4, B: PD3
  #define ENC2_A_GPIO GPIOD
  #define ENC2_A_PIN  4
  #define ENC2_B_GPIO GPIOD
  #define ENC2_B_PIN  3
  #define ENC2_RM     0
  #define ENC2_PORT_EN (RCC_IOPDEN)
#elif ENC2_MAP == 1                   // A: PC5, B: PC2
  #define ENC2_A_GPIO GPIOC
  #define ENC2_A_PIN  5
  #define ENC2_B_GPIO GPIOC
  #define ENC2_B_PIN  2
  #define ENC2_RM     2
  #define ENC2_PORT_EN (RCC_IOPCEN)
#elif ENC2_MAP == 2                   // A: PC1, B: PD3
  #define ENC2_A_GPIO GPIOC
  #define ENC2_A_PIN  1
  #define ENC2_B_GPIO GPIOD
  #define ENC2_B_PIN  3
  #define ENC2_RM     1
  #define ENC2_PORT_EN (RCC_IOPCEN | RCC_IOPDEN)
#elif ENC2_MAP == 3                   // A: PC1, B: PC7
  #define ENC2_A_GPIO GPIOC
  #define ENC2_A_PIN  1
  #define ENC2_B_GPIO GPIOC
  #define ENC2_B_PIN  7
  #define ENC2_RM     3
  #define ENC2_PORT_EN (RCC_IOPCEN)
#elif ENC2_MAP == 4                   // A: PC0, B: PC1
  #define ENC2_A_GPIO GPIOC
  #define ENC2_A_PIN  0
  #define ENC2_B_GPIO GPIOC
  #define ENC2_B_PIN  1
  #define ENC2_RM     4
  #define ENC2_PORT_EN (RCC_IOPCEN)
#elif ENC2_MAP == 5                   // A: PA0, B: PA1
  #define ENC2_A_GPIO GPIOA
  #define ENC2_A_PIN  0
  #define ENC2_B_GPIO GPIOA
  #define ENC2_B_PIN  1
  #define ENC2_RM     5
  #define ENC2_PORT_EN (RCC_IOPAEN)
#elif ENC2_MAP == 6                   // A: PB1, B: PA1
  #define ENC2_A_GPIO GPIOB
  #define ENC2_A_PIN  1
  #define ENC2_B_GPIO GPIOA
  #define ENC2_B_PIN  1
  #define ENC2_RM     6
  #define ENC2_PORT_EN (RCC_IOPAEN | RCC_IOPBEN)
#elif ENC2_MAP == 7                   // A: PD3, B: PD4
  #define ENC2_A_GPIO GPIOD
  #define ENC2_A_PIN  3
  #define ENC2_B_GPIO GPIOD
  #define ENC2_B_PIN  4
  #define ENC2_RM     7
  #define ENC2_PORT_EN (RCC_IOPDEN)
#else
  #error Wrong ENC2_MAP (0..7)
#endif

// ===================================================================================
// Rotary Encoder 1 using Timer1
// ===================================================================================

// Init rotary encoder 1 pins and setup timer1 in encoder mode
void ENC1_init(void) {
  // Enable clocks: GPIO port(s), AFIO, timer1
  RCC->PB2PCENR |= ENC1_PORT_EN | RCC_AFIOEN | RCC_TIM1EN;

  // Select pin mapping (TIM1_RM[3:0] = AFIO_PCFR1[13:10])
  AFIO->PCFR1 = (AFIO->PCFR1 & ~AFIO_PCFR1_TIM1_REMAP) | ((uint32_t)ENC1_RM << 10);

  // Set pins to input with pullup
  ENC1_A_GPIO->CFGLR = (ENC1_A_GPIO->CFGLR & ~((uint32_t)0xf << (ENC1_A_PIN << 2)))
                                           |  ((uint32_t)0x8 << (ENC1_A_PIN << 2));
  ENC1_B_GPIO->CFGLR = (ENC1_B_GPIO->CFGLR & ~((uint32_t)0xf << (ENC1_B_PIN << 2)))
                                           |  ((uint32_t)0x8 << (ENC1_B_PIN << 2));
  ENC1_A_GPIO->BSHR  = (uint32_t)1 << ENC1_A_PIN;
  ENC1_B_GPIO->BSHR  = (uint32_t)1 << ENC1_B_PIN;

  // Setup timer
  TIM1->CHCTLR1   = (uint16_t)0x0101;       // CH1 and CH2 as inputs (TI1, TI2)
  TIM1->SMCFGR    = (uint16_t)0b011;        // set encoder mode 3
  TIM1->CTLR1     = TIM_CEN;                // enable/start timer1
}

// Set rotary encoder 1 current and maximum count value
void ENC1_set(uint16_t cur, uint16_t max) {
  TIM1->CNT    = cur;                       // set current count value (timer1 counter)
  TIM1->ATRLR  = max;                       // set max count value (timer1 auto-reload)
  TIM1->SWEVGR = TIM_UG;                    // re-initialize timer
}

// Read rotary encoder 1 current count value
uint16_t ENC1_get(void) {
  return TIM1->CNT;                         // read current counter value
}

// ===================================================================================
// Rotary Encoder 2 using Timer2
// ===================================================================================

// Init rotary encoder 2 pins and setup timer2 in encoder mode
void ENC2_init(void) {
  // Enable clocks: GPIO port(s), AFIO, timer2
  RCC->PB2PCENR |= ENC2_PORT_EN | RCC_AFIOEN;
  RCC->PB1PCENR |= RCC_TIM2EN;

  // Select pin mapping (TIM2_RM[2:0] = AFIO_PCFR1[16:14])
  AFIO->PCFR1 = (AFIO->PCFR1 & ~AFIO_PCFR1_TIM2_REMAP) | ((uint32_t)ENC2_RM << 14);

  // Set pins to input with pullup
  ENC2_A_GPIO->CFGLR = (ENC2_A_GPIO->CFGLR & ~((uint32_t)0xf << (ENC2_A_PIN << 2)))
                                           |  ((uint32_t)0x8 << (ENC2_A_PIN << 2));
  ENC2_B_GPIO->CFGLR = (ENC2_B_GPIO->CFGLR & ~((uint32_t)0xf << (ENC2_B_PIN << 2)))
                                           |  ((uint32_t)0x8 << (ENC2_B_PIN << 2));
  ENC2_A_GPIO->BSHR  = (uint32_t)1 << ENC2_A_PIN;
  ENC2_B_GPIO->BSHR  = (uint32_t)1 << ENC2_B_PIN;

  // Setup timer
  TIM2->CHCTLR1   = (uint16_t)0x0101;       // CH1 and CH2 as inputs (TI1, TI2)
  TIM2->SMCFGR    = (uint16_t)0b011;        // set encoder mode 3
  TIM2->CTLR1     = TIM_CEN;                // enable/start timer2
}

// Set rotary encoder 2 current and maximum count value
void ENC2_set(uint16_t cur, uint16_t max) {
  TIM2->CNT    = cur;                       // set current count value (timer2 counter)
  TIM2->ATRLR  = max;                       // set max count value (timer2 auto-reload)
  TIM2->SWEVGR = TIM_UG;                    // re-initialize timer
}

// Read rotary encoder 2 current count value
uint16_t ENC2_get(void) {
  return TIM2->CNT;                         // read current counter value
}
