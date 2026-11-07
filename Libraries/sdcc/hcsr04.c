// ===================================================================================
// Basic HC-SR04 Ultrasonic Distance Sensor Functions                         * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "hcsr04.h"

// Init pins
void SR04_init(void) {
  PIN_low(SR04_PIN_TRIG);
  PIN_output(SR04_PIN_TRIG);
  PIN_input(SR04_PIN_ECHO);
}

// Measure distance in mm, returns 0 if no echo
uint16_t SR04_getDistance(void) {
  uint16_t cnt = 0;
  PIN_high(SR04_PIN_TRIG);                        // 10us trigger pulse
  DLY_us(10);
  PIN_low(SR04_PIN_TRIG);
  while(!PIN_read(SR04_PIN_ECHO)) {               // wait for start of echo pulse
    if(++cnt > 1000) return 0;                    // (~500us after trigger)
    DLY_us(10);
  }
  cnt = 0;
  while(PIN_read(SR04_PIN_ECHO)) {                // measure echo pulse in 10us steps
    if(++cnt > 2400) return 0;                    // max. 24ms (> 4m)
    DLY_us(10);
  }
  // distance = time * 343m/s / 2 = cnt * 10us * 0.1715mm/us = cnt * 1.715mm
  return (uint16_t)(((uint32_t)cnt * (1715UL * SR04_CAL)) / 100000UL);
}
