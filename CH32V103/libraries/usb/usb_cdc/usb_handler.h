// ===================================================================================
// USB Handler for CH32V103 (USBHD)                                           * v1.1 *
// ===================================================================================
//
// Endpoint functions (used by the class drivers):
// -----------------------------------------------
// USB_EP_init(ep,type,buf,out,in) setup endpoint ep (1..7) with buffer buf
// USB_EP_send(ep, len)     transmit len bytes from IN buffer of endpoint ep
// USB_EP_receive(ep)       ready to receive next OUT packet on endpoint ep (ACK)
// USB_EP_hold(ep)          do not accept OUT packets on endpoint ep (NAK)
// USB_RX_len               number of bytes of the last received OUT packet
//
// After an OUT packet has been received, the endpoint responds with NAK until
// USB_EP_receive() is called. After an IN packet has been sent, the endpoint
// responds with NAK until USB_EP_send() is called again.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "usb_descr.h"

// ===================================================================================
// USB Handler Parameters and Checks
// ===================================================================================
#if SYS_USE_VECTORS == 0
  #error Interrupt vector table must be enabled (SYS_USE_VECTORS in system.h)!
#endif

// MCU supply voltage (0: 3.3V, 1: 5V), can be defined in config.h
#ifndef USB_VDD
  #define USB_VDD                 0
#endif

// USB clock (48MHz) from PLL clock (= F_CPU)
#if   F_CPU == 48000000
  #define USB_RCC_USBPRE          RCC_USBPRE              // PLLCLK / 1
#elif F_CPU == 72000000
  #define USB_RCC_USBPRE          0                       // PLLCLK / 1.5
#else
  #error USB requires F_CPU = 48000000 or 72000000!
#endif

// ===================================================================================
// Custom External USB Handler Functions
// ===================================================================================
uint8_t CDC_control(void);
void CDC_EP_init(void);
void CDC_EP0_OUT(void);
void CDC_EP2_IN(void);
void CDC_EP2_OUT(void);

// ===================================================================================
// USB Handler Defines
// ===================================================================================
// Custom USB handler functions
#define USB_INIT_endpoints        CDC_EP_init   // custom USB EP init handler
#define USB_CLASS_SETUP_handler   CDC_control   // handle custom class requests
#define USB_CLASS_OUT_handler     CDC_EP0_OUT   // handle class out

// Endpoint callback functions
#define EP2_IN_callback           CDC_EP2_IN
#define EP2_OUT_callback          CDC_EP2_OUT

// ===================================================================================
// Variables
// ===================================================================================
#define USB_SetupBuf     ((PUSB_SETUP_REQ)EP0_buffer)
extern volatile uint8_t  USB_SetupReq, USB_SetupTyp, USB_Config, USB_Addr, USB_ENUM_OK;
extern volatile uint16_t USB_SetupLen;
extern volatile uint8_t  USB_RX_len;
extern const uint8_t*    USB_pDescr;

// ===================================================================================
// Functions
// ===================================================================================
void USB_init(void);
void USB_EP0_copyDescr(uint8_t len);
void USB_EP_init(uint8_t ep, uint8_t type, uint8_t* buf, uint8_t out, uint8_t in);
void USB_EP_send(uint8_t ep, uint8_t len);
void USB_EP_receive(uint8_t ep);
void USB_EP_hold(uint8_t ep);

#ifdef __cplusplus
}
#endif
