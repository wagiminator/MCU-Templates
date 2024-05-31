// ===================================================================================
// USB HID Multitouch Functions for CH551, CH552 and CH554                    * v0.9 *
// ===================================================================================
//
// Functions available:
// --------------------
// MT_init()                init multitouch
// MT_touchDown(c,ci,x,y)   send a multitouch report for touch down
// MT_touchUp(c,ci)         send a multitouch report for touch up

#pragma once
#include "usb_handler.h"
#include "usb_hid.h"
#include <stdint.h>

// Functions
#define MT_init HID_init // init multitouch
void MT_touchDown(uint8_t count, uint8_t contact_ident, uint16_t x,
                  uint16_t y); // send a multitouch report for touch down
void MT_touchUp(uint8_t count,
                uint8_t contact_ident); // send a multitouch report for touch up
