// ===================================================================================
// USB HID Standard Mouse Functions for megaAVR 0-Series                      * v1.1 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "usb_mouse.h"

// ===================================================================================
// Mouse HID Report
// ===================================================================================

// HID report typedef
typedef struct {
  uint8_t buttons;                    // button states
  int8_t  xmove;                      // relative movement on the x-axis
  int8_t  ymove;                      // relative movement on the y-axis
  int8_t  wmove;                      // relative movement of the wheel
} HID_MOUSE_REPORT_TYPE;

// Initialize HID report
volatile HID_MOUSE_REPORT_TYPE MOUSE_report = {
  .buttons = 0,
  .xmove   = 0,
  .ymove   = 0,
  .wmove   = 0
};

// ===================================================================================
// Mouse Functions
// ===================================================================================

// Press mouse button(s)
// Add relative movement with saturation (-127..127)
static int8_t MOUSE_add(int8_t a, int8_t b) {
  int16_t r = (int16_t)a + b;
  return (r > 127) ? 127 : (r < -127) ? -127 : (int8_t)r;
}

void MOUSE_press(uint8_t buttons) {
  MOUSE_report.buttons |= buttons;    // press button(s)
}

// Release mouse button(s)
void MOUSE_release(uint8_t buttons) {
  MOUSE_report.buttons &= ~buttons;   // release button(s)
}

// Move mouse pointer
void MOUSE_move(int8_t xrel, int8_t yrel) {
  INT_ATOMIC_BLOCK {
    MOUSE_report.xmove = MOUSE_add(MOUSE_report.xmove, xrel); // add x-movement
    MOUSE_report.ymove = MOUSE_add(MOUSE_report.ymove, yrel); // add y-movement
  }
}

// Move mouse wheel
void MOUSE_wheel(int8_t rel) {
  INT_ATOMIC_BLOCK {
    MOUSE_report.wmove = MOUSE_add(MOUSE_report.wmove, rel);  // add wheel movement
  }
}

// ===================================================================================
// V-USB Software USB User Handle Functions
// ===================================================================================
// Send report if buttons have changed or mouse was moved (called after each USB poll)
void usb_handle_user_in(void) {
  static uint8_t last_buttons;
  uint8_t buf[4];
  if(!USB_EP1_ready()) return;
  if( (MOUSE_report.buttons != last_buttons) || MOUSE_report.xmove
    || MOUSE_report.ymove || MOUSE_report.wmove ) {
    buf[0] = last_buttons = MOUSE_report.buttons;
    buf[1] = MOUSE_report.xmove; MOUSE_report.xmove = 0;
    buf[2] = MOUSE_report.ymove; MOUSE_report.ymove = 0;
    buf[3] = MOUSE_report.wmove; MOUSE_report.wmove = 0;
    USB_EP1_send(buf, 4);
  }
}

// No data from host
void usb_handle_user_data(uint8_t ep, uint8_t* data, uint8_t len) {
  (void)ep; (void)data; (void)len;
}

// GET_REPORT request
usbMsgLen_t usb_handle_get_report(uint8_t id) {
  (void)id;
  usbMsgPtr = (usbMsgPtr_t)&MOUSE_report;
  return sizeof(MOUSE_report);
}
