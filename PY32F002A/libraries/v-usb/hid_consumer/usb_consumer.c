// ===================================================================================
// USB HID Consumer Media Keyboard Device Functions for PY32F0xx              * v1.1 *
// ===================================================================================

#include "usb_consumer.h"

// ===================================================================================
// HID reports
// ===================================================================================
volatile uint8_t CON_report[3] = {1, 0, 0};

// ===================================================================================
// Consumer Multimedia Keyboard Functions
// ===================================================================================

// Press a consumer key on keyboard
void CON_press(uint16_t key) {
  CON_report[1] = key & 0xff;
  CON_report[2] = key >> 8;
}

// Release a consumer key on keyboard
void CON_release(void) {
  CON_report[1] = 0;
  CON_report[2] = 0;
}

// Press and release a consumer key on keyboard
void CON_type(uint16_t key) {
  CON_press(key);
  DLY_ms(2);
  CON_release();
  DLY_ms(2);
}

// ===================================================================================
// Software USB User Handle Functions (rv003usb)
// ===================================================================================
USB_RAMFUNC void usb_handle_user_in_request(struct usb_endpoint * e, uint8_t * scratchpad, int endp, uint32_t sendtok, struct rv003usb_internal * ist) {

  // Mouse
  if(endp == 1) {
    usb_send_data((uint8_t*)&CON_report, sizeof(CON_report), 0, sendtok);
  }

  // Control transfer
  else usb_send_empty(sendtok);
}
