// ===================================================================================
// USB HID Consumer Media Keyboard Device Functions for CH32X035/X034/X033    * v1.0 *
// ===================================================================================

#include "usb_consumer.h"

#define CON_sendReport()    HID_sendReport(CON_report, sizeof(CON_report))

// ===================================================================================
// HID reports
// ===================================================================================
uint8_t CON_report[3] = {1, 0, 0};

// ===================================================================================
// Consumer Multimedia Keyboard Functions
// ===================================================================================

// Press a consumer key on keyboard
void CON_press(uint8_t key) {
  CON_report[1] = key;
  CON_sendReport();                             // send report
}

// Release a consumer key on keyboard
void CON_release(void) {
  CON_report[1] = 0;
  CON_sendReport();
}

// Press and release a consumer key on keyboard
void CON_type(uint8_t key) {
  CON_press(key);
  CON_release();
}
