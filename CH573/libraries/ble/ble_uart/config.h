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
#define BLE_MEMHEAP_SIZE    (8*1024)  // RAM for the BLE library

// BLE host
#define BLE_NAME            "CH573 UART" // device name (max. 20 characters)
#define BLE_APPEARANCE      0x0000    // GAP appearance (0: unknown)
#define BLE_ATTRIBUTES      12        // max number of GATT attributes
#define BLE_ATT_MTU         247       // max ATT MTU supported

// BLE UART
#define BUART_RXBUF         256       // size of receive ring buffer
