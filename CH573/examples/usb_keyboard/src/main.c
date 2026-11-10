// ===================================================================================
// Project:   CH573F Examples - USB Keyboard
// Version:   v1.0
// Year:      2026
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// The board acts as a USB keyboard (HID). A short press on the BOOT key types a
// text with the chip temperature and the uptime, a long press toggles Caps Lock. The
// KEY types the supply voltage. The LED shows the Caps Lock state of the host.
//
// Tested template functions: USB device (HID keyboard incl. LED output report),
// keys (short and long press), ADC (temperature, supply voltage), SysTick (millis).
//
// Settings in config.h: KBD_TEXT, PIN_LED, LED_ACTIVE_LOW, USB descriptors.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "board.h"                        // board support (LED, keys, ADC)
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

// Append string, returns pointer behind the last character
char* addStr(char* p, const char* s) {
  while(*s) *p++ = *s++;
  *p = 0;
  return p;
}

// Type text, chip temperature and uptime
void typeMessage(void) {
  char line[48];
  char* p;
  int16_t t = BOARD_temp();
  KBD_print(KBD_TEXT);
  p = addStr(line, " (");
  if(t < 0) { *p++ = '-'; t = -t; }
  p = numToStr(p, t / 10); *p++ = '.';
  p = numToStr(p, t % 10);
  p = addStr(p, " C, uptime ");
  p = numToStr(p, BOARD_millis() / 1000);
  p = addStr(p, " s)\n");
  KBD_print(line);
}

// Type supply voltage
void typeVoltage(void) {
  char line[24];
  char* p;
  uint16_t mv = BOARD_vdd();
  p = addStr(line, "VDD: ");
  p = numToStr(p, mv / 1000); *p++ = '.';
  if((mv % 1000) < 100) *p++ = '0';
  if((mv % 1000) < 10)  *p++ = '0';
  p = numToStr(p, mv % 1000);
  p = addStr(p, " V\n");
  KBD_print(line);
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  BOARD_init();                           // init LED, keys, millis, ADC
  KBD_init();                             // init USB HID keyboard

  // Loop
  while(1) {
    // BOOT key: short press types text, long press toggles Caps Lock
    switch(BTN_event(BTN_BOOT, 0)) {
      case BTN_SHORT: typeMessage(); break;
      case BTN_LONG:  KBD_type(KBD_KEY_CAPS_LOCK); break;
      default:        break;
    }

    // KEY: types supply voltage
    if(BTN_pressed(BTN_KEY)) typeVoltage();

    // LED shows Caps Lock state of the host
    if(KBD_CAPS_LOCK_state != LED_state()) LED_set(KBD_CAPS_LOCK_state);
  }
}
