// ===================================================================================
// Project:   CH573F Examples - USB Media Keys
// Version:   v1.0
// Year:      2026
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// The board acts as a USB media controller (HID consumer control). The BOOT key
// controls the media player: a short press toggles play/pause, a double press skips
// to the next track and a long press mutes. The KEY works as volume control: a
// short press turns the volume up, a long press turns it down. The LED flashes with
// each command.
//
// Tested template functions: USB device (HID consumer control), keys (short, long
// and double press), SysTick (millis).
//
// Settings in config.h: MEDIA_KEY_SHORT, MEDIA_KEY_DOUBLE, MEDIA_KEY_LONG, PIN_LED,
// USB descriptors.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "board.h"                        // board support (LED, keys)
#include "usb_consumer.h"                 // USB HID consumer control

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  BOARD_init();                           // init LED, keys, millis, ADC
  CON_init();                             // init USB HID consumer control

  // Loop
  while(1) {
    // BOOT key: media player
    switch(BTN_event(BTN_BOOT, 1)) {
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

    // KEY: volume up (short press) / down (long press)
    switch(BTN_event(BTN_KEY, 0)) {
      case BTN_SHORT: CON_type(CON_VOL_UP);   LED_flash(50); break;
      case BTN_LONG:  CON_type(CON_VOL_DOWN); LED_flash(50); break;
      default:        break;
    }
    LED_update();                         // switch off LED after flash
  }
}
