// ===================================================================================
// Millis Functions for RP2350 (using the 64-bit microsecond TIMER0)          * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "millis.h"

static uint64_t MIL_start;                // TIMER0 value at reset

// Read 64-bit TIMER0 value (consistent reading of high and low word)
static uint64_t MIL_timer(void) {
  uint32_t hi, lo;
  do {
    hi = TIMER0->TIMERAWH;
    lo = TIMER0->TIMERAWL;
  } while(hi != TIMER0->TIMERAWH);
  return ((uint64_t)hi << 32) | lo;
}

// Init and start millis counter
void MIL_init(void) {
  RESET_release(RESETS_RESET_TIMER0);      // release TIMER0 from reset (if not done)
  MIL_reset();
}

// Reset millis counter to zero
void MIL_reset(void) {
  MIL_start = MIL_timer();
}

// Read current millis counter value
uint32_t MIL_read(void) {
  return (uint32_t)((MIL_timer() - MIL_start) / 1000);
}

// Read current micros counter value
uint32_t MIC_read(void) {
  return (uint32_t)(MIL_timer() - MIL_start);
}
