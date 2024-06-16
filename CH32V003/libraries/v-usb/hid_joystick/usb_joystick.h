// ===================================================================================
// USB HID Game Controller (Joystick) Functions for CH32V003                  * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// JOY_init()               init HID joystick
// JOY_press(b)             press joystick button(s) (bitwise)
// JOY_release(b)           release joystick button(s)
// JOY_stick(x,y)           set joystick position (x,y) [-127...127]
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "usb_handler.h"

// Functions
#define JOY_init usb_setup                  // init HID joystick
void JOY_press(uint8_t buttons);            // press joystick button(s)
void JOY_release(uint8_t buttons);          // release joystick button(s)
void JOY_stick(int8_t xpos, int8_t ypos);   // set joystick position

#ifdef __cplusplus
}
#endif
