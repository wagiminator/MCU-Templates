// ===================================================================================
// USB Descriptors and Definitions
// ===================================================================================
//
// Definition of USB descriptors and endpoints.
//
// The following must be defined in config.h:
// USB_VENDOR_ID            - Vendor ID (16-bit word)
// USB_PRODUCT_ID           - Product ID (16-bit word)
// USB_DEVICE_VERSION       - Device version (16-bit BCD)
// USB_LANGUAGE             - Language descriptor code
// USB_MAX_POWER_mA         - Device max power in mA
// All string descriptors.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "usb.h"
#include "config.h"

// ===================================================================================
// USB Endpoint Definitions
// ===================================================================================
// Endpoint sizes
#define EP0_SIZE        8
#define EP1_SIZE        8
#define EP2_SIZE        64

#define EP0_BUF_SIZE    EP_BUF_SIZE(EP0_SIZE)
#define EP1_BUF_SIZE    EP_BUF_SIZE(EP1_SIZE)
#define EP2_BUF_SIZE    EP_BUF_SIZE(EP2_SIZE) + 64

#define EP_BUF_SIZE(x)  (x+2<64 ? x+2 : 64)

// Endpoint buffers
extern uint8_t __attribute__((aligned(4))) EP0_buffer[];
extern uint8_t __attribute__((aligned(4))) EP1_buffer[];
extern uint8_t __attribute__((aligned(4))) EP2_buffer[];

// ===================================================================================
// Device and Configuration Descriptors
// ===================================================================================
typedef struct __attribute__((packed)) {
  USB_CFG_DESCR config;
  USB_IAD_DESCR association;
  USB_ITF_DESCR interface0;
  uint8_t functional[19];
  USB_ENDP_DESCR ep1IN;
  USB_ITF_DESCR interface1;
  USB_ENDP_DESCR ep2OUT;
  USB_ENDP_DESCR ep2IN;
} USB_CFG_DESCR_CDC, *PUSB_CFG_DESCR_CDC;

extern const USB_DEV_DESCR DevDescr;
extern const USB_CFG_DESCR_CDC CfgDescr;

// ===================================================================================
// String Descriptors
// ===================================================================================
extern const USB_STR_DESCR LangDescr;
extern const USB_STR_DESCR ManufDescr;
extern const USB_STR_DESCR ProdDescr;
extern const USB_STR_DESCR SerDescr;
extern const USB_STR_DESCR InterfDescr;

#define USB_STR_DESCR_i0    (uint8_t*)&LangDescr
#define USB_STR_DESCR_i1    (uint8_t*)&ManufDescr
#define USB_STR_DESCR_i2    (uint8_t*)&ProdDescr
#define USB_STR_DESCR_i3    (uint8_t*)&SerDescr
#define USB_STR_DESCR_i4    (uint8_t*)&InterfDescr
#define USB_STR_DESCR_ix    (uint8_t*)&SerDescr

#define USB_CHAR_GLUE(s) u##s
#define USB_CHAR_TO_STR_DESCR(s)  {           \
  .bLength = sizeof(USB_CHAR_GLUE(s)),        \
  .bDescriptorType = USB_DESCR_TYP_STRING,    \
  .bString = USB_CHAR_GLUE(s)                 \
}

#ifdef __cplusplus
}
#endif
