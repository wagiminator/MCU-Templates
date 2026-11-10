// ===================================================================================
// Bluetooth LE Beacon (iBeacon and Eddystone)                                * v1.0 *
// ===================================================================================
// 2025 by Stefan Wagner:   https://github.com/wagiminator

#include <string.h>
#include "ble_beacon.h"

// Init BLE
uint8_t BEACON_init(void) {
  return BLE_init();
}

// Send any advertising data (non-connectable)
uint8_t BEACON_raw(const uint8_t* data, uint8_t len) {
  return BLE_advertise(data, len, 0, 0, BEACON_INTERVAL, BLE_ADV_NONCONN);
}

// Send Apple iBeacon
uint8_t BEACON_iBeacon(const uint8_t* uuid, uint16_t major, uint16_t minor, int8_t power) {
  uint8_t d[30] = {
    0x02, 0x01, 0x06,                               // flags
    0x1A, 0xFF, 0x4C, 0x00, 0x02, 0x15              // manufacturer data: Apple, iBeacon
  };
  memcpy(d + 9, uuid, 16);
  d[25] = major >> 8; d[26] = major;
  d[27] = minor >> 8; d[28] = minor;
  d[29] = power;
  return BEACON_raw(d, 30);
}

// Send Eddystone frame (service data of service 0xFEAA)
static uint8_t BEACON_eddystone(const uint8_t* frame, uint8_t len) {
  uint8_t d[31] = {
    0x02, 0x01, 0x06,                               // flags
    0x03, 0x03, 0xAA, 0xFE,                         // complete list of 16-bit UUIDs
    0x00, 0x16, 0xAA, 0xFE                          // service data: length set below
  };
  if(len > 20) len = 20;
  d[7] = len + 3;
  memcpy(d + 11, frame, len);
  return BEACON_raw(d, 11 + len);
}

// Send Eddystone-URL beacon
uint8_t BEACON_eddystoneURL(const char* url, int8_t power) {
  static const char* prefix[4] = {"http://www.", "https://www.", "http://", "https://"};
  static const char* suffix[14] = {".com/", ".org/", ".edu/", ".net/", ".info/", ".biz/",
                                   ".gov/", ".com", ".org", ".edu", ".net", ".info",
                                   ".biz", ".gov"};
  uint8_t f[20] = {0x10, (uint8_t)power};           // frame type URL, TX power
  uint8_t n = 2, i;
  f[n++] = 3;                                       // default: https://
  for(i=0; i<4; i++) {
    uint8_t l = strlen(prefix[i]);
    if(!strncmp(url, prefix[i], l)) { f[2] = i; url += l; break; }
  }
  while(*url && (n < 20)) {                         // encode URL with suffix codes
    for(i=0; i<14; i++) {
      uint8_t l = strlen(suffix[i]);
      if(!strncmp(url, suffix[i], l)) { f[n++] = i; url += l; break; }
    }
    if(i == 14) f[n++] = *url++;
  }
  return BEACON_eddystone(f, n);
}

// Send Eddystone-UID beacon
uint8_t BEACON_eddystoneUID(const uint8_t* ns, const uint8_t* instance, int8_t power) {
  uint8_t f[20] = {0x00, (uint8_t)power};           // frame type UID, TX power
  memcpy(f + 2, ns, 10);
  memcpy(f + 12, instance, 6);
  return BEACON_eddystone(f, 20);
}

// Stop sending
void BEACON_stop(void) {
  BLE_advStop();
}
