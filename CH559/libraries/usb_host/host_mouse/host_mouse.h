// ===================================================================================
// USB Host Mouse Functions for CH559                                         * v1.0 *
// ===================================================================================
//
// Use a USB mouse (boot protocol) connected to the CH559 USB host port.
//
// Functions available:
// --------------------
// MOUSE_init()             init USB host
// MOUSE_update()           handle USB host and poll mouse (call frequently),
//                          returns 1 if a new report was received
// MOUSE_connected()        check if a mouse is connected and ready
// MOUSE_getButtons()       get current buttons (MOUSE_BUTTON_xxx bits)
// MOUSE_getX()             get accumulated X movement since last call (int16)
// MOUSE_getY()             get accumulated Y movement since last call (int16)
// MOUSE_getWheel()         get accumulated wheel movement since last call (int8)
//
// Notes:
// ------
// - With USBH_PORTS = 2 the device can be connected to either root hub port.
// - In boot protocol the wheel is not standardized, it is evaluated if the mouse
//   sends a 4th byte.
//
// Example:
// --------
// MOUSE_init();
// while(1) {
//   if(MOUSE_update()) {
//     x += MOUSE_getX(); y += MOUSE_getY();
//     if(MOUSE_getButtons() & MOUSE_BUTTON_LEFT) ...
//   }
// }
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "usb_host.h"

// Button bits
#define MOUSE_BUTTON_LEFT   0x01
#define MOUSE_BUTTON_RIGHT  0x02
#define MOUSE_BUTTON_MIDDLE 0x04

// Variables
extern uint8_t MOUSE_port;                    // root hub port of the device

// Functions
#define MOUSE_init()        USBH_init()
#define MOUSE_connected()   USBH_ready(MOUSE_port)
uint8_t MOUSE_update(void);
uint8_t MOUSE_getButtons(void);
int16_t MOUSE_getX(void);
int16_t MOUSE_getY(void);
int8_t  MOUSE_getWheel(void);
