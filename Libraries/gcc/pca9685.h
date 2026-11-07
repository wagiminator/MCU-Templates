// ===================================================================================
// Basic PCA9685 16-Channel 12-Bit PWM/Servo Driver Functions                 * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// PCA_init()               init driver with PWM frequency PCA_FREQ
// PCA_setPWM(ch,v)         set duty cycle of channel ch (0-15) to v (0 - 4096),
//                          0: always off, 4096: always on
// PCA_setServo(ch,a)       set servo on channel ch (0-15) to angle a (0 - 180°)
// PCA_setPulse(ch,us)      set pulse width of channel ch (0-15) in microseconds
// PCA_sleep(s)             set sleep state (0: wake up, 1: sleep, outputs off)
//
// Notes:
// ------
// - For servos set PCA_FREQ to 50 (Hz), for LEDs e.g. to 1000.
// - The internal 25MHz oscillator has a tolerance of a few percent.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"                      // choose your I2C library
#include "system.h"
#ifdef __SDCC
#include "delay.h"
#endif

// PCA9685 parameters
#define PCA_INIT_I2C          1       // 1: init I2C with PCA_init()
#define PCA_ADDR              0x40    // I2C address (0x40 - 0x7F, set by A0..A5)
#define PCA_FREQ              50      // PWM frequency in Hz (24 - 1526)
#define PCA_SERVO_MIN         500     // servo pulse width for   0° in us
#define PCA_SERVO_MAX         2500    // servo pulse width for 180° in us

// PCA9685 functions
void PCA_init(void);                  // init driver
void PCA_setPWM(uint8_t ch, uint16_t value);  // set duty cycle (0 - 4096)
void PCA_setPulse(uint8_t ch, uint16_t us);   // set pulse width in us
void PCA_sleep(uint8_t slp);                  // set sleep state
#define PCA_setServo(ch,a)    PCA_setPulse((ch), PCA_SERVO_MIN + (uint16_t)((uint32_t)(a) \
                              * (PCA_SERVO_MAX - PCA_SERVO_MIN) / 180))

#ifdef __cplusplus
}
#endif
