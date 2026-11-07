// ===================================================================================
// User Configurations
// ===================================================================================

#pragma once

// USB pin defines (D+, D- and DPU must be on the same port)
#define USB_PORT            A         // [A-F] GPIO port to use with D+, D- and DPU
#define USB_PIN_DP          2         // [0-7] GPIO number for USB D+ pin (Px2/Px6 recommended)
#define USB_PIN_DM          1         // [0-7] GPIO number for USB D- pin
//#define USB_PIN_DPU         3         // [0-7] GPIO for feeding the 1.5k pull-up on USB D- pin; comment out if not used / tied to 3V3!

// USB polling timer
#define USB_POLL_TIMER      0         // 0: RTC/PIT, 1: TCB0, 2: TCB1, 3: TCB2, 4: TCB3 (x09)

// USB configuration descriptor
#define USB_MAX_POWER_mA    50        // max power in mA 

// USB device descriptor
#define USB_VENDOR_ID       0x1209    // VID
#define USB_PRODUCT_ID      0xc003    // PID
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)
#define USB_LANGUAGE        0x0409    // US English

// USB descriptor strings
#define MANUF_STR           "wagiminator"
#define PROD_STR            "HID Media Keyboard"
#define SERIAL_STR          "MEGAAVRHID"
