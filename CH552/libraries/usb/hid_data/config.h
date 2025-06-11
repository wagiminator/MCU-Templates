// ===================================================================================
// User Configurations
// ===================================================================================

#pragma once

// Pin definitions
#define PIN_LED             P14       // pin connected to LED

// USB device descriptor
#define USB_VENDOR_ID       0x16C0    // VID (shared www.voti.nl)
#define USB_PRODUCT_ID      0x05DF    // PID (shared generic HID)
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)

// USB configuration descriptor
#define USB_MAX_POWER_mA    100       // max power in mA 

// USB descriptor strings
#define MANUFACTURER_STR    'w','a','g','i','m','i','n','a','t','o','r'
#define PRODUCT_STR         'H','I','D','-','D','a','t','a'
#define SERIAL_STR          'C','H','5','5','x','H','I','D'
#define INTERFACE_STR       'H','I','D','-','D','a','t','a'
