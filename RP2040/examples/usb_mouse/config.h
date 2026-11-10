// ===================================================================================
// Configuration of the Raspberry Pi Pico 2 Example 'usb_mouse'
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
#define CIRCLE_RADIUS         100       // radius of the circle in pixels (10..120)
#define MOUSE_DRAW            0         // 1: hold left mouse button while moving (draw)
#define JIGGLE_INTERVAL       30000     // mouse jiggler: time between two moves in ms

// ===================================================================================
// USB Descriptors
// ===================================================================================
#define USB_VENDOR_ID         0x16C0    // VID (www.voti.nl)
#define USB_PRODUCT_ID        0x27DA    // PID (shared HID-mice)
#define USB_DEVICE_VERSION    0x0100    // v1.0 (BCD-format)
#define USB_LANGUAGE          0x0409    // US English
#define USB_MAX_POWER_mA      100       // max power in mA
#define MANUF_STR             "wagiminator"
#define PROD_STR              "Pico 2 USB Mouse"
#define SERIAL_STR            "RP2350USBMOUSE"
#define INTERF_STR            "HID-Mouse"
