// ===================================================================================
// USB Host Gamepad/Joystick Functions for CH559                              * v1.0 *
// ===================================================================================
//
// Use a USB HID gamepad or joystick connected to the CH559 USB host port. The HID
// report descriptor is parsed to find the axes, the hat switch and the buttons.
//
// Functions available:
// --------------------
// GPD_init()               init USB host
// GPD_update()             handle USB host and poll gamepad (call frequently),
//                          returns 1 if a new report was received
// GPD_connected()          check if a gamepad is connected and ready
// GPD_getAxis(n)           get axis n (GPD_X, GPD_Y, GPD_Z, GPD_RX, GPD_RY, GPD_RZ),
//                          0..255 (center ~128), 128 if axis not available
// GPD_hasAxis(n)           check if axis n is available
// GPD_getHat()             get hat switch / D-pad (0: up, 1: up-right, ..., 7: up-left,
//                          GPD_HAT_CENTER: not pressed or not available)
// GPD_getButtons()         get buttons (bit 0 = button 1, up to 16 buttons)
// GPD_getButtonCount()     get number of buttons
//
// Notes:
// ------
// - With USBH_PORTS = 2 the device can be connected to either root hub port.
// - Works with standard HID gamepads/joysticks (e.g. many generic USB controllers,
//   PS3/PS4 compatible pads in HID mode). XInput controllers (Xbox 360/One) use a
//   vendor-specific protocol and are not supported.
// - Axes are scaled by their logical range to 8 bits, without calibration.
// - Report descriptors longer than USBH_BUF_SIZE (config.h) are only partially
//   parsed.
//
// Example:
// --------
// GPD_init();
// while(1) {
//   if(GPD_update()) {
//     PWM_write(P24, GPD_getAxis(GPD_X));
//     if(GPD_getButtons() & 0x0001) ...      // button 1 pressed
//   }
// }
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "usb_host.h"

// Axis numbers
#define GPD_X               0
#define GPD_Y               1
#define GPD_Z               2
#define GPD_RX              3
#define GPD_RY              4
#define GPD_RZ              5
#define GPD_AXES            6

// Hat switch
#define GPD_HAT_CENTER      8

// Variables
extern uint8_t GPD_port;                    // root hub port of the device
extern __xdata uint8_t  GPD_axis[GPD_AXES];   // current axis values
extern __xdata uint8_t  GPD_hat;              // current hat value
extern __xdata uint16_t GPD_buttons;          // current buttons
extern __xdata uint8_t  GPD_btnCount;         // number of buttons
extern __xdata uint8_t  GPD_axisMask;         // available axes (bit n = axis n)

// Functions
#define GPD_init()          USBH_init()
#define GPD_connected()     USBH_ready(GPD_port)
#define GPD_getAxis(n)      (GPD_axis[(n)])
#define GPD_hasAxis(n)      (GPD_axisMask & (1 << (n)))
#define GPD_getHat()        (GPD_hat)
#define GPD_getButtons()    (GPD_buttons)
#define GPD_getButtonCount() (GPD_btnCount)
uint8_t GPD_update(void);
