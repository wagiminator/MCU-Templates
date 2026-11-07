// ===================================================================================
// Basic 433MHz Remote Control Transmitter Functions                          * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "rc433.h"

// Send high pulse of h*T and low pulse of l*T (internal)
static void RC_pulse(uint8_t h, uint8_t l) {
  PIN_high(RC_PIN);
  while(h--) DLY_us(RC_PULSE);
  PIN_low(RC_PIN);
  while(l--) DLY_us(RC_PULSE);
}

// Init data pin
void RC_init(void) {
  PIN_low(RC_PIN);
  PIN_output(RC_PIN);
}

// Send code with number of bits
void RC_send(uint32_t code, uint8_t bits) {
  uint8_t r, i;
  if(bits > 32) bits = 32;
  for(r = RC_REPEAT; r; r--) {
    for(i = bits; i; i--) {                       // MSB first
      if((code >> (i - 1)) & 1) RC_pulse(3, 1);   // bit 1
      else                      RC_pulse(1, 3);   // bit 0
    }
    RC_pulse(1, 31);                              // sync
  }
}
