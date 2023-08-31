// ===================================================================================
// USB HID Functions for CH551, CH552 and CH554                               * v1.1 *
// ===================================================================================

#include "ch554.h"
#include "usb.h"
#include "usb_hid.h"
#include "usb_descr.h"

// ===================================================================================
// Variables and Defines
// ===================================================================================
volatile __bit HID_writeBusyFlag = 0;                     // upload pointer busy flag

// ===================================================================================
// Front End Functions
// ===================================================================================

// Send HID report
void HID_sendReport(__xdata uint8_t* buf, uint8_t len) {
  uint8_t i;
  while(HID_writeBusyFlag);                                   // wait for ready to write
  for(i=0; i<len; i++) EP1_buffer[i] = buf[i];                // copy report to EP1 buffer
  UEP1_T_LEN = len;                                           // set length to upload
  HID_writeBusyFlag = 1;                                      // set busy flag
  UEP1_CTRL = (UEP1_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_ACK;  // upload data and respond ACK
}

// ===================================================================================
// HID-Specific USB Handler Functions
// ===================================================================================

// Setup/reset HID endpoints
void HID_EP_init(void) {
  UEP1_DMA    = (uint16_t)EP1_buffer;       // EP1 data transfer address
  UEP1_CTRL   = bUEP_AUTO_TOG               // EP1 Auto flip sync flag
              | UEP_T_RES_NAK;              // EP1 IN transaction returns NAK
  UEP4_1_MOD  = bUEP1_TX_EN;                // EP1 TX enable
  UEP1_T_LEN  = 0;                          // EP1 nothing to send
  HID_writeBusyFlag = 0;                    // reset write busy flag
}

// Endpoint 1 IN handler (HID report transfer to host)
void HID_EP1_IN(void) {
  UEP1_T_LEN = 0;                                             // no data to send anymore
  UEP1_CTRL = (UEP1_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;  // default NAK
  HID_writeBusyFlag = 0;                                      // clear busy flag
}
