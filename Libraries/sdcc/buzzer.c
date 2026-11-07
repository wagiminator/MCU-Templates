// ===================================================================================
// Basic Buzzer/Speaker Tone Functions (Software)                             * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "buzzer.h"

// Init pin
void BUZ_init(void) {
  PIN_low(BUZ_PIN);
  PIN_output(BUZ_PIN);
}

// Play tone with frequency f for ms milliseconds (blocking)
void BUZ_tone(uint16_t freq, uint16_t ms) {
  uint16_t half, i;
  uint32_t cycles;
  if(freq < 20 || freq > 20000) {                 // pause
    while(ms--) DLY_ms(1);
    return;
  }
  half   = 50000 / freq;                          // half period in 10us units
  cycles = ((uint32_t)freq * ms) / 1000;          // number of periods
  while(cycles--) {
    PIN_high(BUZ_PIN);
    for(i = half; i; i--) DLY_us(10);
    PIN_low(BUZ_PIN);
    for(i = half; i; i--) DLY_us(10);
  }
}
