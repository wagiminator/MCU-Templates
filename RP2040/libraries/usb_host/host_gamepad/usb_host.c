// ===================================================================================
// USB Host Handler for RP2040 (USBCTRL)                                      * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "usb_host.h"

// ===================================================================================
// MCU Specific Definitions (host mode memory layout of the USB DPRAM)
// ===================================================================================
#define USBH_SETUP_PKT      ((volatile uint8_t*) (USB_DPRAM_BASE + 0x000)) // SETUP packet
#define USBH_EPX_BUF_CTRL   (*(volatile uint32_t*)(USB_DPRAM_BASE + 0x080)) // EPX buffer control
#define USBH_EPX_CTRL       (*(volatile uint32_t*)(USB_DPRAM_BASE + 0x100)) // EPX control
#define USBH_EPX_DATA       ((volatile uint8_t*) (USB_DPRAM_BASE + 0x180)) // EPX data buffer
#define USBH_EPX_OFFSET     0x180                                         // data buffer offset

#define USBH_SIE_BASE       (USB_SIE_CTRL_SOF_EN | USB_SIE_CTRL_KEEP_ALIVE_EN \
                            | USB_SIE_CTRL_PULLDOWN_EN)                   // SOF, keep-alive
#define USBH_ST_ERRORS      (USB_SIE_STATUS_STALL_REC_Msk | USB_SIE_STATUS_RX_TIMEOUT_Msk \
                            | USB_SIE_STATUS_DATA_SEQ_ERROR_Msk | USB_SIE_STATUS_CRC_ERROR_Msk \
                            | USB_SIE_STATUS_BIT_STUFF_ERROR_Msk | USB_SIE_STATUS_RX_OVERFLOW_Msk)
#define USBH_ST_ALL         (USBH_ST_ERRORS | USB_SIE_STATUS_TRANS_COMPLETE_Msk \
                            | USB_SIE_STATUS_NAK_REC_Msk | USB_SIE_STATUS_ACK_REC_Msk)
#define USBH_SPEED()        ((USB->SIE_STATUS & USB_SIE_STATUS_SPEED_Msk) >> 8) // 0: none, 1: LS, 2: FS

// ===================================================================================
// Variables
// ===================================================================================
USBH_DEV_t USBH_dev;                                    // device information
uint8_t USBH_buffer[USBH_BUF_SIZE];                     // descriptor/data buffer
uint8_t USBH_RX_buffer[64] __attribute__((aligned(4))); // receive buffer
uint8_t USBH_TX_buffer[64] __attribute__((aligned(4))); // transmit buffer (SETUP, OUT)

// ===================================================================================
// Init USB Host
// ===================================================================================
void USBH_init(void) {
  uint16_t i;

  // Reset USB controller and clear DPRAM (clk_usb = 48MHz is set by the system init)
  RESET_cycle(RESETS_RESET_USBCTRL);
  for(i = 0; i < 4096 / 4; i++) ((volatile uint32_t*)USB_DPRAM_BASE)[i] = 0;

  // Setup USB host (controller connected to the PHY, VBUS detection forced)
  USB->USB_MUXING = USB_USB_MUXING_TO_PHY | USB_USB_MUXING_SOFTCON;
  USB->USB_PWR    = USB_USB_PWR_VBUS_DETECT | USB_USB_PWR_VBUS_DETECT_OVERRIDE_EN;
  USB->MAIN_CTRL  = USB_MAIN_CTRL_CONTROLLER_EN   // enable controller
                  | USB_MAIN_CTRL_HOST_NDEVICE;   // host mode
  USB->SIE_CTRL   = USBH_SIE_BASE;                // SOF, keep-alive, pull-downs
  USB->ADDR_ENDP  = 0;                            // device address 0, EP0
  USB->INTE       = 0;                            // no interrupts (polled)
  USB->SIE_STATUS = 0xFFFFFFFF;                   // clear all flags
  USBH_dev.state  = USBH_DEV_NONE;
}

// ===================================================================================
// Execute a USB Transaction (internal)
// ===================================================================================
// pid:  token PID (USB_PID_SETUP, USB_PID_IN, USB_PID_OUT)
// ep:   endpoint number
// tog:  data toggle (0: DATA0, 1: DATA1)
// len:  number of bytes to send (OUT), in USBH_TX_buffer
// nak:  number of retries on NAK (0: return immediately)
// Received data (IN) is copied to USBH_RX_buffer, length to USBH_rxlen
static uint8_t USBH_rxlen;
static uint8_t USBH_transact(uint8_t pid, uint8_t ep, uint8_t tog, uint8_t len, uint16_t nak) {
  uint32_t st, bc, sie;
  uint16_t t;
  uint8_t  i, err = 3;

  while(1) {
    // Setup device address, endpoint and EPX
    USB->ADDR_ENDP = USBH_dev.addr | ((uint32_t)ep << USB_ADDR_ENDP_ENDPOINT_Pos);
    USBH_EPX_CTRL  = USB_EP_CTRL_ENABLE | USB_EP_CTRL_INT_PER_BUFFER
                   | (ep ? USB_EP_CTRL_TYPE_INTERRUPT : USB_EP_CTRL_TYPE_CONTROL)
                   | USBH_EPX_OFFSET;
    USB->SIE_STATUS = USBH_ST_ALL;                // clear flags

    // Prepare buffer and start transaction
    if(pid == USB_PID_SETUP) {
      for(i = 0; i < 8; i++) USBH_SETUP_PKT[i] = USBH_TX_buffer[i];
      sie = USBH_SIE_BASE | USB_SIE_CTRL_SEND_SETUP;
    }
    else {
      bc = (tog ? USB_BUF_CTRL_DATA1_PID : 0) | USB_BUF_CTRL_LAST;
      if(pid == USB_PID_OUT) {
        for(i = 0; i < len; i++) USBH_EPX_DATA[i] = USBH_TX_buffer[i];
        bc |= USB_BUF_CTRL_FULL | len;
        sie = USBH_SIE_BASE | USB_SIE_CTRL_SEND_DATA;
      }
      else {
        bc |= 64;                                 // max. packet length
        sie = USBH_SIE_BASE | USB_SIE_CTRL_RECEIVE_DATA;
      }
      USBH_EPX_BUF_CTRL = bc;                     // buffer control without AVAILABLE
      DLY_us(1);                                  // (clk_sys/clk_usb synchronization)
      USBH_EPX_BUF_CTRL = bc | USB_BUF_CTRL_AVAIL;
    }
    USB->SIE_CTRL = sie;                          // set transaction type
    DLY_us(1);
    USB->SIE_CTRL = sie | USB_SIE_CTRL_START_TRANS; // start transaction

    // Wait for end of transaction
    for(t = 2000; t; t--) {                       // max. ~2ms
      st = USB->SIE_STATUS;
      if(st & (USB_SIE_STATUS_TRANS_COMPLETE_Msk | USBH_ST_ERRORS)) break;
      if(st & USB_SIE_STATUS_NAK_REC_Msk) break;
      DLY_us(1);
    }
    if(!t || !(st & USB_SIE_STATUS_TRANS_COMPLETE_Msk)) {
      USB->SIE_CTRL = USBH_SIE_BASE | USB_SIE_CTRL_STOP_TRANS; // stop transaction
      DLY_us(2);
      USB->SIE_CTRL = USBH_SIE_BASE;
      USBH_EPX_BUF_CTRL = 0;                      // release buffer
    }
    USB->SIE_STATUS = USBH_ST_ALL;                // clear flags

    // Evaluate result
    if(st & USB_SIE_STATUS_TRANS_COMPLETE_Msk) {
      if(pid == USB_PID_IN) {
        USBH_rxlen = USBH_EPX_BUF_CTRL & USB_BUF_CTRL_LEN_Msk;
        if(USBH_rxlen > 64) USBH_rxlen = 64;
        for(i = 0; i < USBH_rxlen; i++) USBH_RX_buffer[i] = USBH_EPX_DATA[i];
      }
      return USBH_OK;
    }
    if(st & USB_SIE_STATUS_STALL_REC_Msk) return USBH_ERR_STALL;
    if(st & USB_SIE_STATUS_NAK_REC_Msk) {
      if(!nak) return USBH_ERR_NAK;
      nak--;
    }
    else if(!t) return USBH_ERR_TIMEOUT;
    else if(!--err) return USBH_ERR_PROTOCOL;
    DLY_us(20);
  }
}

// ===================================================================================
// Prepare SETUP Packet in TX Buffer
// ===================================================================================
void USBH_setup(uint8_t type, uint8_t req, uint16_t value, uint16_t index, uint16_t len) {
  uint8_t* p = USBH_TX_buffer;
  *p++ = type;
  *p++ = req;
  *p++ = (uint8_t)value; *p++ = (uint8_t)(value >> 8);
  *p++ = (uint8_t)index; *p++ = (uint8_t)(index >> 8);
  *p++ = (uint8_t)len;   *p   = (uint8_t)(len >> 8);
}

// ===================================================================================
// Execute Control Transfer (SETUP packet must be prepared with USBH_setup)
// ===================================================================================
uint8_t USBH_control(uint8_t* buf) {
  uint8_t  res, tog, n, i;
  uint8_t  dir = USBH_setupReq->bRequestType & USB_REQ_TYP_IN;
  uint16_t len = USBH_setupReq->wLengthL | ((uint16_t)USBH_setupReq->wLengthH << 8);
  uint8_t  ep0 = USBH_dev.ep0size;

  // SETUP stage
  res = USBH_transact(USB_PID_SETUP, 0, 0, 8, 1000);
  if(res) return res;

  // DATA stage
  tog = 1;
  if(len && dir) {                              // IN data stage
    while(len) {
      res = USBH_transact(USB_PID_IN, 0, tog, 0, 2000);
      if(res) return res;
      tog ^= 1;
      n = USBH_rxlen;
      if(n > len) n = len;
      for(i = 0; i < n; i++) *buf++ = USBH_RX_buffer[i];
      len -= n;
      if(n < ep0) break;                        // short packet: end of data
    }
    return USBH_transact(USB_PID_OUT, 0, 1, 0, 1000); // STATUS stage: OUT ZLP DATA1
  }
  else if(len) {                                // OUT data stage
    while(len) {
      n = (len > ep0) ? ep0 : len;
      for(i = 0; i < n; i++) USBH_TX_buffer[i] = *buf++;
      res = USBH_transact(USB_PID_OUT, 0, tog, n, 1000);
      if(res) return res;
      tog ^= 1;
      len -= n;
    }
  }
  return USBH_transact(USB_PID_IN, 0, 1, 0, 1000);  // STATUS stage: IN ZLP DATA1
}

// ===================================================================================
// Poll Interrupt IN Endpoint (returns number of received bytes, 0 if none)
// ===================================================================================
uint8_t USBH_poll(void) {
  if(USBH_dev.state != USBH_DEV_READY) return 0;
  if(USBH_transact(USB_PID_IN, USBH_dev.ep, USBH_dev.tog, 0, 0)) return 0;
  USBH_dev.tog ^= 1;
  return USBH_rxlen;
}

// ===================================================================================
// Enumerate Device (internal)
// ===================================================================================
static uint8_t USBH_enumerate(void) {
  USBH_DEV_t* dev = &USBH_dev;
  uint8_t* p;
  uint16_t total, i;
  uint8_t  res, found = 0;

  // Reset device
  dev->addr    = 0;
  dev->speed   = 1;
  dev->ep0size = 8;
  dev->itf     = 0;
  dev->ep      = 0;
  dev->tog     = 0;
  dev->rlen    = 0;
  DLY_ms(100);                                  // wait for stable connection
  USB->SIE_CTRL = USBH_SIE_BASE | USB_SIE_CTRL_RESET_BUS; // bus reset
  DLY_ms(20);
  USB->SIE_CTRL = USBH_SIE_BASE;
  DLY_ms(20);
  if(!USBH_SPEED()) return USBH_ERR_TIMEOUT;    // device removed?
  dev->speed = (USBH_SPEED() == 2);             // 1: low-speed, 2: full-speed

  // Get first 8 bytes of device descriptor (for EP0 size)
  USBH_setup(USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, USB_DESCR_TYP_DEVICE << 8, 0, 8);
  res = USBH_control(USBH_buffer);
  if(res) return res;
  dev->ep0size = USBH_buffer[7];
  if(!dev->ep0size || dev->ep0size > 64) dev->ep0size = 8;

  // Set address 1
  USBH_setup(USB_REQ_TYP_OUT, USB_SET_ADDRESS, 1, 0, 0);
  res = USBH_control(0);
  if(res) return res;
  dev->addr = 1;
  DLY_ms(5);

  // Get configuration descriptor (first 9 bytes, then complete)
  USBH_setup(USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, USB_DESCR_TYP_CONFIG << 8, 0, 9);
  res = USBH_control(USBH_buffer);
  if(res) return res;
  total = USBH_buffer[2] | ((uint16_t)USBH_buffer[3] << 8);
  if(total > USBH_BUF_SIZE) total = USBH_BUF_SIZE;
  USBH_setup(USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, USB_DESCR_TYP_CONFIG << 8, 0, total);
  res = USBH_control(USBH_buffer);
  if(res) return res;

  // Parse configuration descriptor: find supported interface and its interrupt IN EP
  for(i = 0; i + 1 < total; i += p[0]) {
    p = USBH_buffer + i;
    if(p[0] < 2) break;                         // invalid descriptor
    if(p[1] == USB_DESCR_TYP_INTERF) {
      if(found) break;                          // next interface: done
      if(USBH_CLASS_match(p)) {
        found = 1;
        dev->itf = p[2];
      }
    }
    else if(found && (p[1] == USB_DESCR_TYP_HID) && (p[0] >= 9))
      dev->rlen = p[7] | ((uint16_t)p[8] << 8);
    else if(found && (p[1] == USB_DESCR_TYP_ENDP) && !dev->ep) {
      if((p[2] & 0x80) && ((p[3] & 0x03) == 0x03)) // interrupt IN endpoint?
        dev->ep = p[2] & 0x0F;
    }
  }
  if(!dev->ep) return USBH_ERR_DESCR;

  // Set configuration
  USBH_setup(USB_REQ_TYP_OUT, USB_SET_CONFIGURATION, USBH_buffer[5], 0, 0);
  res = USBH_control(0);
  if(res) return res;

  // Class specific initialization
  return USBH_CLASS_init();
}

// ===================================================================================
// Handle Connect/Disconnect (call frequently)
// ===================================================================================
void USBH_task(void) {
  if(USBH_SPEED()) {                                    // device connected?
    if(USBH_dev.state == USBH_DEV_NONE)                 // new device
      USBH_dev.state = USBH_enumerate() ? USBH_DEV_FAILED : USBH_DEV_READY;
  }
  else if(USBH_dev.state != USBH_DEV_NONE) {            // device removed
    USBH_dev.state = USBH_DEV_NONE;
    USBH_dev.addr  = 0;
  }
}
