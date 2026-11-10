// ===================================================================================
// USB Handler for CH573/CH571                                              * v1.0 *
// ===================================================================================
//
// USB full-speed device using the USB peripheral (pins PB10 = D-, PB11 = D+) with
// internal 1.5k pull-up resistor.
//
// The data toggles of endpoints 1..3 are handled by this handler (the same way as
// in the WCH examples). Endpoint buffers are used in DMA mode, if an endpoint has
// OUT and IN enabled, the IN buffer starts at offset 64 (see usb_descr.h). The
// buffers must be 4-byte aligned (RAM addresses fit into the 16-bit DMA registers).
// Endpoint 4 (which shares the buffer of endpoint 0) is not used.
//
// Ported from the USB handler of the CH32V103 template (same USB peripheral).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "usb_handler.h"

// ===================================================================================
// Variables
// ===================================================================================
volatile uint8_t  USB_SetupReq, USB_SetupTyp, USB_Config, USB_Addr, USB_ENUM_OK;
volatile uint16_t USB_SetupLen;
volatile uint8_t  USB_RX_len;
const uint8_t*    USB_pDescr;

// ===================================================================================
// Endpoint Register Access
// ===================================================================================
// (one control register per endpoint for both directions, R8_UEPn_CTRL)
#define USB_UEP_DMA(n)      (*(__IO uint16_t*)((uintptr_t)&R16_UEP0_DMA  + 4*(n)))
#define USB_UEP_T_LEN(n)    (*(__IO uint8_t* )((uintptr_t)&R8_UEP0_T_LEN + 4*(n)))
#define USB_UEP_CTRL(n)     (*(__IO uint8_t* )((uintptr_t)&R8_UEP0_CTRL  + 4*(n)))

// Set IN (T) or OUT (R) response of endpoint n
#define USB_UEP_T_RES(n, r) (USB_UEP_CTRL(n) = (USB_UEP_CTRL(n) & ~MASK_UEP_T_RES) | (r))
#define USB_UEP_R_RES(n, r) (USB_UEP_CTRL(n) = (USB_UEP_CTRL(n) & ~MASK_UEP_R_RES) | (r))

// Endpoint mode register (EP1: R8_UEP4_1_MOD, EP2/3: R8_UEP2_3_MOD)
static __IO uint8_t* USB_EP_modReg(uint8_t ep) {
  return (ep == 1) ? &R8_UEP4_1_MOD : &R8_UEP2_3_MOD;
}

// ===================================================================================
// Endpoint Functions (also used by the class drivers)
// ===================================================================================

// Setup endpoint (ep: 1..3, buf: endpoint buffer, out/in: enable direction)
void USB_EP_init(uint8_t ep, uint8_t type, uint8_t* buf, uint8_t out, uint8_t in) {
  __IO uint8_t* mod;
  uint8_t shift = ((ep == 1) || (ep == 3)) ? 4 : 0;
  uint8_t bits  = (out ? RB_UEP4_RX_EN : 0) | (in ? RB_UEP4_TX_EN : 0);
  if(!ep || (ep > 3)) return;                   // only endpoints 1..3 supported
  mod = USB_EP_modReg(ep);
  (void)type;                                   // type is defined by descriptor only
  *mod = (*mod & ~((uint8_t)0x0f << shift)) | (bits << shift);
  USB_UEP_DMA(ep)   = (uint16_t)(uintptr_t)buf; // buffer address (IN at +64 if OUT used)
  USB_UEP_T_LEN(ep) = 0;                        // nothing to send
  USB_UEP_CTRL(ep)  = UEP_T_RES_NAK             // IN: NAK, DATA0
                    | UEP_R_RES_ACK;            // OUT: ACK, DATA0
}

// Transmit len bytes of endpoint IN buffer to host
void USB_EP_send(uint8_t ep, uint8_t len) {
  USB_UEP_T_LEN(ep) = len;
  USB_UEP_T_RES(ep, UEP_T_RES_ACK);
}

// Ready to receive next OUT packet (ACK)
void USB_EP_receive(uint8_t ep) {
  USB_UEP_R_RES(ep, UEP_R_RES_ACK);
}

// Do not receive OUT packets (NAK)
void USB_EP_hold(uint8_t ep) {
  USB_UEP_R_RES(ep, UEP_R_RES_NAK);
}

// Set (stall=1) or clear (stall=0) endpoint halt feature (addr: endpoint address)
static uint8_t USB_EP_halt(uint8_t addr, uint8_t stall) {
  uint8_t ep = addr & 0x0f;
  if(!ep || (ep > 3)) return 0;
  if(addr & 0x80) {
    if(stall) USB_UEP_T_RES(ep, UEP_T_RES_STALL);
    else      USB_UEP_CTRL(ep) = (USB_UEP_CTRL(ep) & ~(RB_UEP_T_TOG | MASK_UEP_T_RES))
                               | UEP_T_RES_NAK;     // DATA0, NAK
  }
  else {
    if(stall) USB_UEP_R_RES(ep, UEP_R_RES_STALL);
    else      USB_UEP_CTRL(ep) = (USB_UEP_CTRL(ep) & ~(RB_UEP_R_TOG | MASK_UEP_R_RES))
                               | UEP_R_RES_ACK;     // DATA0, ACK
  }
  return 1;
}

// Check if endpoint is halted
static uint8_t USB_EP_isHalted(uint8_t addr) {
  uint8_t ep = addr & 0x0f;
  if(addr & 0x80) return (USB_UEP_CTRL(ep) & MASK_UEP_T_RES) == UEP_T_RES_STALL;
  return (USB_UEP_CTRL(ep) & MASK_UEP_R_RES) == UEP_R_RES_STALL;
}

// ===================================================================================
// Setup/Reset Endpoints
// ===================================================================================
static void USB_EP_reset(void) {
  R16_UEP0_DMA  = (uint16_t)(uintptr_t)EP0_buffer;
  R8_UEP0_T_LEN = 0;
  R8_UEP0_CTRL  = UEP_T_RES_NAK | UEP_R_RES_ACK;

  #ifdef USB_INIT_endpoints
  USB_INIT_endpoints();                     // custom EP init handler
  #endif

  USB_ENUM_OK = 0;
  USB_Config  = 0;
  USB_Addr    = 0;
}

// ===================================================================================
// USB Init Function
// ===================================================================================
void USB_init(void) {
  // Reset and setup USB device
  R8_USB_CTRL       = RB_UC_RESET_SIE | RB_UC_CLR_ALL;
  DLY_us(10);
  R8_USB_CTRL       = 0x00;
  R8_UEP4_1_MOD     = 0x00;                 // all endpoints disabled (EP4 not used)
  R8_UEP2_3_MOD     = 0x00;
  USB_EP_reset();
  R8_USB_DEV_AD     = 0x00;
  R8_USB_CTRL       = RB_UC_DEV_PU_EN | RB_UC_INT_BUSY | RB_UC_DMA_EN;
  R16_PIN_ANALOG_IE |= RB_PIN_USB_IE | RB_PIN_USB_DP_PU;  // USB I/O and D+ pull-up
  R8_USB_INT_FG     = 0xff;
  R8_UDEV_CTRL      = RB_UD_PD_DIS | RB_UD_PORT_EN;
  R8_USB_INT_EN     = RB_UIE_SUSPEND | RB_UIE_BUS_RST | RB_UIE_TRANSFER;
  NVIC_EnableIRQ(USB_IRQn);
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
// Endpoint EP0 Handlers
// ===================================================================================

// Endpoint 0 SETUP handler
static void USB_EP0_SETUP(void) {
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
            if(USB_SetupBuf->wIndexL == 0) {
              USB_pDescr = USB_REPORT_DESCR;
              len = USB_REPORT_DESCR_LEN;
            }
            else len = 0xff;
            break;
          #endif

          #ifdef USB_HID_DESCR_PTR
          case USB_DESCR_TYP_HID:
            if(USB_SetupBuf->wIndexL == 0) {
              USB_pDescr = USB_HID_DESCR_PTR;
              len = USB_HID_DESCR_PTR[0];
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
        EP0_buffer[0] = 0x00;                 // only alternate setting 0
        if(USB_SetupLen > 1) USB_SetupLen = 1;
        len = USB_SetupLen;
        break;

      case USB_SET_INTERFACE:
        break;

      case USB_GET_STATUS:
        EP0_buffer[0] = 0x00;
        EP0_buffer[1] = 0x00;
        if((USB_SetupTyp & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
          EP0_buffer[0] = USB_EP_isHalted(USB_SetupBuf->wIndexL);
        if(USB_SetupLen > 2) USB_SetupLen = 2;
        len = USB_SetupLen;
        break;

      case USB_CLEAR_FEATURE:
      case USB_SET_FEATURE:
        if((USB_SetupTyp & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE) {
          if((USB_SetupBuf->wValueL != 0x01) || !(((uint8_t*)&CfgDescr)[7] & 0x20))
            len = 0xff;                       // only remote wakeup, if supported
        }
        else if(((USB_SetupTyp & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
             && (USB_SetupBuf->wValueL == 0x00)) {
          if(!USB_EP_halt(USB_SetupBuf->wIndexL, USB_SetupReq == USB_SET_FEATURE))
            len = 0xff;
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

  if(len == 0xff) {                           // request not supported -> STALL
    USB_SetupReq = 0xff;
    R8_UEP0_CTRL  = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;
  }
  else if(USB_SetupTyp & USB_REQ_TYP_IN) {    // data stage IN (DATA1 first)
    USB_SetupLen -= len;
    R8_UEP0_T_LEN = len;
    R8_UEP0_CTRL  = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
  }
  else if(USB_SetupLen) {                     // data stage OUT (DATA1 first)
    R8_UEP0_CTRL  = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
  }
  else {                                      // no data stage: status IN (ZLP)
    R8_UEP0_T_LEN = 0;
    R8_UEP0_CTRL  = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
  }
}

// Endpoint 0 IN handler
static void USB_EP0_IN(void) {
  uint8_t len;

  #ifdef USB_CLASS_IN_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS) {
    USB_CLASS_IN_handler();
    return;
  }
  #endif

  if((USB_SetupTyp & USB_REQ_TYP_IN) && (USB_SetupReq == USB_GET_DESCRIPTOR)) {
    len = USB_SetupLen >= EP0_SIZE ? EP0_SIZE : USB_SetupLen;
    USB_EP0_copyDescr(len);                   // next packet of descriptor
    USB_SetupLen -= len;
    R8_UEP0_T_LEN = len;
    R8_UEP0_CTRL ^= RB_UEP_T_TOG;
    return;
  }

  if(USB_SetupReq == USB_SET_ADDRESS)         // status stage of SET_ADDRESS completed
    R8_USB_DEV_AD = (R8_USB_DEV_AD & RB_UDA_GP_BIT) | USB_Addr;

  R8_UEP0_CTRL = UEP_T_RES_NAK | RB_UEP_R_TOG | UEP_R_RES_ACK; // transfer completed
}

// Endpoint 0 OUT handler
static void USB_EP0_OUT(void) {
  if(USB_SetupTyp & USB_REQ_TYP_IN) {         // status stage of IN transfer completed
    R8_UEP0_CTRL = UEP_T_RES_NAK | RB_UEP_R_TOG | UEP_R_RES_ACK;
    return;
  }

  #ifdef USB_CLASS_OUT_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS) USB_CLASS_OUT_handler();
  #endif

  #ifdef USB_VENDOR_OUT_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_VENDOR) USB_VENDOR_OUT_handler();
  #endif

  USB_SetupLen = 0;                           // only single packet data stages
  R8_UEP0_T_LEN = 0;                      // status stage: send ZLP (DATA1)
  R8_UEP0_CTRL  = RB_UEP_T_TOG | UEP_T_RES_ACK | UEP_R_RES_NAK;
}

// ===================================================================================
// USB Interrupt Service Routine
// ===================================================================================
void USB_IRQHandler(void) __attribute__((interrupt));
void USB_IRQHandler(void) {
  uint8_t intflag = R8_USB_INT_FG;
  uint8_t intst   = R8_USB_INT_ST;

  // USB transfer completed interrupt
  if(intflag & RB_UIF_TRANSFER) {
    uint8_t ep = intst & MASK_UIS_ENDP;
    switch(intst & MASK_UIS_TOKEN) {

      case UIS_TOKEN_SETUP:
        USB_EP0_SETUP();
        break;

      case UIS_TOKEN_IN:
        if(!ep) {
          USB_EP0_IN();
          break;
        }
        if(ep > 3) break;
        USB_UEP_CTRL(ep) = ((USB_UEP_CTRL(ep) ^ RB_UEP_T_TOG)   // next DATAx
                         & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
        switch(ep) {
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
        if(!(intst & RB_UIS_TOG_OK)) break;        // ignore out-of-sync packets
        USB_RX_len = (uint8_t)R8_USB_RX_LEN;
        if(!ep) {
          USB_EP0_OUT();
          break;
        }
        if(ep > 3) break;
        USB_UEP_CTRL(ep) = ((USB_UEP_CTRL(ep) ^ RB_UEP_R_TOG)   // next DATAx
                         & ~MASK_UEP_R_RES) | UEP_R_RES_NAK;
        switch(ep) {
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
          default: USB_EP_receive(ep); break;
        }
        break;

      default:
        break;
    }
    R8_USB_INT_FG = RB_UIF_TRANSFER;
  }

  // USB bus suspend or wakeup event interrupt
  if(intflag & RB_UIF_SUSPEND) {
    R8_USB_INT_FG = RB_UIF_SUSPEND;
    #ifdef USB_SUSPEND_handler
    if(R8_USB_MIS_ST & RB_UMS_SUSPEND) USB_SUSPEND_handler();
    #endif
  }

  // USB bus reset event interrupt
  if(intflag & RB_UIF_BUS_RST) {
    #ifdef USB_RESET_handler
    USB_RESET_handler();
    #endif
    USB_EP_reset();
    R8_USB_DEV_AD = 0;
    R8_USB_INT_FG = 0xff;
  }
}
