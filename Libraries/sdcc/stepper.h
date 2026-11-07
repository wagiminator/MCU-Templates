// ===================================================================================
// Basic Stepper Motor Functions                                              * v1.0 *
// ===================================================================================
//
// Supports step/direction drivers (A4988, DRV8825, TMC2208, ...) and unipolar motors
// with 4 coil drivers (28BYJ-48 with ULN2003, half-step mode).
//
// Functions available:
// --------------------
// STEP_init()              init pins
// STEP_move(n)             move n steps (+/-), blocking
// STEP_setSpeed(sps)       set speed in steps per second (1 - 10000)
// STEP_enable()            enable driver (STEP/DIR) / energize coils (4-wire)
// STEP_disable()           disable driver / release all coils (no holding torque)
//
// Notes:
// ------
// - 28BYJ-48 in half-step mode: 4096 steps per revolution, max. ~1000 steps/s.
// - The enable pin of STEP/DIR drivers is optional (comment out STEP_PIN_EN).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"
#include "system.h"
#ifdef __SDCC
#include "delay.h"
#endif

// Stepper parameters
#define STEP_TYPE             0       // 0: STEP/DIR driver, 1: 4-wire (ULN2003)
#define STEP_SPEED            500     // initial speed in steps per second

#if STEP_TYPE == 0
#define STEP_PIN_STEP         P14     // pin connected to STEP
#define STEP_PIN_DIR          P15     // pin connected to DIR
#define STEP_PIN_EN           P16     // pin connected to /EN (comment out if unused)
#else
#define STEP_PIN_IN1          P14     // pin connected to IN1 (coil A)
#define STEP_PIN_IN2          P15     // pin connected to IN2 (coil B)
#define STEP_PIN_IN3          P16     // pin connected to IN3 (coil C)
#define STEP_PIN_IN4          P17     // pin connected to IN4 (coil D)
#endif

// Stepper functions
void STEP_init(void);                 // init pins
void STEP_move(int32_t steps);        // move n steps (blocking)
void STEP_setSpeed(uint16_t sps);     // set speed in steps per second
void STEP_enable(void);               // enable driver / energize coils
void STEP_disable(void);              // disable driver / release coils

#ifdef __cplusplus
}
#endif
