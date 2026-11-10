// ===================================================================================
// Board Support for Raspberry Pi Pico                                        * v1.0 *
// ===================================================================================
//
// Common functions for the on-board hardware, which are used by all examples. The
// pins and the board name are defined in config.h.
//
// Functions available:
// --------------------
// BOARD_init()             init LED, millis counter and ADC (must be called first)
// BOARD_millis()           get milliseconds since start (32-bit)
// BOARD_resetReason()      get reason of last reset as string
// BOARD_printInfo(out)     print board information (chip, clock, unique ID, last
//                          reset) via output function (e.g. USB_putchar)
// BOARD_temp()             read chip temperature in 1/10 degrees Celsius
// BOARD_vsys()             read supply voltage VSYS in mV (GP29 = VSYS/3)
// BOARD_random()           get 32-bit random number (ring oscillator random bit)
//
// LED_on()                 switch on-board LED on
// LED_off()                switch on-board LED off
// LED_toggle()             toggle on-board LED
// LED_set(v)               set LED (0: off, 1: on)
// LED_state()              get LED state (0: off, 1: on)
// LED_flash(ms)            switch LED on for n ms (non-blocking, see LED_update())
// LED_update()             switch LED off after flash time (call regularly)
//
// BTN_read()               check if BOOTSEL button is currently pressed
// BTN_pressed()            check if BOOTSEL button was pressed (debounced, once per
//                          press, call regularly)
// BTN_event(dbl)           get button event (call regularly): BTN_NONE, BTN_SHORT,
//                          BTN_LONG (held for BTN_LONG_MS), BTN_DOUBLE (only if
//                          dbl = 1, a short press is then reported after
//                          BTN_DOUBLE_MS)
//
// Notes:
// ------
// - The BOOTSEL button is not connected to a GPIO, but to the chip select line of
//   the flash memory. To read it, the chip select output is briefly disabled, so
//   the flash cannot be accessed during that time. BTN_read() therefore runs from
//   SRAM with interrupts disabled. Core 1 must not execute code from flash while
//   the button is read.
// - The ADC is used for the temperature sensor and VSYS. Analog inputs GP26..GP28
//   (ADC0..ADC2) can be read with the ADC functions of gpio.h.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "gpio.h"
#include "print.h"
#include "millis.h"
#include "config.h"

// ===================================================================================
// Board Functions
// ===================================================================================
void        BOARD_init(void);
const char* BOARD_resetReason(void);
void        BOARD_printInfo(void (*out)(char));
int16_t     BOARD_temp(void);
uint16_t    BOARD_vsys(void);
uint32_t    BOARD_random(void);
#define     BOARD_millis()    MIL_read()

// ===================================================================================
// LED Functions
// ===================================================================================
void    LED_set(uint8_t on);
uint8_t LED_state(void);
void    LED_flash(uint16_t ms);
void    LED_update(void);

#define LED_on()        LED_set(1)
#define LED_off()       LED_set(0)
#define LED_toggle()    LED_set(!LED_state())

// ===================================================================================
// Button Functions (BOOTSEL button)
// ===================================================================================
#define BTN_LONG_MS     800             // min. time for a long press in ms
#define BTN_DOUBLE_MS   350             // max. time between two presses of a double press

enum{BTN_NONE, BTN_SHORT, BTN_LONG, BTN_DOUBLE};

uint8_t BTN_read(void);
uint8_t BTN_pressed(void);
uint8_t BTN_event(uint8_t dbl);

#ifdef __cplusplus
};
#endif
