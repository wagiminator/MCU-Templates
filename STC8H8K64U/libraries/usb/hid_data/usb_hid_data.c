// ===================================================================================
// USB HID Data Functions for STC8H8K64U                          * v1.1 *
// ===================================================================================

#include "usb_hid_data.h"

// ===================================================================================
// Variables and Defines
// ===================================================================================
volatile __xdata uint8_t HID_readByteCount;     // number of data bytes in RX buffer
volatile __bit HID_writeBusyFlag;               // TX buffer is being transmitted flag

#if HID_DATA_FUNCTIONS > 0
volatile __xdata uint8_t HID_readPointer;       // data pointer for fetching
volatile __xdata uint8_t HID_writePointer = 0;  // data pointer for writing
#endif

// Endpoint buffers (EP1: OUT at offset 0, IN at offset 64)
#define HID_IN_buffer           (EP1_buffer + 64)

// ===================================================================================
// Front End Functions
// ===================================================================================
#if HID_DATA_FUNCTIONS > 0
// Flush the TX buffer (upload to host)
void HID_flush(void) {
  if(!HID_writeBusyFlag && HID_writePointer) {  // not busy and buffer not empty?
    while(HID_writePointer < EP1_SIZE)
      HID_IN_buffer[HID_writePointer++] = 0;    // fill up report with zeros
    HID_writeBusyFlag = 1;                      // busy for now
    HID_writePointer  = 0;                      // reset write pointer
    USB_EP_send(1, EP1_SIZE);                   // full report needs to be transmitted
  }
}

// Write single byte to TX buffer
void HID_write(uint8_t c) {
  while(HID_writeBusyFlag);                     // wait for ready to write
  HID_IN_buffer[HID_writePointer++] = c;        // write byte to buffer
  if(HID_writePointer == EP1_SIZE) HID_flush(); // flush if buffer full
}

// Read single byte from RX buffer
uint8_t HID_read(void) {
  uint8_t data;
  while(!HID_readByteCount);                    // wait for data
  data = HID_OUT_buffer[HID_readPointer++];     // get character
  if(--HID_readByteCount == 0) USB_EP_receive(1); // request new data if empty
  return data;
}
#endif

// Send a complete report (EP1_SIZE bytes) from buffer
void HID_sendReport(__xdata uint8_t* buf) {
  uint8_t i;
  while(HID_writeBusyFlag);                     // wait for ready to write
  for(i=0; i<EP1_SIZE; i++) HID_IN_buffer[i] = buf[i];
  HID_writeBusyFlag = 1;                        // busy for now
  USB_EP_send(1, EP1_SIZE);                     // upload report to host
}

// Release RX buffer after the received report has been processed
void HID_release(void) {
  HID_readByteCount = 0;
  USB_EP_receive(1);
}

// ===================================================================================
// HID-Specific USB Handler Functions
// ===================================================================================
#pragma save
#pragma nooverlay                               // functions are called by USB interrupt

// Setup/reset HID endpoints
void HID_EP_init(void) {
  USB_EP_init(1, EP1_buffer, 1, 1);             // EP1: OUT and IN
  HID_readByteCount = 0;                        // reset received bytes counter
  HID_writeBusyFlag = 0;                        // reset write busy flag
  #if HID_DATA_FUNCTIONS > 0
  HID_writePointer  = 0;                        // reset write pointer
  #endif
}

// Handle HID class SETUP requests
uint8_t HID_control(void) {
  switch(USB_SetupReq) {
    case HID_SET_IDLE:
    case HID_SET_PROTOCOL:
    case HID_SET_REPORT:                        // accept (data stage is ignored)
      return 0;
    default:
      return 0xff;                              // not supported -> STALL
  }
}

// Endpoint 1 IN handler (HID report transfer to host completed)
void HID_EP1_IN(void) {
  HID_writeBusyFlag = 0;                        // clear busy flag
}

// Endpoint 1 OUT handler (HID report transfer from host completed)
void HID_EP1_OUT(void) {
  if(USB_RX_len) {                              // received some bytes?
    HID_readByteCount = USB_RX_len;             // set number of received data bytes
    #if HID_DATA_FUNCTIONS > 0
    HID_readPointer = 0;                        // reset read pointer for fetching
    #endif
  }                                             // endpoint stays at NAK
  else USB_EP_receive(1);                       // ZLP -> ready for next packet
}

#pragma restore
