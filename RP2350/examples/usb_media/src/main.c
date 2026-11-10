// ===================================================================================
// Project:   Raspberry Pi Pico 2 Examples - USB Media Keys
// Version:   v1.0
// Year:      2026
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// The board appears as a USB media key controller (HID consumer control). No driver
// is required. The keys control the media player of the PC or smartphone (USB OTG):
//
//   BOOTSEL short press    MEDIA_KEY_SHORT  (default: play/pause)
//   BOOTSEL double press   MEDIA_KEY_DOUBLE (default: next track)
//   BOOTSEL long press     MEDIA_KEY_LONG   (default: mute)
//
// The LED flashes once (short), twice (double) or long (long press).
//
// Tested template functions: USB device (HID consumer control), BOOTSEL button
// (short, long and double press), timer (millis).
//
// Settings in config.h: MEDIA_KEY_SHORT, MEDIA_KEY_DOUBLE, MEDIA_KEY_LONG, PIN_LED,
// USB descriptors.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "board.h"                        // board support (LED, button)
#include "usb_consumer.h"                 // USB HID consumer control

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  BOARD_init();                           // init LED, millis, ADC
  CON_init();                             // init USB HID consumer control

  // Loop
  while(1) {
    switch(BTN_event(1)) {
      case BTN_SHORT:
        CON_type(MEDIA_KEY_SHORT);
        LED_flash(100);
        break;
      case BTN_DOUBLE:
        CON_type(MEDIA_KEY_DOUBLE);
        LED_on(); DLY_ms(100); LED_off(); DLY_ms(100);
        LED_flash(100);
        break;
      case BTN_LONG:
        CON_type(MEDIA_KEY_LONG);
        LED_flash(500);
        break;
      default:
        break;
    }
    LED_update();                         // switch off LED after flash
  }
}
