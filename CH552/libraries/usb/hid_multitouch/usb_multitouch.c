// ===================================================================================
// USB HID Multitouch Functions for CH551, CH552 and CH554                    * v0.9 *
// ===================================================================================

#include "usb_multitouch.h"

// Handle CLASS SETUP requests
uint8_t MT_control(void) {
  switch (USB_SetupReq) {
  case HID_GET_REPORT:
    EP0_buffer[0] = 10; // 10 fingers max
    return 1;

  default:
    return 0xff; // failed
  }
}

// Send a multitouch report for touch down
// Keep track of the contact count yourself
// Contact Identifiers should be unique for each contact
// X and Y are the screen dimensions as if it were a 10000x10000 screen
// Tip Switch and In Range currently hardcoded
// Pressure is currently hardcoded to maximum
void MT_touchDown(uint8_t count, uint8_t contact_ident, uint16_t x,
                  uint16_t y) {
  __xdata unsigned char touchDownReport[] = {
      count,                          // Contact Count
      contact_ident,                  // Contact Identifier
      0x03,                           // Tip Switch and In Range
      0x7F,                           // Pressure
      x & 0xff,      (x >> 8) & 0xFF, // X
      y & 0xff,      (y >> 8) & 0xFF  // Y
  };
  HID_sendReport(touchDownReport, sizeof(touchDownReport));
}

void MT_touchUp(uint8_t count, uint8_t contact_ident) {
  __xdata unsigned char touchUpReport[] = {
      count,               // Contact Count
      contact_ident,       // Contact Identifier
      0x00,                // Not in Range
      0x00,                // No pressure
      0x00,          0x00, // Disregarded
      0x00,          0x00, // Disregarded
  };
  HID_sendReport(touchUpReport, sizeof(touchUpReport));
}
