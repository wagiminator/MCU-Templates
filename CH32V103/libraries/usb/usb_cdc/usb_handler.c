// ===================================================================================
// USB Handler for CH32V103                                                   * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "usb_handler.h"

// ===================================================================================
// Variables
// ===================================================================================
volatile uint16_t USB_SetupLen;
volatile uint8_t  USB_SetupReq, USB_SetupTyp, USB_Config, USB_Addr, USB_ENUM_OK;
const uint8_t*    USB_pDescr;

// ===================================================================================
// Setup/Reset Endpoints
// ===================================================================================
void USB_EP_init(void) {
  USBHD->UEP0_DMA   = (uint32_t)EP0_buffer; // EP0 data transfer address
  USBHD->UEP0_CTRL  = UEP_R_RES_ACK         // EP0 Manual flip, OUT transaction returns ACK
                    | UEP_T_RES_NAK;        // EP0 IN transaction returns NAK
  USBHD->UEP0_T_LEN = 0;                    // Nothing to send now

  #ifdef USB_INIT_endpoints
  USB_INIT_endpoints();                     // Custom EP init handler
  #endif

  USB_ENUM_OK = 0;
  USB_Config  = 0;
  USB_Addr    = 0;
}

// ===================================================================================
// USB Init Function
// ===================================================================================
void USB_init(void) {
  // Setup USB module
  #if USB_VDD > 0
  EXTEN->EXTEN_CTR |= EXTEN_USBHD_IO_EN | EXTEN_USB_5V_SEL;
  #else
  EXTEN->EXTEN_CTR |= EXTEN_USBHD_IO_EN;    // Enable USB I/O
  #endif

  #if   F_CPU == 48000000
  RCC->CFGR0 |=  RCC_USBPRE;                // No prescaler
  #elif F_CPU == 72000000
  RCC->CFGR0 &= ~RCC_USBPRE;                // Prescaler 1.5
  #else
  #error Unsupported system clock frequency for USB operation
  #endif

  RCC->APB2PCENR |= RCC_IOPAEN;             // Enable port A
  RCC->AHBPCENR  |= RCC_USBHD;              // USBHD clock enable

  // Setup endpoints
  USB_EP_init();                            // Setup endpoints

  // Init USB device
  USBHD->INT_EN    = UIE_SUSPEND            // Enable device hang interrupt
                   | UIE_TRANSFER           // Enable USB transfer completion interrupt
                   | UIE_BUS_RST;           // Enable device mode USB bus reset interrupt
  USBHD->CTRL      = UC_DEV_PU_EN           // USB internal pull-up enable
                   | UC_INT_BUSY            // Return NAK if USB INT flag not clear
                   | UC_DMA_EN;             // DMA enable
  USBHD->UDEV_CTRL = UD_PD_DIS              // Disable UDP/UDM pulldown resistor
                   | UD_PORT_EN;            // Enable port, full-speed
  NVIC_EnableIRQ(USBHD_IRQn);               // Enable USB interrupts
}

// ===================================================================================
// Endpoint Copy Function
// ===================================================================================
// Copy descriptor *USB_pDescr to Ep0
void USB_EP0_copyDescr(uint8_t len) {
  uint8_t* tgt = EP0_buffer;
  while(len--) *tgt++ = *USB_pDescr++;
}

// ===================================================================================
// Endpoint Handler
// ===================================================================================

// Endpoint 0 SETUP handler
void USB_EP0_SETUP(void) {
  uint8_t len = 0;
  USB_SetupLen = ((uint16_t)USB_SetupBuf->wLengthH<<8) | (USB_SetupBuf->wLengthL);
  USB_SetupReq = USB_SetupBuf->bRequest;
  USB_SetupTyp = USB_SetupBuf->bRequestType;

  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_STANDARD) {
    switch(USB_SetupReq) {
      case USB_GET_DESCRIPTOR:
        switch(USB_SetupBuf->wValueH) {

          case USB_DESCR_TYP_DEVICE:
            USB_pDescr = (uint8_t*)&DevDescr;
            len = sizeof(DevDescr);
            break;

          case USB_DESCR_TYP_CONFIG:
            USB_pDescr = (uint8_t*)&CfgDescr;
            len = sizeof(CfgDescr);
            break;

          case USB_DESCR_TYP_STRING:
            switch(USB_SetupBuf->wValueL) {
              case 0:   USB_pDescr = USB_STR_DESCR_i0; break;
              case 1:   USB_pDescr = USB_STR_DESCR_i1; break;
              case 2:   USB_pDescr = USB_STR_DESCR_i2; break;
              case 3:   USB_pDescr = USB_STR_DESCR_i3; break;
              #ifdef USB_STR_DESCR_i4
              case 4:   USB_pDescr = USB_STR_DESCR_i4; break;
              #endif
              #ifdef USB_STR_DESCR_i5
              case 5:   USB_pDescr = USB_STR_DESCR_i5; break;
              #endif
              #ifdef USB_STR_DESCR_i6
              case 6:   USB_pDescr = USB_STR_DESCR_i6; break;
              #endif
              #ifdef USB_STR_DESCR_i7
              case 7:   USB_pDescr = USB_STR_DESCR_i7; break;
              #endif
              #ifdef USB_STR_DESCR_i8
              case 8:   USB_pDescr = USB_STR_DESCR_i8; break;
              #endif
              #ifdef USB_STR_DESCR_i9
              case 9:   USB_pDescr = USB_STR_DESCR_i9; break;
              #endif
              #ifdef USB_STR_DESCR_ixee
              case 0xee:  USB_pDescr = USB_STR_DESCR_ixee; break;
              #endif
              default:  USB_pDescr = USB_STR_DESCR_ix; break;
            }
            len = USB_pDescr[0];
            break;

          #ifdef USB_REPORT_DESCR
          case USB_DESCR_TYP_REPORT:
            if(USB_SetupBuf->wValueL == 0) {
              USB_pDescr = USB_REPORT_DESCR;
              len = USB_REPORT_DESCR_LEN;
            }
            else len = 0xff;
            break;
          #endif

          default:
            len = 0xff;
            break;
        }

        if(len != 0xff) {
          if(USB_SetupLen > len) USB_SetupLen = len;
          len = USB_SetupLen >= EP0_SIZE ? EP0_SIZE : USB_SetupLen;
          USB_EP0_copyDescr(len);
        }
        break;

      case USB_SET_ADDRESS:
        USB_Addr = USB_SetupBuf->wValueL;
        break;

      case USB_GET_CONFIGURATION:
        EP0_buffer[0] = USB_Config;
        if(USB_SetupLen > 1) USB_SetupLen = 1;
        len = USB_SetupLen;
        break;

      case USB_SET_CONFIGURATION:
        USB_Config  = USB_SetupBuf->wValueL;
        USB_ENUM_OK = 1;
        break;

      case USB_GET_INTERFACE:
        break;

      case USB_SET_INTERFACE:
        break;

      case USB_GET_STATUS:
        EP0_buffer[0] = 0x00;
        EP0_buffer[1] = 0x00;
        if(USB_SetupLen > 2) USB_SetupLen = 2;
        len = USB_SetupLen;
        break;

      case USB_CLEAR_FEATURE:
        if((USB_SetupTyp & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE ) {
          if(USB_SetupBuf->wValueL == 0x01) {
            if(((uint8_t*)&CfgDescr)[7] & 0x20) {
              // wake up
            }
            else len = 0xff;
          }
          else len = 0xff;
        }
        else if((USB_SetupTyp & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) {
          switch(USB_SetupBuf->wIndexL) {
            #ifdef EP1_OUT_callback
            case 0x01:
              USBHD->UEP1_CTRL = (USBHD->UEP1_CTRL & ~(UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
              break;
            #endif
            #ifdef EP1_IN_callback
            case 0x81:
              USBHD->UEP1_CTRL = (USBHD->UEP1_CTRL & ~(UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
              break;
            #endif
            #ifdef EP2_OUT_callback
            case 0x02:
              USBHD->UEP2_CTRL = (USBHD->UEP2_CTRL & ~(UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
              break;
            #endif
            #ifdef EP2_IN_callback
            case 0x82:
              USBHD->UEP2_CTRL = (USBHD->UEP2_CTRL & ~(UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
              break;
            #endif
            #ifdef EP3_OUT_callback
            case 0x03:
              USBHD->UEP3_CTRL = (USBHD->UEP3_CTRL & ~(UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
              break;
            #endif
            #ifdef EP3_IN_callback
            case 0x83:
              USBHD->UEP3_CTRL = (USBHD->UEP3_CTRL & ~(UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
              break;
            #endif
            #ifdef EP4_OUT_callback
            case 0x04:
              USBHD->UEP4_CTRL = (USBHD->UEP4_CTRL & ~(UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
              break;
            #endif
            #ifdef EP4_IN_callback
            case 0x84:
              USBHD->UEP4_CTRL = (USBHD->UEP4_CTRL & ~(UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
              break;
            #endif
            #ifdef EP5_OUT_callback
            case 0x05:
              USBHD->UEP5_CTRL = (USBHD->UEP5_CTRL & ~(UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
              break;
            #endif
            #ifdef EP5_IN_callback
            case 0x85:
              USBHD->UEP5_CTRL = (USBHD->UEP5_CTRL & ~(UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
              break;
            #endif
            #ifdef EP6_OUT_callback
            case 0x06:
              USBHD->UEP6_CTRL = (USBHD->UEP6_CTRL & ~(UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
              break;
            #endif
            #ifdef EP6_IN_callback
            case 0x86:
              USBHD->UEP6_CTRL = (USBHD->UEP6_CTRL & ~(UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
              break;
            #endif
            #ifdef EP7_OUT_callback
            case 0x07:
              USBHD->UEP7_CTRL = (USBHD->UEP7_CTRL & ~(UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
              break;
            #endif
            #ifdef EP7_IN_callback
            case 0x87:
              USBHD->UEP7_CTRL = (USBHD->UEP7_CTRL & ~(UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
              break;
            #endif
            default:
              len = 0xff;
              break;
          }
        }
        else len = 0xff;
        break;

      case USB_SET_FEATURE:
        if((USB_SetupTyp & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE) {
          if(USB_SetupBuf->wValueL == 0x01) {
            if(!(((uint8_t*)&CfgDescr)[7] & 0x20)) len = 0xff;
          }
          else len = 0xff;
        }
        else if((USB_SetupTyp & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) {
          if(USB_SetupBuf->wValueL == 0x00) {
            switch(USB_SetupBuf->wIndexL) {
              #ifdef EP1_OUT_callback
              case 0x01:
                USBHD->UEP1_CTRL = (USBHD->UEP1_CTRL & ~UEP_R_TOG) | UEP_R_RES_STALL;
                break;
              #endif
              #ifdef EP1_IN_callback
              case 0x81:
                USBHD->UEP1_CTRL = (USBHD->UEP1_CTRL & ~UEP_T_TOG) | UEP_T_RES_STALL;
                break;
              #endif
              #ifdef EP2_OUT_callback
              case 0x02:
                USBHD->UEP2_CTRL = (USBHD->UEP2_CTRL & ~UEP_R_TOG) | UEP_R_RES_STALL;
                break;
              #endif
              #ifdef EP2_IN_callback
              case 0x82:
                USBHD->UEP2_CTRL = (USBHD->UEP2_CTRL & ~UEP_T_TOG) | UEP_T_RES_STALL;
                break;
              #endif
              #ifdef EP3_OUT_callback
              case 0x03:
                USBHD->UEP3_CTRL = (USBHD->UEP3_CTRL & ~UEP_R_TOG) | UEP_R_RES_STALL;
                break;
              #endif
              #ifdef EP3_IN_callback
              case 0x83:
                USBHD->UEP3_CTRL = (USBHD->UEP3_CTRL & ~UEP_T_TOG) | UEP_T_RES_STALL;
                break;
              #endif
              #ifdef EP4_OUT_callback
              case 0x04:
                USBHD->UEP4_CTRL = (USBHD->UEP4_CTRL & ~UEP_R_TOG) | UEP_R_RES_STALL;
                break;
              #endif
              #ifdef EP4_IN_callback
              case 0x84:
                USBHD->UEP4_CTRL = (USBHD->UEP4_CTRL & ~UEP_T_TOG) | UEP_T_RES_STALL;
                break;
              #endif
              #ifdef EP5_OUT_callback
              case 0x05:
                USBHD->UEP5_CTRL = (USBHD->UEP5_CTRL & ~UEP_R_TOG) | UEP_R_RES_STALL;
                break;
              #endif
              #ifdef EP5_IN_callback
              case 0x85:
                USBHD->UEP5_CTRL = (USBHD->UEP5_CTRL & ~UEP_T_TOG) | UEP_T_RES_STALL;
                break;
              #endif
              #ifdef EP6_OUT_callback
              case 0x06:
                USBHD->UEP6_CTRL = (USBHD->UEP6_CTRL & ~UEP_R_TOG) | UEP_R_RES_STALL;
                break;
              #endif
              #ifdef EP6_IN_callback
              case 0x86:
                USBHD->UEP6_CTRL = (USBHD->UEP6_CTRL & ~UEP_T_TOG) | UEP_T_RES_STALL;
                break;
              #endif
              #ifdef EP7_OUT_callback
              case 0x07:
                USBHD->UEP7_CTRL = (USBHD->UEP7_CTRL & ~UEP_R_TOG) | UEP_R_RES_STALL;
                break;
              #endif
              #ifdef EP7_IN_callback
              case 0x87:
                USBHD->UEP7_CTRL = (USBHD->UEP7_CTRL & ~UEP_T_TOG) | UEP_T_RES_STALL;
                break;
              #endif
              default:
                len = 0xff;
                break;
            }
          }
          else len = 0xff;
        }
        else len = 0xff;
        break;

      default:
        len = 0xff;
        break;
    }
  }

  #ifdef USB_CLASS_SETUP_handler
  else if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS) {
    len = USB_CLASS_SETUP_handler();
  }
  #endif

  #ifdef USB_VENDOR_SETUP_handler
  else if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_VENDOR) {
    len = USB_VENDOR_SETUP_handler();
  }
  #endif

  else len = 0xff;

  if(len == 0xff) {
    USB_SetupReq = 0xff;
    USBHD->UEP0_CTRL = UEP_R_TOG | UEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;
  }
  else {
    USB_SetupLen -= len;
    USBHD->UEP0_T_LEN = len;
    USBHD->UEP0_CTRL  = UEP_R_TOG | UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
  }
}

// Endpoint 0 IN handler
void USB_EP0_IN(void) {
  uint8_t len;

  #ifdef USB_CLASS_IN_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS) {
    USB_CLASS_IN_handler();
    return;
  }
  #endif

  #ifdef USB_VENDOR_IN_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_VENDOR) {
    USB_VENDOR_IN_handler();
    return;
  }
  #endif

  switch(USB_SetupReq) {
    case USB_GET_DESCRIPTOR:
      len = USB_SetupLen >= EP0_SIZE ? EP0_SIZE : USB_SetupLen;
      USB_EP0_copyDescr(len);       
      USB_SetupLen -= len;
      USBHD->UEP0_T_LEN = len;
      USBHD->UEP0_CTRL ^= UEP_T_TOG;
      break;

    case USB_SET_ADDRESS:
      USBHD->DEV_AD    = (USBHD->DEV_AD & UDA_GP_BIT) | USB_Addr;
      USBHD->UEP0_CTRL = UEP_T_RES_NAK | UEP_R_TOG | UEP_R_RES_ACK;
      break;

    default:
      USBHD->UEP0_CTRL = UEP_T_RES_NAK | UEP_R_TOG | UEP_R_RES_ACK;
      break;
  }
}

// Endpoint 0 OUT handler
void USB_EP0_OUT(void) {
  #ifdef USB_CLASS_OUT_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS) {
    USB_CLASS_OUT_handler();
    return;
  }
  #endif

  #ifdef USB_VENDOR_OUT_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_VENDOR) {
    USB_VENDOR_OUT_handler();
    return;
  }
  #endif

  USBHD->UEP0_CTRL = UEP_T_TOG | UEP_T_RES_ACK | UEP_R_RES_ACK;
}

// ===================================================================================
// USB Interrupt Service Routine
// ===================================================================================
void USBHD_IRQHandler(void) __attribute__((interrupt));
void USBHD_IRQHandler(void) {
  uint8_t intflag = USBHD->INT_FG;
  uint8_t intst   = USBHD->INT_ST;

  // USB transfer completed interrupt
  if(intflag & UIF_TRANSFER) {
    uint8_t callIndex = intst & MASK_UIS_ENDP;
    switch(intst & MASK_UIS_TOKEN) {

      case UIS_TOKEN_SETUP:
        EP0_SETUP_callback();
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
          #ifdef EP5_IN_callback
          case 5: EP5_IN_callback(); break;
          #endif
          #ifdef EP6_IN_callback
          case 6: EP6_IN_callback(); break;
          #endif
          #ifdef EP7_IN_callback
          case 7: EP7_IN_callback(); break;
          #endif
          default: break;
        }
        break;

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
          #ifdef EP5_OUT_callback
          case 5: EP5_OUT_callback(); break;
          #endif
          #ifdef EP6_OUT_callback
          case 6: EP6_OUT_callback(); break;
          #endif
          #ifdef EP7_OUT_callback
          case 7: EP7_OUT_callback(); break;
          #endif
          default: break;
        }
        break;
    }
    USBHD->INT_FG = UIF_TRANSFER;
  }

  // USB bus suspend / wake up
  if(intflag & UIF_SUSPEND) {
    USBHD->INT_FG = UIF_SUSPEND;
    #ifdef USB_SUSPEND_handler
    if(USBHD->MIS_ST & UMS_SUSPEND) USB_SUSPEND_handler();
    #endif
  }

  // Device mode USB bus reset
  if(intflag & UIF_BUS_RST) {
    USB_EP_init();                          // reset endpoints
    USBHD->DEV_AD = 0x00;                   // reset device address
    USBHD->INT_FG = 0xff;                   // clear interrupt flags
  }
}
