// ===================================================================================
// Bluetooth LE Beacon (iBeacon and Eddystone)                                * v1.0 *
// ===================================================================================
//
// Turns the device into a Bluetooth LE beacon, which only sends advertising
// packets (no connection).
//
// Functions available:
// --------------------
// BEACON_init()                  init BLE (must be called first)
// BEACON_iBeacon(uuid,maj,min,p) send Apple iBeacon (16-byte UUID MSB first,
//                                major, minor, measured power at 1m in dBm)
// BEACON_eddystoneURL(url, p)    send Eddystone-URL beacon (e.g. "https://abc.de",
//                                URL max. 17 characters after the prefix,
//                                p: power at 0m in dBm)
// BEACON_eddystoneUID(ns,inst,p) send Eddystone-UID beacon (10-byte namespace,
//                                6-byte instance)
// BEACON_raw(data, len)          send any advertising data (max. 31 bytes)
// BEACON_stop()                  stop sending
//
// Notes:
// ------
// - The advertising interval is set by BEACON_INTERVAL in config.h (ms).
//
// 2025 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ble_host.h"

#ifndef BEACON_INTERVAL
  #define BEACON_INTERVAL   500       // advertising interval in ms
#endif

uint8_t BEACON_init(void);
uint8_t BEACON_iBeacon(const uint8_t* uuid, uint16_t major, uint16_t minor, int8_t power);
uint8_t BEACON_eddystoneURL(const char* url, int8_t power);
uint8_t BEACON_eddystoneUID(const uint8_t* ns, const uint8_t* instance, int8_t power);
uint8_t BEACON_raw(const uint8_t* data, uint8_t len);
void    BEACON_stop(void);

#ifdef __cplusplus
};
#endif
