// ===================================================================================
// USB Host Functions for CH554                                               * v1.0 *
// ===================================================================================
//
// Lightweight USB host for HID devices directly connected to the USB port of the
// CH554 (no hubs). Low-speed and full-speed devices are supported.
// The host is polled (no interrupts).
//
// Functions available:
// --------------------
// USBH_init()              init USB host and port
// USBH_task()              handle connect/disconnect and enumeration (call frequently)
// USBH_ready(p)            check if a supported device is ready on port p
// USBH_poll(p)             poll interrupt IN endpoint of device on port p,
//                          returns number of received bytes (data in USBH_RX_buffer)
// USBH_setup(t,r,v,i,l)    prepare SETUP packet (type, request, value, index, length)
// USBH_control(p,buf)      execute control transfer with prepared SETUP on port p,
//                          data stage to/from buf, returns 0 on success
//
// Class driver interface (implemented by the class driver, e.g. host_keyboard.c):
// -------------------------------------------------------------------------------
// uint8_t USBH_CLASS_match(__xdata uint8_t* itf)  return 1 if interface is supported
// uint8_t USBH_CLASS_init(uint8_t port)           init device, return 0 on success
//
// Port:
// -----
// Port 0: UDP/UDM (P3.6/P3.7), the only USB port of the CH554 (USBH_PORTS = 1)
//
// Notes:
// ------
// - Only the CH554 supports the USB host mode (not CH551/CH552).
// - F_CPU must be at least 12MHz (recommended: 24MHz).
// - The USB DMA buffers are at the beginning of the xRAM (0x0000 - 0x007F), so
//   XRAM_LOC = 0x0100 in the makefile must not be changed.
// - VBUS (5V) for the device must be provided by the board.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch554.h"
#include "usb.h"
#include "config.h"

// ===================================================================================
// Parameters (can be overwritten in config.h)
// ===================================================================================
#define USBH_PORTS          1         // number of ports (CH554: 1)
#ifndef USBH_BUF_SIZE
  #define USBH_BUF_SIZE     128       // size of descriptor buffer
#endif

// ===================================================================================
// Device States and Error Codes
// ===================================================================================
#define USBH_DEV_NONE       0         // no device connected
#define USBH_DEV_READY      1         // supported device ready
#define USBH_DEV_FAILED     2         // device not supported or enumeration failed

#define USBH_OK             0         // success
#define USBH_ERR_NAK        1         // device responded with NAK
#define USBH_ERR_STALL      2         // device responded with STALL
#define USBH_ERR_TIMEOUT    3         // no response
#define USBH_ERR_PROTOCOL   4         // other error (toggle, CRC, ...)
#define USBH_ERR_DESCR      5         // descriptor not supported

// ===================================================================================
// Device Information
// ===================================================================================
typedef struct {
  uint8_t  state;                     // device state (see above)
  uint8_t  speed;                     // 0: low-speed, 1: full-speed
  uint8_t  addr;                      // USB device address
  uint8_t  ep0size;                   // max packet size of EP0
  uint8_t  itf;                       // interface number
  uint8_t  ep;                        // interrupt IN endpoint number
  uint8_t  tog;                       // data toggle of interrupt IN endpoint
  uint16_t rlen;                      // HID report descriptor length
} USBH_DEV_t;

extern __xdata USBH_DEV_t USBH_dev[USBH_PORTS];
extern __xdata uint8_t USBH_buffer[USBH_BUF_SIZE];  // descriptor/data buffer
__xdata __at (0x0000) uint8_t USBH_RX_buffer[64];   // DMA receive buffer
__xdata __at (0x0040) uint8_t USBH_TX_buffer[64];   // DMA transmit buffer
#define USBH_setupReq ((PXUSB_SETUP_REQ)USBH_TX_buffer)

// ===================================================================================
// Functions
// ===================================================================================
void USBH_init(void);
void USBH_task(void);
uint8_t USBH_poll(uint8_t port);
void USBH_setup(uint8_t type, uint8_t req, uint16_t value, uint16_t index, uint16_t len);
uint8_t USBH_control(uint8_t port, __xdata uint8_t* buf);
#define USBH_ready(p)       (USBH_dev[(p)].state == USBH_DEV_READY)

// Class driver interface
uint8_t USBH_CLASS_match(__xdata uint8_t* itf);
uint8_t USBH_CLASS_init(uint8_t port);
