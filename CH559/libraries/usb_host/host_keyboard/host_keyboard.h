// ===================================================================================
// USB Host Keyboard Functions for CH559                                      * v1.0 *
// ===================================================================================
//
// Use a USB keyboard (boot protocol) connected to the CH559 USB host port.
//
// Functions available:
// --------------------
// KBD_init()               init USB host
// KBD_update()             handle USB host and poll keyboard (call frequently),
//                          returns 1 if a new report was received
// KBD_connected()          check if a keyboard is connected and ready
// KBD_available()          check if characters are in the input buffer
// KBD_read()               read next character from input buffer (0 if empty)
// KBD_getModifiers()       get current modifier keys (KBD_MOD_xxx bits)
// KBD_isPressed(k)         check if key with HID keycode k is currently pressed
// KBD_setLEDs(l)           set keyboard LEDs (KBD_LED_NUM | KBD_LED_CAPS | KBD_LED_SCROLL)
// KBD_report[]             last received report (8 bytes: modifiers, 0, 6 keycodes)
//
// Characters returned by KBD_read():
// ----------------------------------
// - ASCII characters (US layout) incl. '\n' (Enter), '\b' (Backspace), '\t' (Tab),
//   0x1B (Escape) and 0x7F (Delete).
// - Other keys are returned as 0x80 | HID keycode, e.g. KBD_KEY_UP (0x80 | 0x52).
// - CapsLock and NumLock are handled internally (incl. keyboard LEDs).
// - No key repeat (typematic) function.
//
// Notes:
// ------
// - With USBH_PORTS = 2 the device can be connected to either root hub port.
//
// Example:
// --------
// KBD_init();
// while(1) {
//   KBD_update();
//   while(KBD_available()) UART0_write(KBD_read());
// }
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "usb_host.h"

// Parameters
#define KBD_BUF_SIZE        8                       // input buffer size (power of 2)

// Modifier bits
#define KBD_MOD_LCTRL       0x01
#define KBD_MOD_LSHIFT      0x02
#define KBD_MOD_LALT        0x04
#define KBD_MOD_LGUI        0x08
#define KBD_MOD_RCTRL       0x10
#define KBD_MOD_RSHIFT      0x20
#define KBD_MOD_RALT        0x40
#define KBD_MOD_RGUI        0x80

// LED bits
#define KBD_LED_NUM         0x01
#define KBD_LED_CAPS        0x02
#define KBD_LED_SCROLL      0x04

// Special keys (returned by KBD_read)
#define KBD_KEY_F1          (0x80 | 0x3A)           // F1..F12: 0xBA..0xC5
#define KBD_KEY_INSERT      (0x80 | 0x49)
#define KBD_KEY_HOME        (0x80 | 0x4A)
#define KBD_KEY_PAGE_UP     (0x80 | 0x4B)
#define KBD_KEY_END         (0x80 | 0x4D)
#define KBD_KEY_PAGE_DOWN   (0x80 | 0x4E)
#define KBD_KEY_RIGHT       (0x80 | 0x4F)
#define KBD_KEY_LEFT        (0x80 | 0x50)
#define KBD_KEY_DOWN        (0x80 | 0x51)
#define KBD_KEY_UP          (0x80 | 0x52)

// Variables
extern uint8_t KBD_port;                    // root hub port of the device
extern __xdata uint8_t KBD_report[8];

// Functions
#define KBD_init()          USBH_init()
#define KBD_connected()     USBH_ready(KBD_port)
#define KBD_getModifiers()  (KBD_report[0])
uint8_t KBD_update(void);
uint8_t KBD_available(void);
char KBD_read(void);
uint8_t KBD_isPressed(uint8_t key);
void KBD_setLEDs(uint8_t leds);
