// ===================================================================================
// Board Support for CH573F Boards                                            * v1.0 *
// ===================================================================================
//
// Common functions for the on-board hardware (LED and two keys), which are used by
// all examples. The pins and the board name are defined in config.h.
//
// Functions available:
// --------------------
// BOARD_init()             init LED, keys, millis counter and ADC (call first)
// BOARD_millis()           get milliseconds since start (32-bit)
// BOARD_resetReason()      get reason of last reset as string
// BOARD_printInfo(out)     print board information (chip, clock, MAC, unique ID,
//                          last reset) via output function (e.g. TERM_putchar)
// BOARD_temp()             read chip temperature in 1/10 degrees Celsius
// BOARD_vdd()              read supply voltage VDD in mV
// BOARD_random()           get 32-bit random number (ADC noise and SysTick)
//
// LED_on()                 switch on-board LED on
// LED_off()                switch on-board LED off
// LED_toggle()             toggle on-board LED
// LED_set(v)               set LED (0: off, 1: on)
// LED_state()              get LED state (0: off, 1: on)
// LED_flash(ms)            switch LED on for n ms (non-blocking, see LED_update())
// LED_update()             switch LED off after flash time (call regularly)
//
// BTN_read(k)              check if key k (BTN_BOOT or BTN_KEY) is currently pressed
// BTN_pressed(k)           check if key k was pressed (debounced, once per press,
//                          call regularly)
// BTN_event(k, dbl)        get key event (call regularly): BTN_NONE, BTN_SHORT,
//                          BTN_LONG (held for BTN_LONG_MS), BTN_DOUBLE (only if
//                          dbl = 1, a short press is then reported after
//                          BTN_DOUBLE_MS)
//
// Minimal C library (the templates are linked without libc):
// ----------------------------------------------------------
// strcmp(), strncmp(), strlen(), memcpy(), memset(), memcmp()
//
// Notes:
// ------
// - BTN_BOOT is the key on PB22 (USB bootloader key, can be used as a normal key
//   after reset), BTN_KEY is the key on PB23. PB23 is also the external reset
//   input RST#, if this has been enabled in the configuration bits (WCH ISP tool),
//   then the key resets the chip.
// - The keys connect the pins to ground, the internal pull-ups are used.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "config.h"
#include "system.h"
#include "gpio.h"
#include "print.h"
#include "millis.h"

// ===================================================================================
// Board Functions
// ===================================================================================
void        BOARD_init(void);
const char* BOARD_resetReason(void);
void        BOARD_printInfo(void (*out)(char));
int16_t     BOARD_temp(void);
uint16_t    BOARD_vdd(void);
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
// Key Functions
// ===================================================================================
#define BTN_LONG_MS     800             // min. time for a long press in ms
#define BTN_DOUBLE_MS   350             // max. time between two presses of a double press

enum{BTN_BOOT, BTN_KEY};                // keys
enum{BTN_NONE, BTN_SHORT, BTN_LONG, BTN_DOUBLE};  // key events

uint8_t BTN_read(uint8_t k);
uint8_t BTN_pressed(uint8_t k);
uint8_t BTN_event(uint8_t k, uint8_t dbl);

#ifdef __cplusplus
};
#endif
