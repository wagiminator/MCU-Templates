// ===================================================================================
// True Random Number Generator (TRNG) Functions for RP2350                  * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// TRNG_init()              Init TRNG and start collecting random bits
// TRNG_read()              Read 32-bit true random number (waits if necessary)
// TRNG_readBuffer(buf,len) Fill buffer (*buf) with (len) random bytes
//
// Notes:
// ------
// - The TRNG collects 192 random bits (6 words) at a time from a ring oscillator
//   based entropy source. TRNG_read() returns these words one after another and
//   starts a new collection automatically, so reading many numbers is slower.
// - For a fast pseudo random generator, use the TRNG to seed it.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

void TRNG_init(void);                         // init TRNG
uint32_t TRNG_read(void);                     // read 32-bit random number
void TRNG_readBuffer(uint8_t* buf, uint16_t len); // fill buffer with random bytes

#ifdef __cplusplus
};
#endif
