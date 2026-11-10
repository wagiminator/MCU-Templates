// ===================================================================================
// User Configurations for Bluetooth LE Beacon
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

// Beacon
#define BEACON_URL          "https://github.com"  // max. 17 chars after prefix
#define BEACON_UUID         0xE2,0xC5,0x6D,0xB5,0xDF,0xFB,0x48,0xD2,0xB0,0x60,0xD0,0xF5,0xA7,0x10,0x96,0xE0
#define BEACON_MAJOR        1         // iBeacon major number
#define BEACON_MINOR        2         // iBeacon minor number
#define BEACON_NAMESPACE    0x43,0x48,0x35,0x37,0x33,0x2D,0x42,0x4C,0x45,0x21 // 10 bytes
#define BEACON_POWER        -59       // measured RSSI at 1m (iBeacon) in dBm
#define BEACON_INTERVAL     500       // advertising interval in ms
#define BEACON_SENSOR_UPDATE 5000     // sensor beacon update interval in ms

// Bluetooth LE handler
#define BLE_TX_POWER        0         // TX power in dBm: -20, -14, -8, -4, 0, 1 .. 5
#define BLE_MEMHEAP_SIZE    (6*1024)  // RAM for the WCH BLE library

// Bluetooth LE host
#define BLE_NAME            "CH573 Beacon"  // device name (max. 20 characters)
#define BLE_APPEARANCE      0x0000    // GAP appearance (0: unknown)
#define BLE_ATTRIBUTES      4        // max number of GATT attributes
#define BLE_ATT_MTU         23       // max ATT MTU supported

// USB device descriptor
#define USB_VENDOR_ID       0x16C0    // VID (shared www.voti.nl)
#define USB_PRODUCT_ID      0x27DD    // PID (shared CDC-ACM)
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)
#define USB_LANGUAGE        0x0409    // US English

// USB configuration descriptor
#define USB_MAX_POWER_mA    100       // max power in mA

// USB descriptor strings
#define MANUF_STR           "wagiminator"
#define PROD_STR            "CH573F Bluetooth LE Beacon"
#define SERIAL_STR          "CH573BLEBEACON"
#define INTERF_STR          "CDC-Serial"
