// ===================================================================================
// USB HID Game Controller (Joystick) Functions for CH32V003                  * v1.0 *
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
// RV003USB Software USB User Handle Functions
// ===================================================================================
void usb_handle_user_in_request(struct usb_endpoint * e, uint8_t * scratchpad, int endp, uint32_t sendtok, struct rv003usb_internal * ist) {

  // Mouse
  if(endp == 1) {
    usb_send_data((uint8_t*)&JOY_report, sizeof(JOY_report), 0, sendtok);
  }

  // Control transfer
  else usb_send_empty(sendtok);
}
