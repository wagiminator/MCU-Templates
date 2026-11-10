// ===================================================================================
// Millis Functions for CH573/CH571                                           * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "millis.h"

static uint64_t MIL_start;            // SysTick value at start

// Read 64-bit SysTick counter value (counting down)
static uint64_t MIL_ticks(void) {
  uint32_t hi, lo;
  do {
    hi = STK->CNTH;
    lo = STK->CNTL;
  } while(hi != STK->CNTH);
  return ((uint64_t)hi << 32) | lo;
}

// Init and start millis counter at zero
void MIL_init(void) {
  MIL_start = MIL_ticks();
}

// Read millis counter
uint32_t MIL_read(void) {
  return (uint32_t)((MIL_start - MIL_ticks()) / (F_CPU / 1000));
}

// Read micros counter
uint32_t MIC_read(void) {
  return (uint32_t)((MIL_start - MIL_ticks()) / (F_CPU / 1000000));
}
