// ===================================================================================
// USB Handler for CH559                                                      * v1.0 *
// ===================================================================================

#include "ch559.h"
#include "usb_handler.h"

// ===================================================================================
// Variables
// ===================================================================================
volatile uint8_t  USB_SetupReq, USB_SetupTyp, USB_Config;
volatile uint16_t USB_SetupLen;
__code uint8_t *pDescr;

// ===================================================================================
// Setup/Reset Endpoints
// ===================================================================================
void USB_EP_init(void) {
  UEP0_DMA_L = (uint16_t)EP0_buffer >> 0;   // EP0 data transfer address (low)
  UEP0_DMA_H = (uint16_t)EP0_buffer >> 8;   // EP0 data transfer address (high)
  UEP0_CTRL  = UEP_R_RES_ACK                // EP0 Manual flip, OUT transaction returns ACK
             | UEP_T_RES_NAK;               // EP0 IN transaction returns NAK
  UEP0_T_LEN = 0;                           // must be zero at start

  #ifdef USB_INIT_endpoints
  USB_INIT_endpoints();                     // custom EP init handler
  #endif
}

// ===================================================================================
// USB Init Function
// ===================================================================================
void USB_init(void) {
  USB_CTRL    = bUC_DEV_PU_EN               // USB internal pull-up enable
              | bUC_INT_BUSY                // Return NAK if USB INT flag not clear
              | bUC_DMA_EN;                 // DMA enable
  UDEV_CTRL   = bUD_DP_PD_DIS               // Disable UDP/UDM pulldown resistor
              | bUD_PORT_EN;                // Enable port, full-speed

  USB_EP_init();                            // setup endpoints

  USB_INT_EN |= bUIE_SUSPEND                // Enable device hang interrupt
              | bUIE_TRANSFER               // Enable USB transfer completion interrupt
              | bUIE_BUS_RST;               // Enable device mode USB bus reset interrupt

  USB_INT_FG |= 0x1f;                       // Clear interrupt flag
  IE_USB      = 1;                          // Enable USB interrupt
  EA          = 1;                          // Enable global interrupts
}

// ===================================================================================
// Fast Copy Function
// ===================================================================================
// Copy descriptor *pDescr to Ep0 using double pointer
// (Thanks to Ralph Doncaster)
#pragma callee_saves USB_EP0_copyDescr
void USB_EP0_copyDescr(uint8_t len) {
  len;                          // stop unreferenced argument warning
  __asm
    push ar7                    ; r7 -> stack
    mov  r7, dpl                ; r7 <- len
    inc  _XBUS_AUX              ; select dptr1
    mov  dptr, #_EP0_buffer     ; dptr1 <- EP0_buffer
    dec  _XBUS_AUX              ; select dptr0
    mov  dpl, _pDescr           ; dptr0 <- *pDescr
    mov  dph, (_pDescr + 1)
    01$:
    clr  a                      ; acc <- #0
    movc a, @a+dptr             ; acc <- *pDescr[dptr0]
    inc  dptr                   ; inc dptr0
    .DB  0xA5                   ; acc -> EP0_buffer[dptr1] & inc dptr1
    djnz r7, 01$                ; repeat len times
    pop  ar7                    ; r7 <- stack
  __endasm;
}

// ===================================================================================
// Endpoint EP0 Handlers
// ===================================================================================
void USB_EP0_SETUP(void) {
  uint8_t len = 0;                                // default is success and upload 0 length
  if(USB_RX_LEN == (sizeof(USB_SETUP_REQ))) {
    USB_SetupLen = ((uint16_t)USB_SetupBuf->wLengthH<<8) | (USB_SetupBuf->wLengthL);
    USB_SetupReq = USB_SetupBuf->bRequest;
    USB_SetupTyp = USB_SetupBuf->bRequestType;

    if((USB_SetupTyp & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD) {
      #ifdef USB_CTRL_NS_handler
      len = USB_CTRL_NS_handler();                // non-standard request
      #else
      len = 0xff;                                 // command not supported
      #endif
    }

    else {                                        // standard request
      switch(USB_SetupReq) {                      // request ccfType
        case USB_GET_DESCRIPTOR:
          switch(USB_SetupBuf->wValueH) {

            case USB_DESCR_TYP_DEVICE:            // Device Descriptor
              pDescr = (uint8_t*)&DevDescr;       // put descriptor into out buffer
              len = sizeof(DevDescr);             // descriptor length
              break;

            case USB_DESCR_TYP_CONFIG:            // Configuration Descriptor
              pDescr = (uint8_t*)&CfgDescr;       // put descriptor into out buffer
              len = sizeof(CfgDescr);             // descriptor length
              break;

            case USB_DESCR_TYP_STRING:
              switch(USB_SetupBuf->wValueL) {      // String Descriptor Index
                case 0:   pDescr = USB_STR_DESCR_i0; break;
                case 1:   pDescr = USB_STR_DESCR_i1; break;
                case 2:   pDescr = USB_STR_DESCR_i2; break;
                case 3:   pDescr = USB_STR_DESCR_i3; break;
                #ifdef USB_STR_DESCR_i4
                case 4:   pDescr = USB_STR_DESCR_i4; break;
                #endif
                #ifdef USB_STR_DESCR_i5
                case 5:   pDescr = USB_STR_DESCR_i5; break;
                #endif
                #ifdef USB_STR_DESCR_i6
                case 6:   pDescr = USB_STR_DESCR_i6; break;
                #endif
                #ifdef USB_STR_DESCR_i7
                case 7:   pDescr = USB_STR_DESCR_i7; break;
                #endif
                #ifdef USB_STR_DESCR_i8
                case 8:   pDescr = USB_STR_DESCR_i8; break;
                #endif
                #ifdef USB_STR_DESCR_i9
                case 9:   pDescr = USB_STR_DESCR_i9; break;
                #endif
                #ifdef USB_STR_DESCR_ixee
                case 0xee:  pDescr = USB_STR_DESCR_ixee; break;
                #endif
                default:  pDescr = USB_STR_DESCR_ix; break;
              }
              len = pDescr[0];                    // descriptor length
              break;

            #ifdef USB_REPORT_DESCR
            case USB_DESCR_TYP_REPORT:
              if(USB_SetupBuf->wValueL == 0) {
                pDescr = USB_REPORT_DESCR;
                len = USB_REPORT_DESCR_LEN;
              }
              else len = 0xff;
              break;
            #endif

            default:
              len = 0xff;                         // unsupported descriptors or error
              break;
          }

          if(len != 0xff) {
            if(USB_SetupLen > len) USB_SetupLen = len;    // limit length
            len = USB_SetupLen >= EP0_SIZE ? EP0_SIZE : USB_SetupLen;
            USB_EP0_copyDescr(len);               // copy descriptor to Ep0
            USB_SetupLen -= len;
            pDescr += len;
          }
          break;

        case USB_SET_ADDRESS:
          USB_SetupLen = USB_SetupBuf->wValueL;   // save the assigned address
          break;

        case USB_GET_CONFIGURATION:
          EP0_buffer[0] = USB_Config;
          if(USB_SetupLen >= 1) len = 1;
          break;

        case USB_SET_CONFIGURATION:
          USB_Config = USB_SetupBuf->wValueL;
          break;

        case USB_GET_INTERFACE:
          break;

        case USB_SET_INTERFACE:
          break;

        case USB_CLEAR_FEATURE:
          if((USB_SetupTyp & 0x1f) == USB_REQ_RECIP_DEVICE) {
            if( (((uint16_t)USB_SetupBuf->wValueH << 8) | USB_SetupBuf->wValueL) == 0x01 ) {
              if( ((uint8_t*)&CfgDescr)[7] & 0x20) {
                // wake up
              }
              else len = 0xff;               // failed
            }
            else len = 0xff;                 // failed
          }
          else if((USB_SetupTyp & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) {
            switch(USB_SetupBuf->wIndexL) {
              #ifdef EP1_OUT_callback
              case 0x01:
                UEP1_CTRL = (UEP1_CTRL & ~(bUEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
                break;
              #endif
              #ifdef EP1_IN_callback
              case 0x81:
                UEP1_CTRL = (UEP1_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
                break;
              #endif
              #ifdef EP2_OUT_callback
              case 0x02:
                UEP2_CTRL = (UEP2_CTRL & ~(bUEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
                break;
              #endif
              #ifdef EP2_IN_callback
              case 0x82:
                UEP2_CTRL = (UEP2_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
                break;
              #endif
              #ifdef EP3_OUT_callback
              case 0x03:
                UEP3_CTRL = (UEP3_CTRL & ~(bUEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
                break;
              #endif
              #ifdef EP3_IN_callback
              case 0x83:
                UEP3_CTRL = (UEP3_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
                break;
              #endif
              #ifdef EP4_OUT_callback
              case 0x04:
                UEP4_CTRL = (UEP4_CTRL & ~(bUEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
                break;
              #endif
              #ifdef EP4_IN_callback
              case 0x84:
                UEP4_CTRL = (UEP4_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
                break;
              #endif
              default:
                len = 0xff;                 // unsupported endpoint
                break;
            }
          }
          else len = 0xff;                  // unsupported for non-endpoint
          break;

        case USB_SET_FEATURE:
          if((USB_SetupTyp & 0x1f) == USB_REQ_RECIP_DEVICE) {
            if( (((uint16_t)USB_SetupBuf->wValueH << 8) | USB_SetupBuf->wValueL) == 0x01 ) {
              if( !(((uint8_t*)&CfgDescr)[7] & 0x20) ) len = 0xff;  // failed
            }
            else len = 0xff;                                        // failed
          }
          else if((USB_SetupTyp & 0x1f) == USB_REQ_RECIP_ENDP) {
            if( (((uint16_t)USB_SetupBuf->wValueH << 8) | USB_SetupBuf->wValueL) == 0x00 ) {
              switch( ( (uint16_t)USB_SetupBuf->wIndexH << 8 ) | USB_SetupBuf->wIndexL ) {
                #ifdef EP1_OUT_callback
                case 0x01:
                  UEP1_CTRL = (UEP1_CTRL & ~bUEP_R_TOG) | UEP_R_RES_STALL;
                  break;
                #endif
                #ifdef EP1_IN_callback
                case 0x81:
                  UEP1_CTRL = (UEP1_CTRL & ~bUEP_T_TOG) | UEP_T_RES_STALL;
                  break;
                #endif
                #ifdef EP2_OUT_callback
                case 0x02:
                  UEP2_CTRL = (UEP2_CTRL & ~bUEP_R_TOG) | UEP_R_RES_STALL;
                  break;
                #endif
                #ifdef EP2_IN_callback
                case 0x82:
                  UEP2_CTRL = (UEP2_CTRL & ~bUEP_T_TOG) | UEP_T_RES_STALL;
                  break;
                #endif
                #ifdef EP3_OUT_callback
                case 0x03:
                  UEP3_CTRL = (UEP3_CTRL & ~bUEP_R_TOG) | UEP_R_RES_STALL;
                  break;
                #endif
                #ifdef EP3_IN_callback
                case 0x83:
                  UEP3_CTRL = (UEP3_CTRL & ~bUEP_T_TOG) | UEP_T_RES_STALL;
                  break;
                #endif
                #ifdef EP4_OUT_callback
                case 0x04:
                  UEP4_CTRL = (UEP4_CTRL & ~bUEP_R_TOG) | UEP_R_RES_STALL;
                  break;
                #endif
                #ifdef EP4_IN_callback
                case 0x84:
                  UEP4_CTRL = (UEP4_CTRL & ~bUEP_T_TOG) | UEP_T_RES_STALL;
                  break;
                #endif
                default:
                  len = 0xff;               // failed
                  break;
              }
            }
            else len = 0xff;                // failed
          }
          else len = 0xff;                  // failed
          break;

        case USB_GET_STATUS:
          EP0_buffer[0] = 0x00;
          EP0_buffer[1] = 0x00;
          if(USB_SetupLen >= 2) len = 2;
          else len = USB_SetupLen;
          break;

        default:
          len = 0xff;                       // failed
          break;
      }
    }
  }
  else len = 0xff;                          // wrong packet length

  if(len == 0xff) {
    USB_SetupReq = 0xff;
    UEP0_CTRL  = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL; // STALL
  }
  else if(len <= EP0_SIZE) {      // Tx data to host or send 0-length packet
    UEP0_T_LEN = len;
    UEP0_CTRL  = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK; // Expect DATA1, Answer ACK
  }
  else {
    UEP0_T_LEN = 0;  // Tx data to host or send 0-length packet
    UEP0_CTRL  = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK; // Expect DATA1, Answer ACK
  }
}

void USB_EP0_IN(void) {
  uint8_t len;

  #ifdef USB_IN_NS_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD) {
    USB_IN_NS_handler();
    return;
  }
  #endif

  switch(USB_SetupReq) {
    case USB_GET_DESCRIPTOR:
      len = USB_SetupLen >= EP0_SIZE ? EP0_SIZE : USB_SetupLen;
      USB_EP0_copyDescr(len);                     // copy descriptor to Ep0                                
      USB_SetupLen  -= len;
      pDescr    += len;
      UEP0_T_LEN = len;
      UEP0_CTRL ^= bUEP_T_TOG;                    // switch between DATA0 and DATA1
      break;

    case USB_SET_ADDRESS:
      USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | USB_SetupLen;
      UEP0_CTRL  = UEP_R_RES_ACK | UEP_T_RES_NAK;
      break;

    default:
      UEP0_T_LEN = 0;                             // end of transaction
      UEP0_CTRL  = UEP_R_RES_ACK | UEP_T_RES_NAK;
      break;
  }
}

void USB_EP0_OUT(void) {
  #ifdef USB_OUT_NS_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD) {
    USB_OUT_NS_handler();
    return;
  }
  #endif

  UEP0_T_LEN = 0;
  UEP0_CTRL  = (UEP0_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK; // respond NAK
}

// ===================================================================================
// USB Interrupt Service Routine
// ===================================================================================
#pragma save
#pragma nooverlay
void USB_interrupt(void) {   // inline not really working in multiple files in SDCC
  if(UIF_TRANSFER) {
    // Dispatch to service functions
    uint8_t callIndex = USB_INT_ST & MASK_UIS_ENDP;
    switch (USB_INT_ST & MASK_UIS_TOKEN) {
      case UIS_TOKEN_OUT:
        switch (callIndex) {
          case 0: EP0_OUT_callback(); break;
          #ifdef EP1_OUT_callback
          case 1: EP1_OUT_callback(); break;
          #endif
          #ifdef EP2_OUT_callback
          case 2: EP2_OUT_callback(); break;
          #endif
          #ifdef EP3_OUT_callback
          case 3: EP3_OUT_callback(); break;
          #endif
          #ifdef EP4_OUT_callback
          case 4: EP4_OUT_callback(); break;
          #endif
          default: break;
        }
        break;
      case UIS_TOKEN_SOF:
        switch (callIndex) {
          #ifdef EP0_SOF_callback
          case 0: EP0_SOF_callback(); break;
          #endif
          #ifdef EP1_SOF_callback
          case 1: EP1_SOF_callback(); break;
          #endif
          #ifdef EP2_SOF_callback
          case 2: EP2_SOF_callback(); break;
          #endif
          #ifdef EP3_SOF_callback
          case 3: EP3_SOF_callback(); break;
          #endif
          #ifdef EP4_SOF_callback
          case 4: EP4_SOF_callback(); break;
          #endif
          default: break;
        }
        break;
      case UIS_TOKEN_IN:
        switch (callIndex) {
          case 0: EP0_IN_callback(); break;
          #ifdef EP1_IN_callback
          case 1: EP1_IN_callback(); break;
          #endif
          #ifdef EP2_IN_callback
          case 2: EP2_IN_callback(); break;
          #endif
          #ifdef EP3_IN_callback
          case 3: EP3_IN_callback(); break;
          #endif
          #ifdef EP4_IN_callback
          case 4: EP4_IN_callback(); break;
          #endif
          default: break;
        }
        break;
      case UIS_TOKEN_SETUP:
        switch (callIndex) {
          case 0: EP0_SETUP_callback(); break;
          #ifdef EP1_SETUP_callback
          case 1: EP1_SETUP_callback(); break;
          #endif
          #ifdef EP2_SETUP_callback
          case 2: EP2_SETUP_callback(); break;
          #endif
          #ifdef EP3_SETUP_callback
          case 3: EP3_SETUP_callback(); break;
          #endif
          #ifdef EP4_SETUP_callback
          case 4: EP4_SETUP_callback(); break;
          #endif
          default: break;
        }
        break;
    }
    UIF_TRANSFER = 0;                       // clear interrupt flag
  }
    
  // Device mode USB bus reset
  if(UIF_BUS_RST) {
    USB_EP_init();                          // reset endpoints
    USB_DEV_AD = 0x00;                      // reset device address
    USB_INT_FG = 0x1f;                      // clear all interrupt flags
  }
    
  // USB bus suspend / wake up
  if (UIF_SUSPEND) {
    UIF_SUSPEND = 0;
    if(!(USB_MIS_ST & bUMS_SUSPEND)) USB_INT_FG = 0x1f; // clear interrupt flag
  }
}
#pragma restore
