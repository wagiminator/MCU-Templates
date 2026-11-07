// ===================================================================================
// V-USB Configuration for tinyAVR 0/1-Series                                 * v1.0 *
// ===================================================================================
//
// This file configures the V-USB driver according to the settings in config.h.
// Normally there is no need to change anything here.
//
// The V-USB driver was ported to the tinyAVR 0/1-series by 12oClocker
// (https://github.com/12oclocker/V-USB_TinyAvr). V-USB is copyright (c) by
// OBJECTIVE DEVELOPMENT Software GmbH, license: GNU GPL v2 or v3 (see License.txt).

#ifndef __usbconfig_h_included__
#define __usbconfig_h_included__

#include "config.h"

// ===================================================================================
// Default Settings
// ===================================================================================
#ifndef USB_PORT
  #define USB_PORT            A
#endif
#ifndef USB_PIN_DP
  #define USB_PIN_DP          2
#endif
#ifndef USB_PIN_DM
  #define USB_PIN_DM          1
#endif
#ifndef USB_POLL_TIMER
  #define USB_POLL_TIMER      0
#endif
#ifndef USB_MAX_POWER_mA
  #define USB_MAX_POWER_mA    100
#endif

// ===================================================================================
// Hardware Configuration
// ===================================================================================
#define USB_CFG_CAT3(a, b, c)         a ## b ## c
#define USB_CFG_XCAT3(a, b, c)        USB_CFG_CAT3(a, b, c)
#define USB_CFG_CAT5(a, b, c, d, e)   a ## b ## c ## d ## e
#define USB_CFG_XCAT5(a, b, c, d, e)  USB_CFG_CAT5(a, b, c, d, e)

// tinyAVR 0/1-series support (12oClocker)
#define USB_CFG_TINYAVR_SERIES        1             // enable tinyAVR support
#define USB_GPIOR0_REG                GPIO_GPIOR0   // GPIOR0 is used by the driver
#define USB_GPIOR0_MEM                0x1C          // address of GPIOR0
#define USB_GPIOR1_REG                GPIO_GPIOR1   // GPIOR1 is used by the driver
#define USB_GPIOR1_MEM                0x1D          // address of GPIOR1

// USB pins
#define USB_CFG_IOPORTNAME            USB_PORT
#define USB_CFG_DMINUS_BIT            USB_PIN_DM
#define USB_CFG_DPLUS_BIT             USB_PIN_DP
#ifdef USB_PIN_DPU
  #define USB_CFG_PULLUP_IOPORTNAME   USB_PORT
  #define USB_CFG_PULLUP_BIT          USB_PIN_DPU
#endif

// Clock and CRC check
#define USB_CFG_CLOCK_KHZ             (F_CPU/1000)
#define USB_CFG_CHECK_CRC             0

// Pin change interrupt on D+ (rising edge)
#define USB_INTR_CFG                  0
#define USB_INTR_CFG_SET              0
#define USB_INTR_CFG_CLR              0
#define USB_INTR_ENABLE               USB_CFG_XCAT5(PORT, USB_PORT, _PIN, USB_PIN_DP, CTRL)
#define USB_INTR_ENABLE_BIT           1             // ISC = 0x02: rising edge
#define USB_INTR_PENDING              USB_CFG_XCAT3(VPORT, USB_PORT, _INTFLAGS)
#define USB_INTR_PENDING_BIT          USB_PIN_DP
#define USB_INTR_VECTOR               USB_CFG_XCAT3(PORT, USB_PORT, _PORT_vect)
#define USB_INTR_VECTOR_NUM           USB_CFG_XCAT3(PORT, USB_PORT, _PORT_vect_num)

// ===================================================================================
// Functional Range
// ===================================================================================
#define USB_CFG_HAVE_INTRIN_ENDPOINT        1       // interrupt-in endpoint 1
#define USB_CFG_HAVE_INTRIN_ENDPOINT3       0       // no second interrupt-in endpoint
#define USB_CFG_EP3_NUMBER                  3
#define USB_CFG_IMPLEMENT_HALT              0
#define USB_CFG_SUPPRESS_INTR_CODE          0
#define USB_CFG_INTR_POLL_INTERVAL          10
#define USB_CFG_IS_SELF_POWERED             0
#define USB_CFG_MAX_BUS_POWER               USB_MAX_POWER_mA
#define USB_CFG_IMPLEMENT_FN_WRITE          1       // usbFunctionWrite() for SET_REPORT
#define USB_CFG_IMPLEMENT_FN_READ           0
#define USB_CFG_IMPLEMENT_FN_WRITEOUT       1       // usbFunctionWriteOut() for OUT endpoints
#define USB_CFG_HAVE_FLOWCONTROL            0
#define USB_CFG_DRIVER_FLASH_PAGE           0
#define USB_CFG_LONG_TRANSFERS              0
#define USB_COUNT_SOF                       0
#define USB_CFG_CHECK_DATA_TOGGLING         0
#define USB_CFG_HAVE_MEASURE_FRAME_LENGTH   1       // needed for oscillator calibration
#define USB_USE_FAST_CRC                    0

// Oscillator calibration after each USB bus reset
#ifndef __ASSEMBLER__
extern void usbHadReset(void);
#endif
#define USB_RESET_HOOK(resetStarts)         if(!resetStarts) {usbHadReset();}

// ===================================================================================
// Device Description (all descriptors are defined in usb_descr.h)
// ===================================================================================
#define USB_CFG_VENDOR_ID           (USB_VENDOR_ID & 0xff), (USB_VENDOR_ID >> 8)
#define USB_CFG_DEVICE_ID           (USB_PRODUCT_ID & 0xff), (USB_PRODUCT_ID >> 8)
#define USB_CFG_DEVICE_VERSION      (USB_DEVICE_VERSION & 0xff), (USB_DEVICE_VERSION >> 8)
#define USB_CFG_DEVICE_CLASS        0
#define USB_CFG_DEVICE_SUBCLASS     0
#define USB_CFG_INTERFACE_CLASS     3
#define USB_CFG_INTERFACE_SUBCLASS  0
#define USB_CFG_INTERFACE_PROTOCOL  0
#define USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH        0

// All descriptors are provided by usbFunctionDescriptor() (from flash)
#define USB_CFG_DESCR_PROPS_DEVICE                  USB_PROP_IS_DYNAMIC
#define USB_CFG_DESCR_PROPS_CONFIGURATION           USB_PROP_IS_DYNAMIC
#define USB_CFG_DESCR_PROPS_STRINGS                 USB_PROP_IS_DYNAMIC
#define USB_CFG_DESCR_PROPS_STRING_0                0
#define USB_CFG_DESCR_PROPS_STRING_VENDOR           0
#define USB_CFG_DESCR_PROPS_STRING_PRODUCT          0
#define USB_CFG_DESCR_PROPS_STRING_SERIAL_NUMBER    0
#define USB_CFG_DESCR_PROPS_HID                     USB_PROP_IS_DYNAMIC
#define USB_CFG_DESCR_PROPS_HID_REPORT              USB_PROP_IS_DYNAMIC
#define USB_CFG_DESCR_PROPS_UNKNOWN                 USB_PROP_IS_DYNAMIC

#endif // __usbconfig_h_included__
