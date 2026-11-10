// ===================================================================================
// Project:   Raspberry Pi Pico Examples - USB Keyboard
// Version:   v1.0
// Year:      2026
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// The board appears as a USB keyboard (HID). No driver is required.
//
//   BOOTSEL short press    types KBD_TEXT followed by the chip temperature and the
//                          uptime, e.g. into an open text editor
//   BOOTSEL long press     presses the Caps Lock key
//
// The on-board LED shows the state of the Caps Lock LED of the PC, which is sent
// by the host to all keyboards (also when Caps Lock is switched on the normal
// keyboard).
//
// Tested template functions: USB device (HID keyboard incl. LED output report),
// BOOTSEL button, ADC, temperature sensor, timer (millis).
//
// Settings in config.h: KBD_TEXT, PIN_LED, USB descriptors.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "board.h"                        // board support (LED, button, ADC)
#include "usb_keyboard.h"                 // USB HID keyboard

// Convert number to decimal string, returns pointer behind the last character
char* numToStr(char* s, int32_t n) {
  char buf[11];
  uint8_t i = 0;
  if(n < 0) { *s++ = '-'; n = -n; }
  do { buf[i++] = '0' + n % 10; n /= 10; } while(n);
  while(i) *s++ = buf[--i];
  *s = 0;
  return s;
}

// Type text, chip temperature and uptime
void typeMessage(void) {
  char line[48];
  char* p;
  int16_t t = BOARD_temp();
  KBD_print(KBD_TEXT);
  p = line;
  *p++ = ' '; *p++ = '(';
  if(t < 0) { *p++ = '-'; t = -t; }
  p = numToStr(p, t / 10); *p++ = '.';
  p = numToStr(p, t % 10);
  for(const char* s = " C, uptime "; *s; ) *p++ = *s++;
  p = numToStr(p, BOARD_millis() / 1000);
  *p++ = ' '; *p++ = 's'; *p++ = ')'; *p++ = '\n'; *p = 0;
  KBD_print(line);
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  BOARD_init();                           // init LED, millis, ADC
  KBD_init();                             // init USB HID keyboard

  // Loop
  while(1) {
    // BOOTSEL button: short press types text, long press toggles Caps Lock
    switch(BTN_event(0)) {
      case BTN_SHORT: typeMessage(); break;
      case BTN_LONG:  KBD_type(KBD_KEY_CAPS_LOCK); break;
      default:        break;
    }

    // LED shows Caps Lock state of the host
    if(KBD_CAPS_LOCK_state != LED_state()) LED_set(KBD_CAPS_LOCK_state);
  }
}
