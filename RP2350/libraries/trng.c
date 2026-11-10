// ===================================================================================
// True Random Number Generator (TRNG) Functions for RP2350                  * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "trng.h"

static uint32_t TRNG_buffer[6];                   // collected random words
static uint8_t  TRNG_count = 0;                   // number of words in buffer

// Start collecting 192 random bits
static void TRNG_collect(void) {
  TRNG->RNG_ICR = 0xFFFFFFFF;                     // clear status bits
  TRNG->RND_SOURCE_ENABLE = 1;                    // enable entropy source
}

// Init TRNG and start collecting random bits
void TRNG_init(void) {
  RESET_cycle(RESETS_RESET_TRNG);                 // reset and release TRNG
  TRNG->RNG_IMR           = 0x0F;                 // mask all interrupts (polled)
  TRNG->TRNG_CONFIG       = 0;                    // shortest inverter chain
  TRNG->TRNG_DEBUG_CONTROL = 0;                   // all statistical tests enabled
  TRNG_count = 0;
  TRNG_collect();
}

// Read 32-bit true random number
uint32_t TRNG_read(void) {
  uint8_t i;
  if(!TRNG_count) {                               // buffer empty?
    while(1) {
      while(!(TRNG->TRNG_VALID & TRNG_TRNG_VALID_EHR_VALID)); // wait for 192 bits
      if(!(TRNG->RNG_ISR & 0x0E)) break;          // no error (autocorr, CRNGT, VN)
      TRNG->TRNG_SW_RESET = 1;                    // error -> restart collection
      TRNG->RNG_IMR = 0x0F;
      TRNG_collect();
    }
    for(i = 0; i < 6; i++) TRNG_buffer[i] = TRNG->EHR_DATA[i]; // read random bits
    TRNG_count = 6;
    TRNG_collect();                               // start next collection
  }
  return TRNG_buffer[--TRNG_count];
}

// Fill buffer with random bytes
void TRNG_readBuffer(uint8_t* buf, uint16_t len) {
  uint32_t rnd = 0;
  uint8_t  n = 0;
  while(len--) {
    if(!n) { rnd = TRNG_read(); n = 4; }
    *buf++ = (uint8_t)rnd;
    rnd >>= 8; n--;
  }
}
