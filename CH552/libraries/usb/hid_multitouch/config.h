// ===================================================================================
// User configurations
// ===================================================================================

#pragma once

// Pin definitions
#define PIN_LED             P14       // pin connected to LED

// USB device descriptor
#define USB_VENDOR_ID       0x16C0    // VID (shared www.voti.nl)
#define USB_PRODUCT_ID      0x27D9    // PID (HID device except mice, keyboards, and joysticks)
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)

// USB configuration descriptor
#define USB_MAX_POWER_mA    50        // max power in mA

// USB descriptor strings
#define MANUFACTURER_STR    'w','a','g','i','m','i','n','a','t','o','r'
#define PRODUCT_STR         'C','H','5','5','x','M','u','l','t','i','t','o','u','c','h
#define SERIAL_STR          'C','H','5','5','x','H','I','D'
#define INTERFACE_STR       'H','I','D','-','M','u','l','t','i','t','o','u','c','h'
