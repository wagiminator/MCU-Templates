// ===================================================================================
// User Configurations for USB Media Keys
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

// Media keys (see usb_consumer.h, e.g. CON_MEDIA_PLAY, CON_MEDIA_NEXT, CON_MEDIA_PREV,
// CON_VOL_UP, CON_VOL_DOWN, CON_VOL_MUTE)
#define MEDIA_KEY_SHORT     0xCD      // short press: play/pause (toggle)
#define MEDIA_KEY_DOUBLE    CON_MEDIA_NEXT  // double press: next track
#define MEDIA_KEY_LONG      CON_VOL_MUTE    // long press: mute

// USB device descriptor
#define USB_VENDOR_ID       0x1189    // VID
#define USB_PRODUCT_ID      0x8890    // PID
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)
#define USB_LANGUAGE        0x0409    // US English

// USB configuration descriptor
#define USB_MAX_POWER_mA    100       // max power in mA

// USB descriptor strings
#define MANUF_STR           "wagiminator"
#define PROD_STR            "CH573F USB Media Keys"
#define SERIAL_STR          "CH573USBMEDIA"
#define INTERF_STR          "HID-Consumer-Media"
