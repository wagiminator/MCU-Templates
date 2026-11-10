// ===================================================================================
// USB Host Handler for SAMD21 (USB in host mode)                             * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "usb_host.h"

// ===================================================================================
// MCU Specific Definitions
// ===================================================================================
#define USBH_PIPE_CTRL      0                         // pipe for control transfers
#define USBH_PIPE_INT       1                         // pipe for interrupt IN transfers
#define USBH_PTYPE_CONTROL  1                         // pipe type: control
#define USBH_PTYPE_INT      4                         // pipe type: interrupt
#define USBH_TOKEN_SETUP    0                         // pipe token: SETUP
#define USBH_TOKEN_IN       1                         // pipe token: IN
#define USBH_TOKEN_OUT      2                         // pipe token: OUT
#define USBH_SIZE_CODE(n)   ((n) <= 8 ? 0 : (n) <= 16 ? 1 : (n) <= 32 ? 2 : 3)
#define USBH_sync()         while(USB->HOST.SYNCBUSY.reg)

// USB host needs an accurate 48MHz clock (DFLL48M locked to the 32.768kHz crystal)
#if SYS_USE_XOSC32K == 0
  #warning USB host needs an accurate clock: use a 32.768kHz crystal (SYS_USE_XOSC32K = 1)
#endif

// Pipe descriptor table in SRAM (2 pipes, bank 0 only)
static UsbHostDescriptor USBH_pipe[2] __attribute__((aligned(4)));

// ===================================================================================
// Variables
// ===================================================================================
USBH_DEV_t USBH_dev;                                    // device information
uint8_t USBH_buffer[USBH_BUF_SIZE];                     // descriptor/data buffer
uint8_t USBH_RX_buffer[64] __attribute__((aligned(4))); // receive buffer
uint8_t USBH_TX_buffer[64] __attribute__((aligned(4))); // transmit buffer (SETUP, OUT)
static volatile uint8_t USBH_connected;                 // device connected flag

// ===================================================================================
// Init USB Host
// ===================================================================================
void USBH_init(void) {
  uint32_t cal, transn, transp, trim;

  // Setup USB pins PA24 (D-) and PA25 (D+), peripheral function G
  PORT->Group[0].PMUX[24 >> 1].reg = PORT_PMUX_PMUXE(6) | PORT_PMUX_PMUXO(6);
  PORT->Group[0].PINCFG[24].reg    = PORT_PINCFG_PMUXEN;
  PORT->Group[0].PINCFG[25].reg    = PORT_PINCFG_PMUXEN;

  // Enable USB module clocks (48MHz from DFLL48M)
  PM->AHBMASK.reg  |= PM_AHBMASK_USB;
  PM->APBBMASK.reg |= PM_APBBMASK_USB;
  #if F_CPU == 48000000
  GCLK_setCH(USB_GCLK_ID, GCLK_SYS);            // GCLK0 (48MHz) -> USB
  #else
  GCLK_setGEN(GCLK_48M, GCLK_SOURCE_DFLL48M, 1);// GCLK4 = DFLL48M (48MHz)
  GCLK_setCH(USB_GCLK_ID, GCLK_48M);            // GCLK4 -> USB
  #endif

  // Reset USB module
  USB->HOST.CTRLA.reg = USB_CTRLA_SWRST;
  while(USB->HOST.SYNCBUSY.reg & USB_SYNCBUSY_SWRST);

  // Load pad calibration values from NVM (use defaults if not programmed)
  cal    = *(uint32_t*)USB_FUSES_TRANSN_ADDR;
  transn = (cal & USB_FUSES_TRANSN_Msk) >> USB_FUSES_TRANSN_Pos;
  transp = (cal & USB_FUSES_TRANSP_Msk) >> USB_FUSES_TRANSP_Pos;
  trim   = (cal & USB_FUSES_TRIM_Msk)   >> USB_FUSES_TRIM_Pos;
  if(transn == 0x1f) transn = 5;
  if(transp == 0x1f) transp = 29;
  if(trim   == 0x07) trim   = 3;
  USB->HOST.PADCAL.reg = USB_PADCAL_TRANSN(transn) | USB_PADCAL_TRANSP(transp)
                       | USB_PADCAL_TRIM(trim);

  // Setup USB host
  USB->HOST.DESCADD.reg = (uint32_t)USBH_pipe;  // pipe descriptor table
  USB->HOST.CTRLA.reg   = USB_CTRLA_MODE_HOST | USB_CTRLA_RUNSTDBY;
  USB->HOST.CTRLB.reg   = USB_HOST_CTRLB_SPDCONF_NORMAL | USB_HOST_CTRLB_VBUSOK;
  USB->HOST.CTRLA.reg  |= USB_CTRLA_ENABLE;     // enable USB module
  while(USB->HOST.SYNCBUSY.reg & USB_SYNCBUSY_ENABLE);
  USB->HOST.INTFLAG.reg = 0xffff;               // clear all flags (no interrupts used)
  USBH_connected = 0;
  USBH_dev.state = USBH_DEV_NONE;
}

// ===================================================================================
// Execute a USB Transaction (internal)
// ===================================================================================
// pid:  token PID (USB_PID_SETUP, USB_PID_IN, USB_PID_OUT)
// ep:   endpoint number (0: control pipe, else: interrupt IN pipe)
// tog:  data toggle (0: DATA0, 1: DATA1)
// len:  number of bytes to send (OUT), in USBH_TX_buffer
// nak:  number of retries on NAK (the hardware repeats NAKed transactions; the
//       transaction is aborted after about (nak + 1) milliseconds)
// Received data (IN) is in USBH_RX_buffer, length in USBH_rxlen
static uint8_t USBH_rxlen;
static uint8_t USBH_transact(uint8_t pid, uint8_t ep, uint8_t tog, uint8_t len, uint16_t nak) {
  uint8_t  p = ep ? USBH_PIPE_INT : USBH_PIPE_CTRL;
  uint8_t  token, size, flags;
  uint32_t t = (uint32_t)(nak + 1) * 1000;      // timeout in microseconds
  UsbHostDescBank* bank = &USBH_pipe[p].HostDescBank[0];

  // Configure pipe
  token = (pid == USB_PID_SETUP) ? USBH_TOKEN_SETUP : (pid == USB_PID_IN) ? USBH_TOKEN_IN : USBH_TOKEN_OUT;
  size  = USBH_SIZE_CODE(USBH_dev.ep0size);
  USB->HOST.HostPipe[p].PSTATUSSET.reg = USB_HOST_PSTATUSSET_PFREEZE;   // freeze pipe
  USB->HOST.HostPipe[p].PCFG.reg = USB_HOST_PCFG_PTYPE(ep ? USBH_PTYPE_INT : USBH_PTYPE_CONTROL)
                                 | USB_HOST_PCFG_PTOKEN(token);
  USB->HOST.HostPipe[p].BINTERVAL.reg = 1;
  bank->ADDR.reg      = (uint32_t)((pid == USB_PID_IN) ? USBH_RX_buffer : USBH_TX_buffer);
  bank->PCKSIZE.reg   = USB_HOST_PCKSIZE_SIZE(ep ? 3 : size)
                      | USB_HOST_PCKSIZE_BYTE_COUNT((pid == USB_PID_IN) ? 0 : len);
  bank->CTRL_PIPE.reg = USB_HOST_CTRL_PIPE_PDADDR(USBH_dev.addr)
                      | USB_HOST_CTRL_PIPE_PEPNUM(ep)
                      | USB_HOST_CTRL_PIPE_PERMAX(3);
  bank->STATUS_BK.reg = 0;
  bank->STATUS_PIPE.reg = 0;
  if(tog) USB->HOST.HostPipe[p].PSTATUSSET.reg = USB_HOST_PSTATUSSET_DTGL;
  else    USB->HOST.HostPipe[p].PSTATUSCLR.reg = USB_HOST_PSTATUSCLR_DTGL;
  USB->HOST.HostPipe[p].PINTFLAG.reg = 0xff;    // clear flags

  // Start transaction
  if(pid == USB_PID_IN) USB->HOST.HostPipe[p].PSTATUSCLR.reg = USB_HOST_PSTATUSCLR_BK0RDY;
  else                  USB->HOST.HostPipe[p].PSTATUSSET.reg = USB_HOST_PSTATUSSET_BK0RDY;
  USB->HOST.HostPipe[p].PSTATUSCLR.reg = USB_HOST_PSTATUSCLR_PFREEZE;  // unfreeze -> start

  // Wait for end of transaction
  do {
    flags = USB->HOST.HostPipe[p].PINTFLAG.reg;
    if(flags & (USB_HOST_PINTFLAG_TRCPT0 | USB_HOST_PINTFLAG_TXSTP | USB_HOST_PINTFLAG_STALL
              | USB_HOST_PINTFLAG_TRFAIL | USB_HOST_PINTFLAG_PERR)) break;
    if(USB->HOST.INTFLAG.reg & USB_HOST_INTFLAG_DDISC) break;  // device removed
    DLY_us(1);
  } while(--t);
  USB->HOST.HostPipe[p].PSTATUSSET.reg = USB_HOST_PSTATUSSET_PFREEZE;   // freeze pipe
  USB->HOST.HostPipe[p].PINTFLAG.reg = 0xff;    // clear flags

  // Evaluate result
  if(flags & USB_HOST_PINTFLAG_STALL) return USBH_ERR_STALL;
  if(flags & (USB_HOST_PINTFLAG_TRFAIL | USB_HOST_PINTFLAG_PERR)) return USBH_ERR_PROTOCOL;
  if(flags & (USB_HOST_PINTFLAG_TRCPT0 | USB_HOST_PINTFLAG_TXSTP)) {
    if(pid == USB_PID_IN) USBH_rxlen = bank->PCKSIZE.bit.BYTE_COUNT;
    return USBH_OK;
  }
  return nak ? USBH_ERR_TIMEOUT : USBH_ERR_NAK;
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
  USB->HOST.INTFLAG.reg = USB_HOST_INTFLAG_RST;
  USB->HOST.CTRLB.reg  |= USB_HOST_CTRLB_BUSRESET; // bus reset
  USBH_sync();
  for(i = 50; i && !(USB->HOST.INTFLAG.reg & USB_HOST_INTFLAG_RST); i--) DLY_ms(1);
  USB->HOST.INTFLAG.reg = USB_HOST_INTFLAG_RST;
  USB->HOST.CTRLB.reg  |= USB_HOST_CTRLB_SOFE;  // start of frame generation
  USBH_sync();
  DLY_ms(20);
  if(!USBH_connected) return USBH_ERR_TIMEOUT;  // device removed?
  dev->speed = !((USB->HOST.STATUS.reg & USB_HOST_STATUS_SPEED_Msk) >> USB_HOST_STATUS_SPEED_Pos);

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
  uint16_t flags = USB->HOST.INTFLAG.reg;
  if(flags & USB_HOST_INTFLAG_DCONN) {                  // device connected
    USB->HOST.INTFLAG.reg = USB_HOST_INTFLAG_DCONN;
    USBH_connected = 1;
  }
  if(flags & USB_HOST_INTFLAG_DDISC) {                  // device disconnected
    USB->HOST.INTFLAG.reg = USB_HOST_INTFLAG_DDISC;
    USBH_connected = 0;
    USB->HOST.CTRLB.reg &= ~USB_HOST_CTRLB_SOFE;        // stop SOF generation
  }
  if(USBH_connected) {                                  // device connected?
    if(USBH_dev.state == USBH_DEV_NONE)                 // new device
      USBH_dev.state = USBH_enumerate() ? USBH_DEV_FAILED : USBH_DEV_READY;
  }
  else if(USBH_dev.state != USBH_DEV_NONE) {            // device removed
    USBH_dev.state = USBH_DEV_NONE;
    USBH_dev.addr  = 0;
  }
}
