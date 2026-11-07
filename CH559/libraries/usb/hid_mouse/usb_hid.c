// ===================================================================================
// USB HID Functions for CH559                                                * v1.2 *
// ===================================================================================

#include "usb_hid.h"

// ===================================================================================
// Variables and Defines
// ===================================================================================
volatile __bit HID_writeBusyFlag;                 // upload pointer busy flag

// ===================================================================================
// Front End Functions
// ===================================================================================

// Send HID report
void HID_sendReport(__xdata uint8_t* buf, uint8_t len) {
  uint8_t i;
  while(HID_writeBusyFlag);                       // wait for ready to write
  for(i=0; i<len; i++) EP1_buffer[i] = buf[i];    // copy report to EP1 buffer
  HID_writeBusyFlag = 1;                          // set busy flag
  UEP1_T_LEN = len;                               // set length to upload
  UEP1_CTRL  = (UEP1_CTRL & ~MASK_UEP_T_RES)
             | UEP_T_RES_ACK;                     // upload report to host
}

// ===================================================================================
// HID-Specific USB Handler Functions
// ===================================================================================

// Setup/reset HID endpoints
void HID_EP_init(void) {
  UEP1_DMA_L = (uint16_t)EP1_buffer >> 0;   // EP1 data transfer address (low)
  UEP1_DMA_H = (uint16_t)EP1_buffer >> 8;   // EP1 data transfer address (high)
  UEP1_CTRL   = bUEP_AUTO_TOG                     // EP1 Auto flip sync flag
              | UEP_T_RES_NAK;                    // EP1 IN transaction returns NAK
  UEP4_1_MOD  = bUEP1_TX_EN;                      // EP1 TX enable
  UEP1_T_LEN  = 0;                                // EP1 nothing to send
  #ifdef EP2_SIZE
  UEP2_DMA_L = (uint16_t)EP2_buffer >> 0;   // EP2 data transfer address (low)
  UEP2_DMA_H = (uint16_t)EP2_buffer >> 8;   // EP2 data transfer address (high)
  UEP2_CTRL   = bUEP_AUTO_TOG                     // EP2 Auto flip sync flag
              | UEP_R_RES_ACK;                    // EP2 OUT transaction returns ACK
  UEP2_3_MOD  = bUEP2_RX_EN;                      // EP2 RX_enable
  #endif
  HID_writeBusyFlag = 0;                          // reset write busy flag
}

// Endpoint 1 IN handler (HID report transfer to host completed)
void HID_EP1_IN(void) {
  UEP1_CTRL  = (UEP1_CTRL & ~MASK_UEP_T_RES)
             | UEP_T_RES_NAK;                     // -> respond NAK
  HID_writeBusyFlag = 0;                          // clear busy flag
}

// Endpoint 2 OUT handler (HID report transfer from host completed)
// No handling is actually necessary here, the auto-ACK is sufficient.
// The current report can be read from the EP2 buffer.
