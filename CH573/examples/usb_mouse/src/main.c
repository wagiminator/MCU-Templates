// ===================================================================================
// Project:   CH573F Examples - USB Mouse
// Version:   v1.0
// Year:      2026
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// The board acts as a USB mouse (HID). A short press on the BOOT key moves the mouse
// pointer along a circle (optionally drawing with the left button pressed), a long
// press switches the mouse jiggler on/off (moves the pointer one pixel every few
// seconds, e.g. to keep the screen awake), the LED shows the jiggler state. The
// KEY works as the left mouse button.
//
// Tested template functions: USB device (HID mouse), keys (short and long press),
// SysTick (millis, delays).
//
// Settings in config.h: CIRCLE_RADIUS, MOUSE_DRAW, JIGGLE_INTERVAL, PIN_LED, USB
// descriptors.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "board.h"                        // board support (LED, keys)
#include "usb_mouse.h"                    // USB HID mouse

// Sine table for 1/4 circle (32 steps, values 0..127)
const int8_t SINE[33] = {
    0,   6,  12,  19,  25,  31,  37,  43,  49,  54,  60,  65,  71,  76,  81,  85,
   90,  94,  98, 102, 106, 109, 112, 115, 117, 120, 122, 123, 125, 126, 126, 127, 127
};

// Get sine of angle a (0..127 = 0..360 degrees), result -127..127
int8_t sine(uint8_t a) {
  a &= 127;
  if(a < 32)  return  SINE[a];
  if(a < 64)  return  SINE[64 - a];
  if(a < 96)  return -SINE[a - 64];
  return -SINE[128 - a];
}

// Move mouse pointer along a circle
void drawCircle(void) {
  int16_t x = CIRCLE_RADIUS, y = 0;       // current position relative to center
  int16_t nx, ny;
  uint8_t a;
  LED_on();
  if(MOUSE_DRAW) MOUSE_press(MOUSE_BUTTON_LEFT);
  DLY_ms(20);
  for(a = 1; a <= 128; a++) {
    nx = (int16_t)sine(a + 32) * CIRCLE_RADIUS / 127;   // cosine
    ny = (int16_t)sine(a)      * CIRCLE_RADIUS / 127;
    MOUSE_move(nx - x, ny - y);
    x = nx; y = ny;
    DLY_ms(10);
  }
  if(MOUSE_DRAW) MOUSE_release(MOUSE_BUTTON_LEFT);
  LED_off();
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  uint8_t  jiggler = 0;                   // mouse jiggler active?
  uint32_t jiggleTime = 0;                // time of last jiggle
  uint8_t  button = 0;                    // left mouse button state

  // Setup
  BOARD_init();                           // init LED, keys, millis, ADC
  MOUSE_init();                           // init USB HID mouse

  // Loop
  while(1) {
    // BOOT key: short press draws circle, long press switches jiggler
    switch(BTN_event(BTN_BOOT, 0)) {
      case BTN_SHORT: drawCircle(); break;
      case BTN_LONG:  jiggler = !jiggler; jiggleTime = BOARD_millis(); break;
      default:        break;
    }
    LED_set(jiggler);

    // KEY: left mouse button
    if(BTN_read(BTN_KEY) != button) {
      button = !button;
      if(button) MOUSE_press(MOUSE_BUTTON_LEFT);
      else       MOUSE_release(MOUSE_BUTTON_LEFT);
      DLY_ms(20);                         // debounce
    }

    // Mouse jiggler: move pointer one pixel and back
    if(jiggler && (BOARD_millis() - jiggleTime >= JIGGLE_INTERVAL)) {
      jiggleTime = BOARD_millis();
      MOUSE_move(1, 0);
      DLY_ms(20);
      MOUSE_move(-1, 0);
    }
  }
}
