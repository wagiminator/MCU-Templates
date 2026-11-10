// ===================================================================================
// Bluetooth LE Host API (GAP Peripheral, GATT Server) for CH573/CH571        * v1.0 *
// ===================================================================================
//
// This is the unified Bluetooth LE host API of the MCU templates, which is used by
// the MCU-independent application libraries (e.g. ble_uart, ble_beacon). The device
// acts as a peripheral: it advertises, accepts one connection from a central (e.g.
// a smartphone) and offers its data via a GATT server. Everything is processed in
// BLE_poll(), which has to be called regularly.
//
// On the ESP32-C3, this API is implemented by a small own host stack (ble_host.c),
// which talks to the controller via HCI. On the CH573, the host (GAP, L2CAP, ATT,
// GATT) is part of the precompiled WCH BLE library, so the API is implemented by
// the MCU-specific BLE handler (ble_handler.c), which maps it to the WCH stack.
//
// General functions available:
// ----------------------------
// BLE_init()                     init controller and host (must be called first)
// BLE_poll()                     process events and data (call regularly!)
// BLE_connected()                check if a central is connected
// BLE_disconnect()               terminate connection
// BLE_getMTU()                   get negotiated ATT MTU (max data = MTU - 3)
//
// Advertising functions available:
// --------------------------------
// BLE_advertise(adv,len,sr,srlen,ms,type)  start advertising with given data,
//                                interval (ms) and type (BLE_ADV_CONNECTABLE,
//                                BLE_ADV_SCANNABLE, BLE_ADV_NONCONN)
// BLE_advertiseName(name, uuid128) start connectable advertising with flags, name
//                                and 128-bit service UUID (NULL: none)
// BLE_advStop()                  stop advertising
//
// GATT server functions available:
// --------------------------------
// BLE_addService16(uuid)         add primary service with 16-bit UUID
// BLE_addService128(uuid)        add primary service with 128-bit UUID (LSB first)
// BLE_addChar16(uuid, props, buf, len, max, cb)   add characteristic with 16-bit UUID
// BLE_addChar128(uuid, props, buf, len, max, cb)  add characteristic, 128-bit UUID
//                                (buf: value buffer, len: initial length, max: size
//                                of buffer, cb: write callback or NULL),
//                                returns value handle
// BLE_setValue(handle, data, len)  change value of characteristic
// BLE_notify(handle, data, len)  send notification, returns 1 if sent
// BLE_notifyEnabled(handle)      check if client has enabled notifications
//
// BLE_EVENT_callback(event)      optional user function (BLE_EVENT_CONNECTED, ...)
//
// Characteristic properties (combine by OR):
// ------------------------------------------
// BLE_READ, BLE_WRITE_NORESP, BLE_WRITE, BLE_NOTIFY
//
// Notes:
// ------
// - Services and characteristics must be added after BLE_init() and before
//   advertising is started.
// - No pairing/bonding (pairing requests are answered with "just works", but keys
//   are not stored), no client role.
// - Advertising is restarted automatically after a disconnection.
// - Handles are the numbers of the attributes in the internal table (1, 2, ..),
//   not the ATT handles, which are assigned by the WCH stack.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "ble_handler.h"

// ===================================================================================
// BLE Host Parameters (defaults, can be changed in config.h)
// ===================================================================================
#ifndef BLE_NAME
  #define BLE_NAME          "CH573"       // default device name
#endif
#ifndef BLE_APPEARANCE
  #define BLE_APPEARANCE    0x0000        // GAP appearance (0: unknown)
#endif
#ifndef BLE_ATTRIBUTES
  #define BLE_ATTRIBUTES    24            // max number of GATT attributes
#endif
#ifndef BLE_ATT_MTU
  #define BLE_ATT_MTU       247           // max ATT MTU supported
#endif

// ===================================================================================
// BLE Definitions
// ===================================================================================
// Characteristic properties
#define BLE_READ            0x02
#define BLE_WRITE_NORESP    0x04
#define BLE_WRITE           0x08
#define BLE_NOTIFY          0x10

// Advertising types
#define BLE_ADV_CONNECTABLE 0x00          // ADV_IND
#define BLE_ADV_SCANNABLE   0x02          // ADV_SCAN_IND
#define BLE_ADV_NONCONN     0x03          // ADV_NONCONN_IND

// Events
#define BLE_EVENT_CONNECTED     1
#define BLE_EVENT_DISCONNECTED  2
#define BLE_EVENT_MTU_CHANGED   3

// Write callback
typedef void (*BLE_WRITE_CB)(uint16_t handle, const uint8_t* data, uint16_t len);

// ===================================================================================
// BLE Host Functions
// ===================================================================================
uint8_t  BLE_init(void);
void     BLE_poll(void);
uint8_t  BLE_connected(void);
void     BLE_disconnect(void);
uint16_t BLE_getMTU(void);

uint8_t  BLE_advertise(const uint8_t* adv, uint8_t len, const uint8_t* sr, uint8_t srlen,
                       uint16_t interval, uint8_t type);
uint8_t  BLE_advertiseName(const char* name, const uint8_t* uuid128);
void     BLE_advStop(void);

uint16_t BLE_addService16(uint16_t uuid);
uint16_t BLE_addService128(const uint8_t* uuid);
uint16_t BLE_addChar16(uint16_t uuid, uint8_t props, uint8_t* buf, uint16_t len,
                       uint16_t max, BLE_WRITE_CB cb);
uint16_t BLE_addChar128(const uint8_t* uuid, uint8_t props, uint8_t* buf, uint16_t len,
                        uint16_t max, BLE_WRITE_CB cb);
void     BLE_setValue(uint16_t handle, const void* data, uint16_t len);
uint8_t  BLE_notify(uint16_t handle, const void* data, uint16_t len);
uint8_t  BLE_notifyEnabled(uint16_t handle);

void     BLE_EVENT_callback(uint8_t event);   // optional user function (weak)

#ifdef __cplusplus
};
#endif
