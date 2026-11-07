// ===================================================================================
// Basic HC-SR04 Ultrasonic Distance Sensor Functions                         * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// SR04_init()              init pins
// SR04_getDistance()       measure distance in mm (20 - 4000mm), returns 0 if no echo
//
// Notes:
// ------
// - The echo pulse is measured with a polling loop of 10us steps (resolution ~2mm).
//   The loop overhead depends on the MCU and F_CPU and can be compensated with
//   SR04_CAL. For exact measurements use a timer with input capture instead.
// - The ECHO output of a 5V module needs a voltage divider for 3.3V MCUs.
// - Wait at least 60ms between two measurements.
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

// HC-SR04 parameters
#define SR04_PIN_TRIG         P14     // pin connected to TRIG
#define SR04_PIN_ECHO         P15     // pin connected to ECHO
#define SR04_CAL              100     // calibration in % (increase if values too small)

// HC-SR04 functions
void SR04_init(void);                 // init pins
uint16_t SR04_getDistance(void);      // measure distance in mm

#ifdef __cplusplus
}
#endif
