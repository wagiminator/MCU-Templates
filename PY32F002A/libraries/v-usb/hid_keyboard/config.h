// ===================================================================================
// User Configurations
// ===================================================================================

#pragma once

// USB pin defines (D+, D- and DPU must be on the same port)
#define USB_PORT            A         // [A,B,F] GPIO port to use with D+, D- and DPU
#define USB_PIN_DP          3         // [0-15] GPIO number for USB D+ pin
#define USB_PIN_DM          1         // [0-15] GPIO number for USB D- pin (interrupt)
//#define USB_PIN_DPU         4         // [0-15] GPIO for feeding the 1.5k pull-up on USB D- pin; comment out if not used / tied to 3V3!

// HSI calibration
#define USB_CALIBRATE       1         // 1: calibrate HSI using USB keep-alive pulses and TIM16

// USB configuration descriptor
#define USB_MAX_POWER_mA    50        // max power in mA 

// USB device descriptor
#define USB_VENDOR_ID       0x1209    // VID
#define USB_PRODUCT_ID      0xc003    // PID
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)
#define USB_LANGUAGE        0x0409    // US English

// USB descriptor strings
#define MANUF_STR           "wagiminator"
#define PROD_STR            "HID Keyboard"
#define SERIAL_STR          "PY32HID"

