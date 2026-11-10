// ===================================================================================
// Project:   CH573F Examples - Bluetooth LE Beacon
// Version:   v1.0
// Year:      2026
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Turns the board into a Bluetooth LE beacon. Each press on the BOOT key switches
// to the next beacon type (the LED flashes 1..4 times to show the type):
//
//   1. Eddystone-URL     sends BEACON_URL
//   2. iBeacon           sends BEACON_UUID, BEACON_MAJOR, BEACON_MINOR
//   3. Eddystone-UID     sends BEACON_NAMESPACE, instance = MAC address
//   4. Sensor beacon     sends the name "CH573-Sens" and the chip temperature, a
//                        counter and the state of KEY as manufacturer specific data
//                        (company ID 0xFFFF = test), updated every
//                        BEACON_SENSOR_UPDATE milliseconds
//
// The beacons can be seen with smartphone apps like "nRF Connect" or "Beacon
// Scanner". The current type and the data sent are reported via the USB serial port.
//
// Tested template functions: BLE handler (WCH BLE library), BLE host API
// (non-connectable advertising, live update of advertising data), beacon library,
// USB serial port, keys, temperature sensor.
//
// Settings in config.h: BEACON_URL, BEACON_UUID, BEACON_MAJOR, BEACON_MINOR,
// BEACON_NAMESPACE, BEACON_POWER, BEACON_INTERVAL, BEACON_SENSOR_UPDATE, BLE_TX_POWER.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "board.h"                        // board support (LED, keys, ADC)
#include "term.h"                         // terminal via USB serial port
#include "ble_beacon.h"                   // beacon, BLE host API, BLE handler

#define TYPES         4                   // number of beacon types

const uint8_t UUID[16]      = { BEACON_UUID };
const uint8_t NAMESPACE[10] = { BEACON_NAMESPACE };
const char*   TYPENAME[TYPES] = { "Eddystone-URL", "iBeacon", "Eddystone-UID", "Sensor beacon" };

uint8_t  instance[6];                     // Eddystone-UID instance (MAC address)
uint16_t counter = 0;                     // sensor beacon counter

// Print bytes as hex string
void printHex(const uint8_t* data, uint8_t len) {
  while(len--) TERM_printf("%02x", *data++);
}

// Send sensor beacon with current values
void sendSensor(uint8_t verbose) {
  int16_t t = BOARD_temp();
  uint8_t d[24] = {
    0x02, 0x01, 0x06,                                   // flags
    0x0B, 0x09, 'C','H','5','7','3','-','S','e','n','s',// complete local name
    0x08, 0xFF, 0xFF, 0xFF,                             // manufacturer data, ID 0xFFFF
    (uint8_t)t, (uint8_t)(t >> 8),                      // temperature (1/10 C, LE)
    (uint8_t)counter, (uint8_t)(counter >> 8),          // counter (LE)
    BTN_read(BTN_KEY)                                   // key state
  };
  BEACON_raw(d, sizeof(d));
  if(verbose) {
    TERM_printf("  #%u: temperature %s%d.%d C, key %d, data: ",
               counter, t < 0 ? "-" : "", (t < 0 ? -t : t) / 10, (t < 0 ? -t : t) % 10, BTN_read(BTN_KEY));
    printHex(d + 17, 7);
    TERM_print("\n");
  }
  counter++;
}

// Start beacon of given type
void startBeacon(uint8_t type) {
  TERM_printf("\nBeacon type %d: %s\n", type + 1, TYPENAME[type]);
  switch(type) {
    case 0:
      BEACON_eddystoneURL(BEACON_URL, BEACON_POWER + 41);   // power at 0m
      TERM_printf("  URL: %s\n", BEACON_URL);
      break;
    case 1:
      BEACON_iBeacon(UUID, BEACON_MAJOR, BEACON_MINOR, BEACON_POWER);
      TERM_print("  UUID: ");
      printHex(UUID, 16);
      TERM_printf(", major: %d, minor: %d\n", BEACON_MAJOR, BEACON_MINOR);
      break;
    case 2:
      BEACON_eddystoneUID(NAMESPACE, instance, BEACON_POWER + 41);
      TERM_print("  Namespace: ");
      printHex(NAMESPACE, 10);
      TERM_print(", instance: ");
      printHex(instance, 6);
      TERM_print("\n");
      break;
    default:
      sendSensor(1);
      break;
  }

  // Show type with LED flashes
  for(uint8_t i = 0; i <= type; i++) {
    LED_on();  BLE_delay(100);
    LED_off(); BLE_delay(200);
  }
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  uint8_t  type = 0, connected = 0;
  uint32_t last;

  // Setup
  BOARD_init();                           // init LED, keys, millis, ADC
  TERM_init();                            // init USB serial port
  if(!BEACON_init()) {
    while(1) { LED_toggle(); DLY_ms(100); }   // error: blink LED quickly
  }
  BLE_getMAC(instance);
  startBeacon(type);

  // Loop
  last = BLE_millis();
  while(1) {
    BLE_poll();                           // process BLE events

    // USB terminal opened: show status
    if(TERM_connected() != connected) {
      connected = TERM_connected();
      if(connected) {
        BLE_delay(100);
        TERM_print("\n=== CH573F - Bluetooth LE Beacon ===\nBLE address: ");
        printHex(instance, 6);
        TERM_printf("\nAdvertising interval: %d ms. Press BOOT key to change the beacon type.\n",
                    BEACON_INTERVAL);
        TERM_printf("Current beacon type %d: %s\n", type + 1, TYPENAME[type]);
      }
    }

    if(BTN_pressed(BTN_BOOT)) {           // BOOT key: next beacon type
      if(++type >= TYPES) type = 0;
      startBeacon(type);
      last = BLE_millis();
    }
    if((type == 3) && (BLE_millis() - last >= BEACON_SENSOR_UPDATE)) {
      sendSensor(1);                      // update sensor beacon
      LED_flash(30);
      last = BLE_millis();
    }
    LED_update();
  }
}
