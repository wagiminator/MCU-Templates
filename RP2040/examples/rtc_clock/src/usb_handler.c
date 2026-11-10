// ===================================================================================
// USB Handler for RP2350                                                     * v1.0 *
// ===================================================================================
//
// USB full-speed device using the USB controller of the RP2350 (dedicated pins USB_DP
// and USB_DM) with internal pull-up resistor. The USB controller is clocked with
// 48MHz by PLL_USB (see CLK_init() in system.c).
//
// The USB controller cannot access the SRAM, it only uses its own 4KB dual-port RAM
// (DPRAM). The endpoint buffers of the class drivers are located in SRAM, this
// handler copies the data to/from the DPRAM. If an endpoint has OUT and IN enabled,
// the IN buffer starts at offset 64 (see usb_descr.h). The data toggles (DATA0/DATA1)
// are managed in software.
//
// All direct accesses to the USB peripheral are made exclusively in this handler.
// The class drivers only use the endpoint functions USB_EP_init(), USB_EP_send(),
// USB_EP_receive(), USB_EP_hold() and the variable USB_RX_len.
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
static uint8_t    USB_EP0_zlp;                // zero-length packet required at the end

// ===================================================================================
// Endpoint State and DPRAM Buffers
// ===================================================================================
#define USB_EP_NUM        16                  // number of endpoints (incl. EP0)
#define USB_EP_SIZE       64                  // max packet size per endpoint
#define USB_DPRAM_BUF(ep, dir) (0x180 + ((ep) - 1) * 128 + (dir) * 64)  // EP1..15 offsets

static uint8_t* USB_EP_buf[USB_EP_NUM];       // SRAM buffer of each endpoint
static uint8_t  USB_EP_pid[USB_EP_NUM][2];    // next data PID (0: DATA0, 1: DATA1)
static uint8_t  USB_EP_type[USB_EP_NUM];      // endpoint type (0: disabled)
#define USB_DIR_OUT       0
#define USB_DIR_IN        1

// SRAM buffer of endpoint direction (IN at offset 64 if OUT and IN are used)
static uint8_t* USB_EP_bufSRAM(uint8_t ep, uint8_t dir) {
  if(!ep) return EP0_buffer;
  if((dir == USB_DIR_IN) && (USB_EP_type[ep] & 0x80)) return USB_EP_buf[ep] + 64;
  return USB_EP_buf[ep];
}

// DPRAM buffer of endpoint direction
static volatile uint8_t* USB_EP_bufDPRAM(uint8_t ep, uint8_t dir) {
  if(!ep) return USB_DPRAM->EP0_BUF;
  return (volatile uint8_t*)(USB_DPRAM_BASE + USB_DPRAM_BUF(ep, dir));
}

// Buffer control register of endpoint direction
#define USB_BUF_CTRL(ep, dir) (*((dir) ? &USB_DPRAM->EP_BUF_CTRL[ep].IN : &USB_DPRAM->EP_BUF_CTRL[ep].OUT))

// Write buffer control: set AVAILABLE bit after the other bits (clock domain crossing)
static void USB_EP_arm(uint8_t ep, uint8_t dir, uint32_t val) {
  volatile uint32_t* bc = &USB_BUF_CTRL(ep, dir);
  uint8_t i;
  val |= USB_EP_pid[ep][dir] ? USB_BUF_CTRL_DATA1_PID : USB_BUF_CTRL_DATA0_PID;
  USB_EP_pid[ep][dir] ^= 1;                   // toggle PID for next packet
  *bc = val;
  for(i = 0; i < 12; i++) __NOP();            // wait at least 3 USB clock cycles
  *bc = val | USB_BUF_CTRL_AVAIL;
}

// ===================================================================================
// Endpoint Functions (also used by the class drivers)
// ===================================================================================

// Setup endpoint (ep: 1..15, buf: endpoint buffer, out/in: enable direction)
void USB_EP_init(uint8_t ep, uint8_t type, uint8_t* buf, uint8_t out, uint8_t in) {
  uint32_t ctrl;
  if(!ep || (ep >= USB_EP_NUM)) return;       // EP0 not supported
  USB_EP_buf[ep]  = buf;
  USB_EP_type[ep] = 0x40 | (type & 3) | ((out && in) ? 0x80 : 0);
  USB_EP_pid[ep][USB_DIR_OUT] = 0;            // DATA0
  USB_EP_pid[ep][USB_DIR_IN]  = 0;
  ctrl = USB_EP_CTRL_ENABLE | USB_EP_CTRL_INT_PER_BUFFER
       | ((uint32_t)(type & 3) << USB_EP_CTRL_TYPE_Pos);
  USB_DPRAM->EP_BUF_CTRL[ep].OUT = 0;
  USB_DPRAM->EP_BUF_CTRL[ep].IN  = 0;
  USB_DPRAM->EP_CTRL[ep].OUT = out ? ctrl | USB_DPRAM_BUF(ep, USB_DIR_OUT) : 0;
  USB_DPRAM->EP_CTRL[ep].IN  = in  ? ctrl | USB_DPRAM_BUF(ep, USB_DIR_IN)  : 0;
  if(out) USB_EP_receive(ep);                 // OUT: ready (ACK)
}

// Transmit len bytes of endpoint IN buffer to host
void USB_EP_send(uint8_t ep, uint8_t len) {
  uint8_t* src = USB_EP_bufSRAM(ep, USB_DIR_IN);
  volatile uint8_t* dst = USB_EP_bufDPRAM(ep, USB_DIR_IN);
  uint8_t i;
  for(i = 0; i < len; i++) dst[i] = src[i];   // copy data into DPRAM
  USB_EP_arm(ep, USB_DIR_IN, len | USB_BUF_CTRL_FULL);
}

// Ready to receive next OUT packet (ACK)
void USB_EP_receive(uint8_t ep) {
  USB_EP_arm(ep, USB_DIR_OUT, USB_EP_SIZE);
}

// Do not receive OUT packets (NAK)
void USB_EP_hold(uint8_t ep) {
  uint32_t bc = USB_BUF_CTRL(ep, USB_DIR_OUT);
  if(bc & USB_BUF_CTRL_AVAIL) {               // buffer armed, but not used yet
    USB_BUF_CTRL(ep, USB_DIR_OUT) = bc & ~USB_BUF_CTRL_AVAIL;
    USB_EP_pid[ep][USB_DIR_OUT] ^= 1;         // undo PID toggle
  }
}

// Stall endpoint 0 (both directions, cleared automatically by the next SETUP packet)
static void USB_EP0_stall(void) {
  USB->EP_STALL_ARM = USB_EP_STALL_ARM_EP0_IN | USB_EP_STALL_ARM_EP0_OUT;
  USB_DPRAM->EP_BUF_CTRL[0].IN  = USB_BUF_CTRL_STALL;
  USB_DPRAM->EP_BUF_CTRL[0].OUT = USB_BUF_CTRL_STALL;
}

// Set (stall=1) or clear (stall=0) endpoint halt feature (addr: endpoint address)
static uint8_t USB_EP_halt(uint8_t addr, uint8_t stall) {
  uint8_t ep  = addr & 0x0f;
  uint8_t dir = (addr & 0x80) ? USB_DIR_IN : USB_DIR_OUT;
  if(!ep || (ep >= USB_EP_NUM) || !USB_EP_type[ep]) return 0;
  if(stall) USB_BUF_CTRL(ep, dir) = USB_BUF_CTRL_STALL;
  else {
    USB_BUF_CTRL(ep, dir) = 0;                // clear stall
    USB_EP_pid[ep][dir] = 0;                  // DATA0
    if(dir == USB_DIR_OUT) USB_EP_receive(ep);
  }
  return 1;
}

// Check if endpoint is halted
static uint8_t USB_EP_isHalted(uint8_t addr) {
  uint8_t ep  = addr & 0x0f;
  uint8_t dir = (addr & 0x80) ? USB_DIR_IN : USB_DIR_OUT;
  if(!ep || (ep >= USB_EP_NUM)) return 0;
  return (USB_BUF_CTRL(ep, dir) & USB_BUF_CTRL_STALL) ? 1 : 0;
}

// ===================================================================================
// Setup/Reset Endpoints
// ===================================================================================
static void USB_EP_reset(void) {
  uint8_t i;

  // Disable all endpoints
  for(i = 1; i < USB_EP_NUM; i++) {
    USB_DPRAM->EP_CTRL[i].IN  = 0;
    USB_DPRAM->EP_CTRL[i].OUT = 0;
    USB_DPRAM->EP_BUF_CTRL[i].IN  = 0;
    USB_DPRAM->EP_BUF_CTRL[i].OUT = 0;
    USB_EP_type[i] = 0;
  }
  USB_DPRAM->EP_BUF_CTRL[0].IN  = 0;
  USB_DPRAM->EP_BUF_CTRL[0].OUT = 0;
  USB->BUFF_STATUS = 0xFFFFFFFF;              // clear buffer status

  #ifdef USB_INIT_endpoints
  USB_INIT_endpoints();                       // custom EP init handler
  #endif

  USB_ENUM_OK = 0;
  USB_Config  = 0;
  USB_Addr    = 0;
}

// ===================================================================================
// USB Init Function
// ===================================================================================
void USB_init(void) {
  uint16_t i;

  // Reset USB controller and clear DPRAM
  RESET_cycle(RESETS_RESET_USBCTRL);
  for(i = 0; i < 4096 / 4; i++) ((volatile uint32_t*)USB_DPRAM_BASE)[i] = 0;

  // Setup USB device (full-speed, controller connected to the PHY, VBUS forced)
  USB->USB_MUXING = USB_USB_MUXING_TO_PHY | USB_USB_MUXING_SOFTCON;
  USB->USB_PWR    = USB_USB_PWR_VBUS_DETECT | USB_USB_PWR_VBUS_DETECT_OVERRIDE_EN;
  USB->MAIN_CTRL  = USB_MAIN_CTRL_CONTROLLER_EN;   // device mode, clear PHY_ISO
  USB->SIE_CTRL   = USB_SIE_CTRL_EP0_INT_1BUF;     // interrupt per EP0 buffer
  USB->ADDR_ENDP  = 0;                             // address 0
  USB_EP_reset();
  USB->INTE = USB_INTE_SETUP_REQ | USB_INTE_BUFF_STATUS | USB_INTE_BUS_RESET
            | USB_INTE_DEV_SUSPEND;
  NVIC_EnableIRQ(USBCTRL_IRQ_IRQn);               // enable USB interrupt
  REG_SET(USB->SIE_CTRL, USB_SIE_CTRL_PULLUP_EN); // attach (internal pull-up)
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

// Send IN packet on EP0 (len bytes from EP0_buffer, len = 0: ZLP) and prepare OUT
// buffer for the status stage
static void USB_EP0_sendIN(uint8_t len) {
  USB_EP_receive(0);                          // OUT: ready (status stage)
  USB_EP_send(0, len);                        // IN:  send packet
}

// Endpoint 0 SETUP handler
static void USB_EP0_SETUP(void) {
  uint8_t len = 0;
  USB_EP0_zlp  = 0;
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
          if(USB_SetupLen > len) {            // descriptor shorter than requested?
            USB_SetupLen = len;
            USB_EP0_zlp  = !(len & (EP0_SIZE - 1)); // multiple of EP0_SIZE -> ZLP
          }
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
    USB_EP0_stall();
  }
  else if(USB_SetupTyp & USB_REQ_TYP_IN) {    // data stage IN
    USB_SetupLen -= len;
    USB_EP0_sendIN(len);
  }
  else if(USB_SetupLen) {                     // data stage OUT
    USB_EP_receive(0);
  }
  else {                                      // no data stage: status IN (ZLP)
    USB_EP0_sendIN(0);
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

  #ifdef USB_VENDOR_IN_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_VENDOR) {
    USB_VENDOR_IN_handler();
    return;
  }
  #endif

  if((USB_SetupTyp & USB_REQ_TYP_IN) && (USB_SetupReq == USB_GET_DESCRIPTOR)) {
    if(!USB_SetupLen) {                       // all data sent?
      if(USB_EP0_zlp) {                       // terminate transfer with ZLP
        USB_EP0_zlp = 0;
        USB_EP_send(0, 0);
      }
      return;
    }
    len = USB_SetupLen >= EP0_SIZE ? EP0_SIZE : USB_SetupLen;
    USB_EP0_copyDescr(len);                   // next packet of descriptor
    USB_SetupLen -= len;
    USB_EP_send(0, len);
    return;
  }

  if(USB_SetupReq == USB_SET_ADDRESS) {       // status stage of SET_ADDRESS completed
    USB->ADDR_ENDP = USB_Addr;                // set device address
    USB_SetupReq = 0xff;
  }
}

// Endpoint 0 OUT handler
static void USB_EP0_OUT(void) {
  if(USB_SetupTyp & USB_REQ_TYP_IN) return;   // status stage of IN transfer completed

  #ifdef USB_CLASS_OUT_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS) USB_CLASS_OUT_handler();
  #endif

  #ifdef USB_VENDOR_OUT_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_VENDOR) USB_VENDOR_OUT_handler();
  #endif

  USB_SetupLen = 0;                           // only single packet data stages
  USB_EP_send(0, 0);                          // status stage: send ZLP
}


// ===================================================================================
// USB Interrupt Service Routine
// ===================================================================================
void USBCTRL_IRQ_IRQHandler(void) {
  uint32_t ints = USB->INTS;
  uint32_t status, bit;
  uint8_t  ep, i;

  // USB bus reset
  if(ints & USB_INTS_BUS_RESET) {
    USB->SIE_STATUS = USB_SIE_STATUS_BUS_RESET;     // clear flag
    #ifdef USB_RESET_handler
    USB_RESET_handler();
    #endif
    USB->ADDR_ENDP = 0;                             // address 0
    USB_EP_reset();
  }

  // USB bus suspend
  if(ints & USB_INTS_DEV_SUSPEND) {
    USB->SIE_STATUS = USB_SIE_STATUS_SUSPENDED;     // clear flag
    #ifdef USB_SUSPEND_handler
    USB_SUSPEND_handler();
    #endif
  }

  // SETUP packet received: copy into EP0_buffer, both directions start with DATA1
  if(ints & USB_INTS_SETUP_REQ) {
    USB->SIE_STATUS = USB_SIE_STATUS_SETUP_REC;     // clear flag
    for(i = 0; i < 8; i++) EP0_buffer[i] = ((volatile uint8_t*)USB_DPRAM_BASE)[i];
    USB_DPRAM->EP_BUF_CTRL[0].IN  = 0;              // cancel pending EP0 transfers
    USB_DPRAM->EP_BUF_CTRL[0].OUT = 0;
    USB->BUFF_STATUS = 3;
    USB_EP_pid[0][USB_DIR_IN]  = 1;
    USB_EP_pid[0][USB_DIR_OUT] = 1;
    USB_EP0_SETUP();
  }

  // Buffers completed (bit 2n: EPn IN, bit 2n+1: EPn OUT)
  if(ints & USB_INTS_BUFF_STATUS) {
    status = USB->BUFF_STATUS;
    for(ep = 0; ep < USB_EP_NUM; ep++) {
      bit = (uint32_t)1 << (ep << 1);

      // IN packet sent (buffer not armed -> NAK until next USB_EP_send())
      if(status & bit) {
        USB->BUFF_STATUS = bit;
        if(!ep) USB_EP0_IN();
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
      }

      // OUT packet received (buffer not re-armed -> NAK until USB_EP_receive())
      bit <<= 1;
      if(status & bit) {
        uint8_t* dst = USB_EP_bufSRAM(ep, USB_DIR_OUT);
        volatile uint8_t* src = USB_EP_bufDPRAM(ep, USB_DIR_OUT);
        USB->BUFF_STATUS = bit;
        USB_RX_len = USB_BUF_CTRL(ep, USB_DIR_OUT) & USB_BUF_CTRL_LEN_Msk;
        for(i = 0; i < USB_RX_len; i++) dst[i] = src[i];  // copy data from DPRAM
        if(!ep) { USB_EP0_OUT(); continue; }
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
      }
    }
  }
}
