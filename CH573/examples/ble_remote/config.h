// ===================================================================================
// User Configurations for Bluetooth LE Remote
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

// Remote
#define TEMP_INTERVAL       2000      // temperature update interval in ms

// Bluetooth LE handler
#define BLE_TX_POWER        0         // TX power in dBm: -20, -14, -8, -4, 0, 1 .. 5
#define BLE_MEMHEAP_SIZE    (6*1024)  // RAM for the WCH BLE library

// Bluetooth LE host
#define BLE_NAME            "CH573 Remote"  // device name (max. 20 characters)
#define BLE_APPEARANCE      0x0000    // GAP appearance (0: unknown)
#define BLE_ATTRIBUTES      12        // max number of GATT attributes
#define BLE_ATT_MTU         64       // max ATT MTU supported

// USB device descriptor
#define USB_VENDOR_ID       0x16C0    // VID (shared www.voti.nl)
#define USB_PRODUCT_ID      0x27DD    // PID (shared CDC-ACM)
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)
#define USB_LANGUAGE        0x0409    // US English

// USB configuration descriptor
#define USB_MAX_POWER_mA    100       // max power in mA

// USB descriptor strings
#define MANUF_STR           "wagiminator"
#define PROD_STR            "CH573F Bluetooth LE Remote"
#define SERIAL_STR          "CH573BLEREMOTE"
#define INTERF_STR          "CDC-Serial"
