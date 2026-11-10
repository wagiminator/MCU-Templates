// ===================================================================================
// Configuration of the Raspberry Pi Pico 2 Example 'usb_media'
// ===================================================================================
//
// All settings of this example are made here. This file is also read by the makefile
// (F_CPU) and by the libraries in the src folder.
//
// After changing a setting, simply run "make flash" again.

#pragma once

// ===================================================================================
// System Clock (also read by the makefile)
// ===================================================================================
// System clock in Hz: 12000000 or 24000000..150000000 (must be generated exactly by the PLL)
#define F_CPU                 150000000

// ===================================================================================
// Board: Raspberry Pi Pico 2
// ===================================================================================
#define PIN_LED               GP25      // pin connected to on-board LED
#define BOARD_NAME            "Raspberry Pi Pico 2"

// ===================================================================================
// Example Settings
// ===================================================================================
// Media keys (see usb_consumer.h, e.g. CON_MEDIA_PLAY, CON_MEDIA_NEXT, CON_MEDIA_PREV,
// CON_VOL_UP, CON_VOL_DOWN, CON_VOL_MUTE)
#define MEDIA_KEY_SHORT       0xCD      // short press: play/pause (toggle)
#define MEDIA_KEY_DOUBLE      CON_MEDIA_NEXT  // double press: next track
#define MEDIA_KEY_LONG        CON_VOL_MUTE    // long press: mute

// ===================================================================================
// USB Descriptors
// ===================================================================================
#define USB_VENDOR_ID         0x1189    // VID
#define USB_PRODUCT_ID        0x8890    // PID
#define USB_DEVICE_VERSION    0x0100    // v1.0 (BCD-format)
#define USB_LANGUAGE          0x0409    // US English
#define USB_MAX_POWER_mA      100       // max power in mA
#define MANUF_STR             "wagiminator"
#define PROD_STR              "Pico 2 USB Media Keys"
#define SERIAL_STR            "RP2350USBMEDIA"
#define INTERF_STR            "HID-Consumer-Media"
