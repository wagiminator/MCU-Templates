// ===================================================================================
// Delay Functions for STC8H Microcontrollers                                 * v1.0 *
// ===================================================================================

#include "delay.h"

#pragma disable_warning 84
void DLY_us(uint16_t n) {
  volatile __idata uint8_t i;
  #ifdef F_CPU
    #if F_CPU <= 5000000
      n >>= 1;
    #endif
    #if F_CPU <= 2500000
      n >>= 1;
    #endif
    #if F_CPU <= 1750000
      n >>= 1;
    #endif
    #if F_CPU <= 650000
      n >>= 1;
    #endif
  #endif

  while(n) {
    #ifdef F_CPU
      #if F_CPU >= 12000000
        --i;
      #endif
      #if F_CPU >= 14000000
        --i;
      #endif
      #if F_CPU >= 16000000
        --i;
      #endif
      #if F_CPU >= 18000000
        --i;
      #endif
      #if F_CPU >= 20000000
        --i;
      #endif
      #if F_CPU >= 22000000
        --i;
      #endif
      #if F_CPU >= 24000000
        --i;
      #endif
      #if F_CPU >= 26000000
        --i;
      #endif
      #if F_CPU >= 28000000
        --i;
      #endif
      #if F_CPU >= 30000000
        --i;
      #endif
      #if F_CPU >= 32000000
        --i;
      #endif
      #if F_CPU >= 34000000
        --i;
      #endif
      #if F_CPU >= 36000000
        --i;
      #endif
      #if F_CPU >= 38000000
        --i;
      #endif
    #endif
    --n;
  }
}

void DLY_ms(uint16_t n) {
  while(n) {
    DLY_us(1000);
    --n;
  }
}
