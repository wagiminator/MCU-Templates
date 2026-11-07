// ===================================================================================
// USB HID Game Controller (Joystick) Functions for megaAVR 0-Series          * v1.0 *
// ===================================================================================

#include "usb_joystick.h"

// ===================================================================================
// Game Controller HID Reports
// ===================================================================================
// HID report typedef
typedef struct {
  int8_t  xpos;                       // joystick position (x-axis)
  int8_t  ypos;                       // joystick position (y-axis)
  uint8_t buttons;                    // joystick button states
} HID_JOY_REPORT_TYPE;

// Initialize HID report
volatile HID_JOY_REPORT_TYPE JOY_report = {
  .xpos    = 0,
  .ypos    = 0,
  .buttons = 0
};

// ===================================================================================
// Joystick Functions
// ===================================================================================

// Press joystick button(s)
void JOY_press(uint8_t buttons) {
  JOY_report.buttons |= buttons;      // press button(s)
}

// Release joystick button(s)
void JOY_release(uint8_t buttons) {
  JOY_report.buttons &= ~buttons;     // release button(s)
}

// Set joystick position
void JOY_stick(int8_t xpos, int8_t ypos) {
  JOY_report.xpos = xpos;             // set joystick x-position
  JOY_report.ypos = ypos;             // set joystick x-position
}

// ===================================================================================
// V-USB Software USB User Handle Functions
// ===================================================================================
// Send report if it has changed (called after each USB poll)
void usb_handle_user_in(void) {
  static uint8_t last[sizeof(JOY_report)];
  uint8_t i, changed = 0;
  volatile uint8_t* report = (volatile uint8_t*)&JOY_report;
  if(!USB_EP1_ready()) return;
  for(i=0; i<sizeof(JOY_report); i++) {
    if(last[i] != report[i]) {
      last[i] = report[i];
      changed = 1;
    }
  }
  if(changed) USB_EP1_send(last, sizeof(JOY_report));
}

// No data from host
void usb_handle_user_data(uint8_t ep, uint8_t* data, uint8_t len) {
  (void)ep; (void)data; (void)len;
}

// GET_REPORT request
usbMsgLen_t usb_handle_get_report(uint8_t id) {
  (void)id;
  usbMsgPtr = (usbMsgPtr_t)&JOY_report;
  return sizeof(JOY_report);
}
