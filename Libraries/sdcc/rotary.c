// ===================================================================================
// Basic Rotary Encoder Functions (Polling)                                   * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "rotary.h"

// Input with pull-up (not all templates have PIN_input_PU)
#ifdef PIN_input_PU
  #define ROT_inputPU(p)      PIN_input_PU(p)
#else
  #define ROT_inputPU(p)      do {PIN_input(p); PIN_pullup(p);} while(0)
#endif

// State transition table: index = (old state << 2) | new state, value = direction
static const int8_t ROT_TABLE[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

static uint8_t ROT_state;             // last state of A and B
static volatile int8_t ROT_count;     // state changes counter

// Read current state of A and B (internal)
static uint8_t ROT_read(void) {
  return (PIN_read(ROT_PIN_A) ? 2 : 0) | (PIN_read(ROT_PIN_B) ? 1 : 0);
}

// Init pins
void ROT_init(void) {
  ROT_inputPU(ROT_PIN_A);
  ROT_inputPU(ROT_PIN_B);
  #ifdef ROT_PIN_SW
  ROT_inputPU(ROT_PIN_SW);
  #endif
  ROT_state = ROT_read();
  ROT_count = 0;
}

// Read encoder state, call frequently
void ROT_update(void) {
  uint8_t state = ROT_read();
  if(state != ROT_state) {
    ROT_count += ROT_TABLE[(ROT_state << 2) | state];
    ROT_state = state;
  }
}

// Get number of detents turned since last call
int8_t ROT_get(void) {
  int8_t detents = ROT_count / ROT_STEPS;
  ROT_count -= detents * ROT_STEPS;               // keep the remainder
  return detents;
}
