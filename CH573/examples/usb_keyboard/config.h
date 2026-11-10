// ===================================================================================
// User Configurations for USB Keyboard
// ===================================================================================

#pragma once

// Board
#define BOARD_NAME          "CH573F Core Board"
#define PIN_LED             PA8       // pin connected to LED
#define LED_ACTIVE_LOW      1         // 1: LED lights up when pin is low, 0: when high
#define PIN_BOOT            PB22      // BOOT key (to GND)
#define PIN_KEY             PB23      // second key (to GND), RST# if enabled

// System
#define SYS_CLK32K          0         // 32kHz clock: 0: internal LSI, 1: 32.768kHz crystal (PA10/PA11)

// Keyboard
#define KBD_TEXT            "Hello from CH573F!"  // text typed by a short press

// USB device descriptor
#define USB_VENDOR_ID       0x16C0    // VID (shared www.voti.nl)
#define USB_PRODUCT_ID      0x27DB    // PID (shared HID-keyboards)
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)
#define USB_LANGUAGE        0x0409    // US English

// USB configuration descriptor
#define USB_MAX_POWER_mA    100       // max power in mA

// USB descriptor strings
#define MANUF_STR           "wagiminator"
#define PROD_STR            "CH573F USB Keyboard"
#define SERIAL_STR          "CH573USBKEYBOARD"
#define INTERF_STR          "HID-Keyboard"
