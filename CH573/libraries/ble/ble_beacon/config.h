// ===================================================================================
// User Configurations
// ===================================================================================

#pragma once

// Pin definitions
#define PIN_LED             PA8       // pin connected to LED

// System
#define SYS_CLK32K          0         // 32kHz clock: 0: internal LSI, 1: LSE crystal (PA10/PA11)

// BLE handler
#define BLE_TX_POWER        0         // TX power in dBm: -20, -14, -8, -4, 0, 1 .. 5
#define BLE_MEMHEAP_SIZE    (6*1024)  // RAM for the BLE library

// BLE host
#define BLE_NAME            "CH573 Beacon" // device name
#define BLE_ATTRIBUTES      4         // max number of GATT attributes
#define BLE_ATT_MTU         23        // max ATT MTU supported

// Beacon
#define BEACON_INTERVAL     500       // advertising interval in ms
