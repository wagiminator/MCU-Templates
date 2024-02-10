// ===================================================================================
// USB HID Data Functions for CH551, CH552 and CH554                          * v1.1 *
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

// ===================================================================================
// Front End Functions
// ===================================================================================
#if HID_DATA_FUNCTIONS > 0
// Flush the TX buffer (upload to host)
void HID_flush(void) {
  if(!HID_writeBusyFlag && HID_writePointer) {  // not busy and buffer not empty?
    HID_writeBusyFlag = 1;                      // busy for now
    UEP1_T_LEN = EP1_SIZE;                      // full buffer needs to be transmitted
    UEP1_CTRL  = (UEP1_CTRL & ~MASK_UEP_T_RES)
               | UEP_T_RES_ACK;                 // upload data to host
  }
}

// Write single byte to TX buffer
void HID_write(uint8_t c) {
  while(HID_writeBusyFlag);                     // wait for ready to write
  EP1_buffer[64 + HID_writePointer++] = c;      // write byte to buffer
  if(HID_writePointer == EP1_SIZE) HID_flush(); // flush if buffer full
}

// Read single byte from RX buffer
uint8_t HID_read(void) {
  uint8_t data;
  while(!HID_readByteCount);                    // wait for data
  data = EP1_buffer[HID_readPointer++];         // get character
  if(--HID_readByteCount == 0)                  // dec number of bytes in buffer
    UEP1_CTRL = (UEP1_CTRL & ~MASK_UEP_R_RES)
              | UEP_R_RES_ACK;                  // request new data if empty
  return data;
}
#endif

// ===================================================================================
// HID-Specific USB Handler Functions
// ===================================================================================

// Setup/reset HID endpoints
void HID_EP_init(void) {
  UEP1_DMA    = (uint16_t)EP1_buffer;           // EP1 data transfer address
  UEP1_CTRL   = bUEP_AUTO_TOG                   // EP1 Auto flip sync flag
              | UEP_T_RES_NAK                   // EP1 IN transaction returns NAK
              | UEP_R_RES_ACK;                  // EP1 OUT transaction returns ACK
  UEP4_1_MOD  = bUEP1_TX_EN                     // EP1 TX enable
              | bUEP1_RX_EN;                    // EP1 RX_enable
  UEP1_T_LEN  = 0;                              // EP1 nothing to send
  HID_readByteCount = 0;                        // reset received bytes counter
  HID_writeBusyFlag = 0;                        // reset write busy flag
}

// Endpoint 1 IN handler (HID report transfer to host)
void HID_EP1_IN(void) {
  UEP1_CTRL = (UEP1_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;  // default NAK
  HID_writeBusyFlag = 0;                        // clear busy flag

  #if HID_DATA_FUNCTIONS > 0
  HID_writePointer = 0;                         // reset write pointer
  #endif
}

// Endpoint 1 OUT handler (HID report transfer from host)
void HID_EP1_OUT(void) {
  if(U_TOG_OK) {                                // discard unsynchronized packets
    HID_readByteCount = USB_RX_LEN;             // set number of received data bytes
    if(HID_readByteCount) {                     // received some bytes=
      UEP1_CTRL = (UEP1_CTRL & ~MASK_UEP_R_RES) | UEP_R_RES_NAK;  // NAK for now

      #if HID_DATA_FUNCTIONS > 0
      HID_readPointer = 0;                      // reset read pointer for fetching
      #endif
    }
  }
}
