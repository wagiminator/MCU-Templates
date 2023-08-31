// ===================================================================================
// USB HID Functions for CH551, CH552 and CH554                               * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// HID_init()               init USB-HID
// HID_sendReport(rep, len) send HID report (pointer to report buffer, length)
//
// 2022 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "usb_handler.h"

#define HID_init USB_init                                 // setup USB-HID
void HID_sendReport(__xdata uint8_t* buf, uint8_t len);   // send HID report
