// ===================================================================================
// Basic Rotary Encoder Functions (Polling)                                   * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// ROT_init()               init pins (with internal pull-ups)
// ROT_update()             read encoder state, call frequently (e.g. every 1ms in a
//                          timer interrupt or in the main loop)
// ROT_get()                get number of detents turned since last call (+/-),
//                          positive = clockwise
// ROT_pressed()            check if switch is pressed (if ROT_PIN_SW is defined)
//
// Notes:
// ------
// - Uses a state table, which suppresses contact bounce and invalid transitions.
// - ROT_STEPS sets the number of state changes per detent (4 for most encoders,
//   2 for some cheap ones). Swap A and B if the direction is reversed.
// - The common pin of the encoder (C) and the switch is connected to GND.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"

// Rotary encoder parameters
#define ROT_PIN_A             P14     // pin connected to A (CLK)
#define ROT_PIN_B             P15     // pin connected to B (DT)
#define ROT_PIN_SW            P16     // pin connected to switch (comment out if unused)
#define ROT_STEPS             4       // state changes per detent (4 or 2)

// Rotary encoder functions
void ROT_init(void);                  // init pins
void ROT_update(void);                // read encoder state
int8_t ROT_get(void);                 // get detents since last call

#ifdef ROT_PIN_SW
#define ROT_pressed()         (!PIN_read(ROT_PIN_SW))
#endif

#ifdef __cplusplus
}
#endif
