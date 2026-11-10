// ===================================================================================
// USB Host Functions for CH554                                               * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "usb_host.h"
#include "delay.h"

#if F_CPU < 12000000
  #error USB host requires F_CPU >= 12MHz!
#endif

// ===================================================================================
// Variables
// ===================================================================================
__xdata USBH_DEV_t USBH_dev[USBH_PORTS];      // device information
__xdata uint8_t USBH_buffer[USBH_BUF_SIZE];   // descriptor/data buffer

// ===================================================================================
// Init USB Host
// ===================================================================================
void USBH_init(void) {
  IE_USB      = 0;                            // polled mode, no USB interrupt
  USB_CTRL    = bUC_HOST_MODE;                // host mode
  USB_DEV_AD  = 0;                            // device address 0
  UH_EP_MOD   = bUH_EP_TX_EN | bUH_EP_RX_EN;  // enable host endpoints (single buffer)
  UH_RX_DMA_H = (uint16_t)USBH_RX_buffer >> 8; // DMA addresses (big-endian xSFR)
  UH_RX_DMA_L = (uint8_t)(uint16_t)USBH_RX_buffer;
  UH_TX_DMA_H = (uint16_t)USBH_TX_buffer >> 8;
  UH_TX_DMA_L = (uint8_t)(uint16_t)USBH_TX_buffer;
  UH_RX_CTRL  = 0;
  UH_TX_CTRL  = 0;
  USB_CTRL    = bUC_HOST_MODE                 // host mode
              | bUC_INT_BUSY                  // pause while UIF_TRANSFER is set
              | bUC_DMA_EN;                   // enable DMA
  UH_SETUP    = bUH_SOF_EN;                   // automatic SOF
  UHOST_CTRL  = 0;                            // port disabled, pull-downs enabled
  USB_INT_FG  = 0xFF;                         // clear all flags
  USB_INT_EN  = bUIE_TRANSFER | bUIE_DETECT;  // enable flags (polled)
  USBH_dev[0].state = USBH_DEV_NONE;
}

// ===================================================================================
// Select Device on Port (set address and speed)
// ===================================================================================
static void USBH_select(uint8_t port) {
  USB_DEV_AD = (USB_DEV_AD & bUDA_GP_BIT) | USBH_dev[port].addr;
  if(USBH_dev[port].speed) USB_CTRL &= ~bUC_LOW_SPEED;
  else                     USB_CTRL |=  bUC_LOW_SPEED;
}

// ===================================================================================
// Execute a USB Transaction
// ===================================================================================
// pid_ep: token PID (upper nibble) and endpoint (lower nibble)
// tog:    data toggle (0: DATA0, 1: DATA1)
// nak:    number of retries on NAK (0: return immediately)
static uint8_t USBH_transact(uint8_t pid_ep, uint8_t tog, uint16_t nak) {
  uint8_t  res, err = 3;
  uint16_t t;
  UH_RX_CTRL = tog ? bUH_R_TOG : 0;           // expected toggle for IN
  UH_TX_CTRL = tog ? bUH_T_TOG : 0;           // toggle for SETUP/OUT
  while(1) {
    UIF_TRANSFER = 0;                         // clear flag
    UH_EP_PID    = pid_ep;                    // start transaction
    for(t = 1000; t && !UIF_TRANSFER; t--) DLY_us(1); // wait max. 1ms
    UH_EP_PID    = 0;                         // stop
    if(!UIF_TRANSFER) return USBH_ERR_TIMEOUT;
    res = U_TOG_OK;                           // toggle OK or ACK received?
    t   = USB_INT_ST & MASK_UIS_H_RES;        // handshake/data PID of device
    UIF_TRANSFER = 0;                         // clear flag (SIE is paused while set)
    if(res) return USBH_OK;
    if(t == USB_PID_STALL) return USBH_ERR_STALL;
    if(t == USB_PID_NAK) {
      if(!nak) return USBH_ERR_NAK;
      nak--;
    }
    else if(((pid_ep >> 4) != USB_PID_IN) && (t == USB_PID_ACK)) return USBH_OK;
    else if(!--err) return USBH_ERR_PROTOCOL;
    DLY_us(20);
  }
}

// ===================================================================================
// Prepare SETUP Packet in TX Buffer
// ===================================================================================
void USBH_setup(uint8_t type, uint8_t req, uint16_t value, uint16_t index, uint16_t len) {
  __xdata uint8_t* p = USBH_TX_buffer;
  *p++ = type;
  *p++ = req;
  *p++ = (uint8_t)value; *p++ = (uint8_t)(value >> 8);
  *p++ = (uint8_t)index; *p++ = (uint8_t)(index >> 8);
  *p++ = (uint8_t)len;   *p   = (uint8_t)(len >> 8);
}

// ===================================================================================
// Execute Control Transfer (SETUP packet must be prepared with USBH_setup)
// ===================================================================================
uint8_t USBH_control(uint8_t port, __xdata uint8_t* buf) {
  uint8_t  res, tog, n, i;
  uint8_t  dir = USBH_setupReq->bRequestType & USB_REQ_TYP_IN;
  uint16_t len = USBH_setupReq->wLengthL | ((uint16_t)USBH_setupReq->wLengthH << 8);
  uint8_t  ep0 = USBH_dev[port].ep0size;

  // SETUP stage
  USBH_select(port);
  UH_TX_LEN = 8;
  res = USBH_transact((uint8_t)(USB_PID_SETUP << 4), 0, 1000);
  if(res) return res;

  // DATA stage
  tog = 1;
  if(len && dir) {                            // IN data stage
    while(len) {
      res = USBH_transact((uint8_t)(USB_PID_IN << 4), tog, 2000);
      if(res) return res;
      tog ^= 1;
      n = USB_RX_LEN;
      if(n > len) n = len;
      for(i = 0; i < n; i++) *buf++ = USBH_RX_buffer[i];
      len -= n;
      if(n < ep0) break;                      // short packet: end of data
    }
    UH_TX_LEN = 0;                            // STATUS stage: OUT ZLP DATA1
    return USBH_transact((uint8_t)(USB_PID_OUT << 4), 1, 1000);
  }
  else if(len) {                              // OUT data stage
    while(len) {
      n = (len > ep0) ? ep0 : len;
      for(i = 0; i < n; i++) USBH_TX_buffer[i] = *buf++;
      UH_TX_LEN = n;
      res = USBH_transact((uint8_t)(USB_PID_OUT << 4), tog, 1000);
      if(res) return res;
      tog ^= 1;
      len -= n;
    }
  }
  return USBH_transact((uint8_t)(USB_PID_IN << 4), 1, 1000); // STATUS stage: IN ZLP DATA1
}

// ===================================================================================
// Poll Interrupt IN Endpoint (returns number of received bytes, 0 if none)
// ===================================================================================
uint8_t USBH_poll(uint8_t port) {
  if(USBH_dev[port].state != USBH_DEV_READY) return 0;
  USBH_select(port);
  if(USBH_transact((uint8_t)(USB_PID_IN << 4) | USBH_dev[port].ep, USBH_dev[port].tog, 0)) return 0;
  USBH_dev[port].tog ^= 1;
  return USB_RX_LEN;
}

// ===================================================================================
// Port Control (internal)
// ===================================================================================
#define USBH_portCtrl(p, v)   UHOST_CTRL = (v)
#define USBH_portGet(p)       (UHOST_CTRL)
#define USBH_attached(p)      (USB_MIS_ST & bUMS_DEV_ATTACH)
#define USBH_lowspeed(p)      (USB_MIS_ST & bUMS_DM_LEVEL)

// ===================================================================================
// Enumerate Device on Port
// ===================================================================================
static uint8_t USBH_enumerate(uint8_t port) {
  __xdata USBH_DEV_t* dev = &USBH_dev[port];
  __xdata uint8_t* p;
  uint16_t total, i;
  uint8_t  res, found = 0, ctrl;

  // Reset device and enable port
  dev->addr    = 0;
  dev->speed   = 1;
  dev->ep0size = 8;
  dev->itf     = 0;
  dev->ep      = 0;
  dev->tog     = 0;
  dev->rlen    = 0;
  DLY_ms(100);                                // wait for stable connection
  ctrl = USBH_portGet(port) & ~(bUH_LOW_SPEED | bUH_PORT_EN);
  USBH_portCtrl(port, ctrl | bUH_BUS_RESET);  // bus reset
  DLY_ms(15);
  USBH_portCtrl(port, ctrl);                  // end of bus reset
  DLY_us(250);
  if(!USBH_attached(port)) return USBH_ERR_TIMEOUT;
  if(USBH_lowspeed(port)) {                   // D- high at attach: low-speed device
    dev->speed = 0;
    ctrl |= bUH_LOW_SPEED;
  }
  USBH_portCtrl(port, ctrl | bUH_PORT_EN);    // enable port
  DLY_ms(20);
  UIF_DETECT = 0;

  // Get first 8 bytes of device descriptor (for EP0 size)
  USBH_setup(USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, USB_DESCR_TYP_DEVICE << 8, 0, 8);
  res = USBH_control(port, USBH_buffer);
  if(res) return res;
  dev->ep0size = USBH_buffer[7];
  if(!dev->ep0size) dev->ep0size = 8;

  // Set address (port 0 -> address 1, port 1 -> address 2)
  USBH_setup(USB_REQ_TYP_OUT, USB_SET_ADDRESS, port + 1, 0, 0);
  res = USBH_control(port, 0);
  if(res) return res;
  dev->addr = port + 1;
  DLY_ms(5);

  // Get configuration descriptor (first 9 bytes, then complete)
  USBH_setup(USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, USB_DESCR_TYP_CONFIG << 8, 0, 9);
  res = USBH_control(port, USBH_buffer);
  if(res) return res;
  total = USBH_buffer[2] | ((uint16_t)USBH_buffer[3] << 8);
  if(total > USBH_BUF_SIZE) total = USBH_BUF_SIZE;
  USBH_setup(USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, USB_DESCR_TYP_CONFIG << 8, 0, total);
  res = USBH_control(port, USBH_buffer);
  if(res) return res;

  // Parse configuration descriptor: find supported interface and its interrupt IN EP
  for(i = 0; i + 1 < total; i += p[0]) {
    p = USBH_buffer + i;
    if(p[0] < 2) break;                       // invalid descriptor
    if(p[1] == USB_DESCR_TYP_INTERF) {
      if(found) break;                        // next interface: done
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
  res = USBH_control(port, 0);
  if(res) return res;

  // Class specific initialization
  return USBH_CLASS_init(port);
}

// ===================================================================================
// Handle Connect/Disconnect (call frequently)
// ===================================================================================
void USBH_task(void) {
  uint8_t port = 0;
  #if USBH_PORTS > 1
  for(; port < USBH_PORTS; port++)
  #endif
  {
    if(USBH_attached(port)) {
      if(USBH_dev[port].state == USBH_DEV_NONE) {         // new device
        USBH_dev[port].state = USBH_enumerate(port) ? USBH_DEV_FAILED : USBH_DEV_READY;
      }
    }
    else if(USBH_dev[port].state != USBH_DEV_NONE) {      // device removed
      USBH_dev[port].state = USBH_DEV_NONE;
      USBH_portCtrl(port, USBH_portGet(port) & ~(bUH_PORT_EN | bUH_LOW_SPEED));
    }
  }
  UIF_DETECT = 0;
}
