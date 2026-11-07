// ===================================================================================
// Basic Stepper Motor Functions                                              * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "stepper.h"

static uint16_t STEP_delay;           // delay between steps in 10us units

// Set speed in steps per second
void STEP_setSpeed(uint16_t sps) {
  if(sps < 1) sps = 1;
  if(sps > 10000) sps = 10000;
  STEP_delay = 100000UL / sps;
}

// Wait between steps (internal)
static void STEP_wait(void) {
  uint16_t i;
  for(i = STEP_delay; i; i--) DLY_us(10);
}

#if STEP_TYPE == 0
// ===================================================================================
// STEP/DIR Driver
// ===================================================================================

// Init pins
void STEP_init(void) {
  PIN_low(STEP_PIN_STEP);  PIN_output(STEP_PIN_STEP);
  PIN_low(STEP_PIN_DIR);   PIN_output(STEP_PIN_DIR);
  #ifdef STEP_PIN_EN
  PIN_low(STEP_PIN_EN);    PIN_output(STEP_PIN_EN); // enabled (active low)
  #endif
  STEP_setSpeed(STEP_SPEED);
}

// Enable/disable driver
void STEP_enable(void) {
  #ifdef STEP_PIN_EN
  PIN_low(STEP_PIN_EN);
  #endif
}

void STEP_disable(void) {
  #ifdef STEP_PIN_EN
  PIN_high(STEP_PIN_EN);
  #endif
}

// Move n steps (blocking)
void STEP_move(int32_t steps) {
  if(steps < 0) {
    PIN_low(STEP_PIN_DIR);
    steps = -steps;
  }
  else PIN_high(STEP_PIN_DIR);
  DLY_us(5);                                      // DIR setup time
  while(steps--) {
    PIN_high(STEP_PIN_STEP);                      // step pulse
    DLY_us(5);
    PIN_low(STEP_PIN_STEP);
    STEP_wait();
  }
}

#else
// ===================================================================================
// 4-Wire Unipolar Motor (Half-Step)
// ===================================================================================

// Half-step sequence (bit 0: IN1 ... bit 3: IN4)
static const uint8_t STEP_SEQ[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09};
static uint8_t STEP_phase;            // current position in sequence

// Set coil outputs (internal)
static void STEP_set(uint8_t coils) {
  if(coils & 1) PIN_high(STEP_PIN_IN1); else PIN_low(STEP_PIN_IN1);
  if(coils & 2) PIN_high(STEP_PIN_IN2); else PIN_low(STEP_PIN_IN2);
  if(coils & 4) PIN_high(STEP_PIN_IN3); else PIN_low(STEP_PIN_IN3);
  if(coils & 8) PIN_high(STEP_PIN_IN4); else PIN_low(STEP_PIN_IN4);
}

// Init pins
void STEP_init(void) {
  STEP_set(0);
  PIN_output(STEP_PIN_IN1); PIN_output(STEP_PIN_IN2);
  PIN_output(STEP_PIN_IN3); PIN_output(STEP_PIN_IN4);
  STEP_setSpeed(STEP_SPEED);
}

// Energize/release coils
void STEP_enable(void) {
  STEP_set(STEP_SEQ[STEP_phase]);
}

void STEP_disable(void) {
  STEP_set(0);
}

// Move n steps (blocking)
void STEP_move(int32_t steps) {
  while(steps) {
    if(steps > 0) {
      STEP_phase = (STEP_phase + 1) & 7;
      steps--;
    }
    else {
      STEP_phase = (STEP_phase - 1) & 7;
      steps++;
    }
    STEP_set(STEP_SEQ[STEP_phase]);
    STEP_wait();
  }
}

#endif
