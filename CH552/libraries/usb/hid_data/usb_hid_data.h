// ===================================================================================
// USB HID Data Functions for CH551, CH552 and CH554                          * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// HID_init()               init USB HID data
// HID_available()          get number of bytes in the RX buffer
// HID_ready()              check if TX buffer is ready to be written
// HID_read()               read single byte from RX buffer (*)
// HID_write(c)             write single byte to TX buffer, flush if full (*)
// HID_flush()              flush TX buffer (*)
//
// (*) only available if HID_DATA_FUNCTIONS is set to 1 (see below in parameters)
//
// 2022 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch554.h"
#include "usb.h"
#include "usb_descr.h"
#include "usb_handler.h"

// ===================================================================================
// HID Parameters
// ===================================================================================
#define HID_DATA_FUNCTIONS    1                     // 1: enable additional functions

// ===================================================================================
// HID Variables
// ===================================================================================
extern volatile __xdata uint8_t HID_readByteCount;  // number of data bytes in RX buffer
extern volatile __bit HID_writeBusyFlag;            // TX buffer is being transmitted flag

// ===================================================================================
// HID Functions
// ===================================================================================
#define HID_init          USB_init                  // setup USB HID data
#define HID_available()   (HID_readByteCount)       // ready to be read
#define HID_ready()       (!HID_writeBusyFlag)      // ready to be written

#if HID_DATA_FUNCTIONS > 0
void HID_flush(void);                               // flush TX buffer
uint8_t HID_read(void);                             // read single byte from RX buffer
void HID_write(uint8_t c);                          // write single byte to TX buffer
#endif
