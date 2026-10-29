// ===================================================================================
// USB HID Standard Mouse Functions for CH32V003                              * v1.1 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

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
// RV003USB Software USB User Handle Functions
// ===================================================================================
void usb_handle_user_in_request(struct usb_endpoint * e, uint8_t * scratchpad, int endp, uint32_t sendtok, struct rv003usb_internal * ist) {

  // Mouse
  if(endp == 1) {
    usb_send_data((uint8_t*)&MOUSE_report, sizeof(MOUSE_report), 0, sendtok);
    MOUSE_report.xmove = 0;             // reset movements
    MOUSE_report.ymove = 0;
    MOUSE_report.wmove = 0;
  }

  // Control transfer
  else usb_send_empty(sendtok);
}
