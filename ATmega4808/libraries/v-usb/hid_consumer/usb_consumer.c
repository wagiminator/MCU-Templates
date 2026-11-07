// ===================================================================================
// USB HID Consumer Media Keyboard Device Functions for megaAVR 0-Series      * v1.1 *
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
// V-USB Software USB User Handle Functions
// ===================================================================================
// Send report if it has changed (called after each USB poll)
void usb_handle_user_in(void) {
  static uint8_t last[3];
  uint8_t i, changed = 0;
  if(!USB_EP1_ready()) return;
  for(i=0; i<3; i++) {
    if(last[i] != CON_report[i]) {
      last[i] = CON_report[i];
      changed = 1;
    }
  }
  if(changed) USB_EP1_send(last, 3);
}

// No data from host
void usb_handle_user_data(uint8_t ep, uint8_t* data, uint8_t len) {
  (void)ep; (void)data; (void)len;
}

// GET_REPORT request
usbMsgLen_t usb_handle_get_report(uint8_t id) {
  (void)id;
  usbMsgPtr = (usbMsgPtr_t)CON_report;
  return sizeof(CON_report);
}
