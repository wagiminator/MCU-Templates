// ===================================================================================
// USB Host Handler for CH32X035 (USBFS)                                      * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "usb_host.h"

// ===================================================================================
// MCU Specific Parameters and Checks
// ===================================================================================
// MCU supply voltage (0: 3.3V, 1: 5V), can be defined in config.h
#ifndef USB_VDD
  #define USB_VDD           0
#endif

// ===================================================================================
// MCU Specific Register Mapping (all register accesses of the host go through these)
// ===================================================================================
#define USBH_R_CTRL         USBFSH->BASE_CTRL   // USB control
#define USBH_R_HOST         USBFSH->HOST_CTRL   // host port control
#define USBH_R_INT_EN       USBFSH->INT_EN      // interrupt enable
#define USBH_R_ADDR         USBFSH->DEV_ADDR    // device address
#define USBH_R_MIS          USBFSH->MIS_ST      // miscellaneous status
#define USBH_R_INT_FG       USBFSH->INT_FG      // interrupt flags
#define USBH_R_INT_ST       USBFSH->INT_ST      // interrupt status
#define USBH_R_RX_LEN       USBFSH->RX_LEN      // received length
#define USBH_R_EP_MOD       USBFSH->HOST_EP_MOD // host endpoint mode
#define USBH_R_RX_DMA       USBFSH->HOST_RX_DMA // receive DMA address (16 bits)
#define USBH_R_TX_DMA       USBFSH->HOST_TX_DMA // transmit DMA address (16 bits)
#define USBH_R_SETUP        USBFSH->HOST_SETUP  // host auxiliary setup
#define USBH_R_EP_PID       USBFSH->HOST_EP_PID // token PID and endpoint
#define USBH_R_RX_CTRL      USBFSH->HOST_RX_CTRL// receiver control
#define USBH_R_TX_LEN       USBFSH->HOST_TX_LEN // transmit length
#define USBH_R_TX_CTRL      USBFSH->HOST_TX_CTRL// transmitter control
#define USBH_DMA(buf)       ((uint16_t)(uintptr_t)(buf))
#define USBH_C_HOST_MODE    USBFS_UC_HOST_MODE
#define USBH_C_INT_BUSY     USBFS_UC_INT_BUSY
#define USBH_C_DMA_EN       USBFS_UC_DMA_EN
#define USBH_C_LOW_SPEED    USBFS_UC_LOW_SPEED
#define USBH_C_RESET        (USBFS_UC_RESET_SIE | USBFS_UC_CLR_ALL)
#define USBH_H_PORT_EN      USBFS_UH_PORT_EN
#define USBH_H_BUS_RESET    USBFS_UH_BUS_RESET
#define USBH_H_LOW_SPEED    USBFS_UH_LOW_SPEED
#define USBH_EP_TX_EN       USBFS_UH_EP_TX_EN
#define USBH_EP_RX_EN       USBFS_UH_EP_RX_EN
#define USBH_SOF_EN         USBFS_UH_SOF_EN
#define USBH_R_TOG          USBFS_UH_R_TOG
#define USBH_T_TOG          USBFS_UH_T_TOG
#define USBH_MIS_ATTACH     USBFS_UMS_DEV_ATTACH
#define USBH_MIS_DM_LEVEL   USBFS_UMS_DM_LEVEL
#define USBH_IF_TRANSFER    USBFS_UIF_TRANSFER
#define USBH_IF_DETECT      USBFS_UIF_DETECT
#define USBH_IE_TRANSFER    USBFS_UIE_TRANSFER
#define USBH_IE_DETECT      USBFS_UIE_DETECT
#define USBH_ST_TOG_OK      USBFS_UIS_TOG_OK
#define USBH_ST_H_RES       USBFS_UIS_H_RES_MASK
#define USBH_ADDR_GP_BIT    USBFS_UDA_GP_BIT

// ===================================================================================
// Variables
// ===================================================================================
USBH_DEV_t USBH_dev;                                    // device information
uint8_t USBH_buffer[USBH_BUF_SIZE];                     // descriptor/data buffer
uint8_t USBH_RX_buffer[64] __attribute__((aligned(4))); // DMA receive buffer
uint8_t USBH_TX_buffer[64] __attribute__((aligned(4))); // DMA transmit buffer

// ===================================================================================
// Init USB Host
// ===================================================================================
void USBH_init(void) {
  // Enable USB I/O (PC16 = D-, PC17 = D+) as floating inputs, enable USBFS module
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN;
  RCC->AHBPCENR  |= RCC_USBFS;
  GPIOC->CFGXR    = (GPIOC->CFGXR & ~((uint32_t)0xFF)) | ((uint32_t)0x44);

  // Select PHY supply voltage, no pull-up resistors (host uses internal pull-downs)
  #if USB_VDD > 0
  AFIO->CTLR = (AFIO->CTLR & ~(AFIO_CTLR_UDP_PUE | AFIO_CTLR_UDM_PUE | AFIO_CTLR_USB_PHY_V33))
             | AFIO_CTLR_USB_IOEN;
  #else
  AFIO->CTLR = (AFIO->CTLR & ~(AFIO_CTLR_UDP_PUE | AFIO_CTLR_UDM_PUE))
             | AFIO_CTLR_USB_PHY_V33 | AFIO_CTLR_USB_IOEN;
  #endif

  // Reset and setup USB host
  USBH_R_CTRL    = USBH_C_RESET;                // reset SIE, clear FIFO and flags
  DLY_us(10);
  USBH_R_CTRL    = USBH_C_HOST_MODE;            // host mode
  USBH_R_HOST    = 0;                           // port disabled, pull-downs enabled
  USBH_R_ADDR    = 0;                           // device address 0
  USBH_R_EP_MOD  = USBH_EP_TX_EN | USBH_EP_RX_EN; // enable host endpoints (single buffer)
  USBH_R_RX_DMA  = USBH_DMA(USBH_RX_buffer);    // DMA buffer addresses
  USBH_R_TX_DMA  = USBH_DMA(USBH_TX_buffer);
  USBH_R_RX_CTRL = 0;
  USBH_R_TX_CTRL = 0;
  USBH_R_CTRL    = USBH_C_HOST_MODE             // host mode
                 | USBH_C_INT_BUSY              // pause while transfer flag is set
                 | USBH_C_DMA_EN;               // enable DMA
  USBH_R_SETUP   = USBH_SOF_EN;                 // automatic SOF/keep-alive
  USBH_R_INT_FG  = 0xFF;                        // clear all flags
  USBH_R_INT_EN  = USBH_IE_TRANSFER | USBH_IE_DETECT; // flags only (NVIC not enabled)
  USBH_dev.state = USBH_DEV_NONE;
}

// ===================================================================================
// Set Speed of Host Port (internal)
// ===================================================================================
static void USBH_setSpeed(uint8_t full) {
  if(full) {
    USBH_R_CTRL &= ~USBH_C_LOW_SPEED;
    USBH_R_HOST &= ~USBH_H_LOW_SPEED;
  }
  else {
    USBH_R_CTRL |=  USBH_C_LOW_SPEED;
    USBH_R_HOST |=  USBH_H_LOW_SPEED;
  }
}

// ===================================================================================
// Execute a USB Transaction (internal)
// ===================================================================================
// pid_ep: token PID (upper nibble) and endpoint (lower nibble)
// tog:    data toggle (0: DATA0, 1: DATA1)
// nak:    number of retries on NAK (0: return immediately)
static uint8_t USBH_transact(uint8_t pid_ep, uint8_t tog, uint16_t nak) {
  uint8_t  res, err = 3;
  uint16_t t;
  USBH_R_RX_CTRL = tog ? USBH_R_TOG : 0;       // expected toggle for IN
  USBH_R_TX_CTRL = tog ? USBH_T_TOG : 0;       // toggle for SETUP/OUT
  while(1) {
    USBH_R_INT_FG = USBH_IF_TRANSFER;           // clear flag
    USBH_R_EP_PID = pid_ep;                     // start transaction
    for(t = 1000; t && !(USBH_R_INT_FG & USBH_IF_TRANSFER); t--) DLY_us(1); // max. ~1ms
    USBH_R_EP_PID = 0;                          // stop
    if(!(USBH_R_INT_FG & USBH_IF_TRANSFER)) return USBH_ERR_TIMEOUT;
    res = USBH_R_INT_ST;                        // toggle OK and response PID
    USBH_R_INT_FG = USBH_IF_TRANSFER;           // clear flag (SIE is paused while set)
    if(res & USBH_ST_TOG_OK) return USBH_OK;    // ACK or matching DATAx received
    res &= USBH_ST_H_RES;                       // handshake/data PID of device
    if(res == USB_PID_STALL) return USBH_ERR_STALL;
    if(res == USB_PID_NAK) {
      if(!nak) return USBH_ERR_NAK;
      nak--;
    }
    else if(((pid_ep >> 4) != USB_PID_IN) && (res == USB_PID_ACK)) return USBH_OK;
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
  USBH_R_TX_LEN = 8;
  res = USBH_transact((uint8_t)(USB_PID_SETUP << 4), 0, 1000);
  if(res) return res;

  // DATA stage
  tog = 1;
  if(len && dir) {                              // IN data stage
    while(len) {
      res = USBH_transact((uint8_t)(USB_PID_IN << 4), tog, 2000);
      if(res) return res;
      tog ^= 1;
      n = (uint8_t)USBH_R_RX_LEN;
      if(n > len) n = len;
      for(i = 0; i < n; i++) *buf++ = USBH_RX_buffer[i];
      len -= n;
      if(n < ep0) break;                        // short packet: end of data
    }
    USBH_R_TX_LEN = 0;                          // STATUS stage: OUT ZLP DATA1
    return USBH_transact((uint8_t)(USB_PID_OUT << 4), 1, 1000);
  }
  else if(len) {                                // OUT data stage
    while(len) {
      n = (len > ep0) ? ep0 : len;
      for(i = 0; i < n; i++) USBH_TX_buffer[i] = *buf++;
      USBH_R_TX_LEN = n;
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
uint8_t USBH_poll(void) {
  if(USBH_dev.state != USBH_DEV_READY) return 0;
  if(USBH_transact((uint8_t)(USB_PID_IN << 4) | USBH_dev.ep, USBH_dev.tog, 0)) return 0;
  USBH_dev.tog ^= 1;
  return (uint8_t)USBH_R_RX_LEN;
}

// ===================================================================================
// Enumerate Device (internal)
// ===================================================================================
static uint8_t USBH_enumerate(void) {
  USBH_DEV_t* dev = &USBH_dev;
  uint8_t* p;
  uint16_t total, i;
  uint8_t  res, found = 0;

  // Reset device and enable port
  dev->addr    = 0;
  dev->speed   = 1;
  dev->ep0size = 8;
  dev->itf     = 0;
  dev->ep      = 0;
  dev->tog     = 0;
  dev->rlen    = 0;
  USBH_R_ADDR  = (USBH_R_ADDR & USBH_ADDR_GP_BIT);    // address 0
  USBH_setSpeed(1);
  DLY_ms(100);                                  // wait for stable connection
  USBH_R_HOST &= ~USBH_H_PORT_EN;
  USBH_R_HOST |=  USBH_H_BUS_RESET;             // bus reset
  DLY_ms(15);
  USBH_R_HOST &= ~USBH_H_BUS_RESET;             // end of bus reset
  DLY_us(250);
  if(!(USBH_R_MIS & USBH_MIS_ATTACH)) return USBH_ERR_TIMEOUT;
  if(USBH_R_MIS & USBH_MIS_DM_LEVEL) {          // D- high at attach: low-speed device
    dev->speed = 0;
    USBH_setSpeed(0);
  }
  USBH_R_HOST |= USBH_H_PORT_EN;                // enable port
  DLY_ms(20);
  USBH_R_INT_FG = USBH_IF_DETECT;

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
  dev->addr   = 1;
  USBH_R_ADDR = (USBH_R_ADDR & USBH_ADDR_GP_BIT) | 1;
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
  if(USBH_R_MIS & USBH_MIS_ATTACH) {
    if(USBH_dev.state == USBH_DEV_NONE)                 // new device
      USBH_dev.state = USBH_enumerate() ? USBH_DEV_FAILED : USBH_DEV_READY;
  }
  else if(USBH_dev.state != USBH_DEV_NONE) {            // device removed
    USBH_dev.state = USBH_DEV_NONE;
    USBH_R_HOST &= ~(USBH_H_PORT_EN | USBH_H_LOW_SPEED);
  }
  USBH_R_INT_FG = USBH_IF_DETECT;
}
