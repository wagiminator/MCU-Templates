// ===================================================================================
// Basic USB CDC Functions for CH32V103                                       * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "usb_cdc.h"

// ===================================================================================
// Variables and Defines
// ===================================================================================

// Initialize line coding
CDC_LINE_CODING_TYPE CDC_lineCoding = {
  .baudrate = 115200,       // baudrate 115200
  .stopbits = 0,            // 1 stopbit
  .parity   = 0,            // no parity
  .databits = 8             // 8 databits
};

// Variables
volatile uint8_t CDC_controlLineState = 0;  // control line state
volatile uint8_t CDC_readByteCount = 0;     // number of data bytes in IN buffer
volatile uint8_t CDC_readPointer   = 0;     // data pointer for fetching
volatile uint8_t CDC_writePointer  = 0;     // data pointer for writing
volatile uint8_t CDC_writeBusyFlag = 0;     // flag of whether upload pointer is busy

// CDC class requests
#define SET_LINE_CODING           0x20      // host configures line coding
#define GET_LINE_CODING           0x21      // host reads configured line coding
#define SET_CONTROL_LINE_STATE    0x22      // generates RS-232/V.24 style control signals

// ===================================================================================
// Front End Functions
// ===================================================================================

// Setup USB-CDC
void CDC_init(void) {
  USB_init();
}

// Check number of bytes in the IN buffer
uint8_t CDC_available(void) {
  return CDC_readByteCount;
}

// Check if OUT buffer is ready to be written
uint8_t CDC_ready(void) {
  return(!CDC_writeBusyFlag);
}

// Flush the OUT buffer
void CDC_flush(void) {
  if(!CDC_writeBusyFlag && CDC_writePointer > 0) {      // not busy and buffer not empty?
    USBHD->UEP2_T_LEN = CDC_writePointer;               // number of bytes in OUT buffer
    USBHD->UEP2_CTRL  = (USBHD->UEP2_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_ACK; // respond ACK
    CDC_writeBusyFlag = 1;                              // busy for now
    CDC_writePointer  = 0;                              // reset write pointer
  }
}

// Write single character to OUT buffer
void CDC_write(char c) {
  while(CDC_writeBusyFlag);                             // wait for ready to write
  EP2_buffer[64 + CDC_writePointer++] = c;              // write character
  if(CDC_writePointer == EP2_SIZE) CDC_flush();         // flush if buffer full
}

// Read single character from IN buffer
char CDC_read(void) {
  char data;
  while(!CDC_readByteCount);                            // wait for data
  data = EP2_buffer[CDC_readPointer++];                 // get character
  if(--CDC_readByteCount == 0)                          // dec number of bytes in buffer
    USBHD->UEP2_CTRL = (USBHD->UEP2_CTRL & ~MASK_UEP_R_RES) | UEP_R_RES_ACK;// request new data if empty
  return data;
}

// ===================================================================================
// CDC-Specific USB Handler Functions
// ===================================================================================

// Setup CDC endpoints
void CDC_EP_init(void) {
  USBHD->UEP1_DMA   = (uint32_t)EP1_buffer;     // EP1 data transfer address
  USBHD->UEP2_DMA   = (uint32_t)EP2_buffer;     // EP2 data transfer address
  USBHD->UEP1_CTRL  = UEP_AUTO_TOG              // EP1 Auto flip sync flag
                    | UEP_T_RES_NAK;            // EP1 IN transaction returns NAK
  USBHD->UEP2_CTRL  = UEP_AUTO_TOG              // EP2 Auto flip sync flag
                    | UEP_T_RES_NAK             // EP2 IN transaction returns NAK
                    | UEP_R_RES_ACK;            // EP2 OUT transaction returns ACK
  USBHD->UEP2_3_MOD = UEP2_RX_EN | UEP2_TX_EN;  // EP2 double buffer
  USBHD->UEP4_1_MOD = UEP1_TX_EN;               // EP1 TX enable
  USBHD->UEP1_T_LEN = 0;                        // Nothing to send
  USBHD->UEP2_T_LEN = 0;                        // Nothing to send
  CDC_readByteCount = 0;                        // reset received bytes counter
  CDC_writeBusyFlag = 0;                        // reset write busy flag
}

// Handle CLASS SETUP requests
uint8_t CDC_control(void) {
  uint8_t i;
  switch(USB_SetupReq) {
    case GET_LINE_CODING:                       // 0x21  currently configured
      for(i=0; i<sizeof(CDC_lineCoding); i++)
        EP0_buffer[i] = ((uint8_t*)&CDC_lineCoding)[i]; // transmit line coding to host
      return sizeof(CDC_lineCoding);
    case SET_CONTROL_LINE_STATE:                // 0x22  generates RS-232/V.24 style control signals
      CDC_controlLineState = EP0_buffer[2];     // read control line state
      return 0;
    case SET_LINE_CODING:                       // 0x20  Configure
      return 0;            
    default:
      return 0xff;                              // command not supported
  }
}

// Endpoint 0 CLASS OUT handler
void CDC_EP0_OUT(void) {
  uint8_t i, len;
  if(USB_SetupReq == SET_LINE_CODING) {         // set line coding
    if(USBHD->INT_FG & U_TOG_OK) {
      len = (uint8_t)USBHD->RX_LEN;
      for(i=0; i<((sizeof(CDC_lineCoding)<=len)?sizeof(CDC_lineCoding):len); i++)
        ((uint8_t*)&CDC_lineCoding)[i] = EP0_buffer[i];   // receive line coding from host
      USBHD->UEP0_T_LEN = 0;                              // send 0-length packet
      USBHD->UEP0_CTRL = (USBHD->UEP0_CTRL & ~MASK_UEP_RES) | UEP_R_RES_ACK | UEP_T_RES_ACK;
    }
  }
  else {
    USBHD->UEP0_T_LEN = 0;
    USBHD->UEP0_CTRL  = UEP_R_RES_ACK | UEP_T_RES_ACK;
  }
}

// Endpoint 1 IN handler
// No handling is actually necessary here, the auto-NAK is sufficient.

// Endpoint 2 IN handler (bulk data transfer to host)
void CDC_EP2_IN(void) {
  CDC_writeBusyFlag = 0;                                // clear busy flag
  USBHD->UEP2_CTRL = (USBHD->UEP2_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
}

// Endpoint 2 OUT handler (bulk data transfer from host)
void CDC_EP2_OUT(void) {
  if((USBHD->INT_FG & U_TOG_OK) && USBHD->RX_LEN) {
    // respond NAK after a packet. Let main code change response after handling.
    USBHD->UEP2_CTRL = (USBHD->UEP2_CTRL & ~MASK_UEP_R_RES) | UEP_R_RES_NAK;
    CDC_readByteCount = USBHD->RX_LEN;                  // set number of received data bytes
    CDC_readPointer = 0;                                // reset read pointer for fetching
  }
}
