// ===================================================================================
// Project:   CH573F Examples - Bluetooth LE Remote (GATT Server)
// Version:   v1.0
// Year:      2026
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Shows how to build an own GATT server with the BLE host API. The board advertises
// as BLE_NAME and offers two services, which can be used with a generic BLE app
// like "nRF Connect" (Android/iOS) or "LightBlue" (iOS/Android):
//
//   LED & Keys service (custom 128-bit UUID, see below):
//     LED          (read, write)   0x00: LED off, 0x01: LED on
//     Keys         (read, notify)  bit 0: BOOT key, bit 1: KEY (1 = pressed),
//                                  a notification is sent with each change
//
//   Environmental Sensing service (0x181A, Bluetooth standard):
//     Temperature  (read, notify)  0x2A6E, sint16 in 0.01 degrees Celsius,
//                                  updated every TEMP_INTERVAL milliseconds
//
// Connection events and LED changes are reported via the USB serial port. While no
// device is connected, the LED flashes briefly every 2 seconds.
//
// Tested template functions: BLE handler, BLE host API (16- and 128-bit services
// and characteristics, read/write callbacks, notifications, events), USB device
// (CDC), keys, temperature sensor.
//
// Settings in config.h: BLE_NAME, TEMP_INTERVAL, BLE_TX_POWER, BLE library settings.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "board.h"                        // board support (LED, keys, ADC)
#include "term.h"                         // terminal via USB serial port
#include "ble_host.h"                     // BLE host API (and BLE handler)

// Custom 128-bit UUIDs (LSB first): 4fe5xxxx-0b6d-4d1c-9a8e-5b4c3a2f1e0d
#define UUID128(n) 0x0d,0x1e,0x2f,0x3a,0x4c,0x5b,0x8e,0x9a,0x1c,0x4d,0x6d,0x0b,(n)&0xFF,(n)>>8,0xe5,0x4f
const uint8_t UUID_SERVICE[16] = { UUID128(0x0001) };
const uint8_t UUID_LED[16]     = { UUID128(0x0002) };
const uint8_t UUID_KEYS[16]    = { UUID128(0x0003) };

uint8_t  ledValue[1];                     // value buffers of the characteristics
uint8_t  keysValue[1];
uint8_t  tempValue[2];
uint16_t keysHandle, tempHandle;          // handles for notifications
volatile uint8_t ledChanged;              // LED written by the client

// ===================================================================================
// Callbacks
// ===================================================================================

// LED characteristic written by the client
void ledWritten(uint16_t handle, const uint8_t* data, uint16_t len) {
  (void)handle;
  if(len) {
    LED_set(data[0] != 0);
    ledChanged = 1;
  }
}

// BLE events
void BLE_EVENT_callback(uint8_t event) {
  switch(event) {
    case BLE_EVENT_CONNECTED:    TERM_print("[BLE] device connected\n"); break;
    case BLE_EVENT_DISCONNECTED: TERM_print("[BLE] device disconnected, advertising again\n");
                                 LED_off(); break;
    case BLE_EVENT_MTU_CHANGED:  TERM_printf("[BLE] MTU changed to %d\n", BLE_getMTU()); break;
    default: break;
  }
}

// Update temperature value (0.01 C)
void updateTemp(void) {
  int16_t t = BOARD_temp() * 10;
  tempValue[0] = (uint8_t)t; tempValue[1] = (uint8_t)(t >> 8);
  BLE_setValue(tempHandle, tempValue, 2);
  BLE_notify(tempHandle, tempValue, 2);
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  uint8_t  connected = 0, keys = 0, k;
  uint32_t lastFlash = 0, lastTemp = 0;
  uint8_t  mac[6];

  // Setup
  BOARD_init();                           // init LED, keys, millis, ADC
  TERM_init();                            // init USB serial port
  if(!BLE_init()) while(1) { LED_toggle(); DLY_ms(100); }   // error: blink LED quickly

  // GATT server: services and characteristics
  BLE_addService128(UUID_SERVICE);
  BLE_addChar128(UUID_LED, BLE_READ | BLE_WRITE | BLE_WRITE_NORESP, ledValue, 1, 1, ledWritten);
  keysHandle = BLE_addChar128(UUID_KEYS, BLE_READ | BLE_NOTIFY, keysValue, 1, 1, 0);
  BLE_addService16(0x181A);               // Environmental Sensing
  tempHandle = BLE_addChar16(0x2A6E, BLE_READ | BLE_NOTIFY, tempValue, 2, 2, 0);
  updateTemp();

  // Start advertising with name and service UUID
  BLE_advertiseName(BLE_NAME, UUID_SERVICE);

  // Loop
  while(1) {
    BLE_poll();                           // process BLE events

    // USB terminal opened: show status
    if(TERM_connected() != connected) {
      connected = TERM_connected();
      if(connected) {
        BLE_delay(100);
        BLE_getMAC(mac);
        TERM_print("\n=== CH573F - Bluetooth LE Remote ===\n");
        TERM_printf("Advertising as '%s' (%02x:%02x:%02x:%02x:%02x:%02x)\n", BLE_NAME,
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        TERM_print("Connect with a BLE app (e.g. nRF Connect).\n");
      }
    }

    // LED changed by the client
    if(ledChanged) {
      ledChanged = 0;
      ledValue[0] = LED_state();
      TERM_printf("[BLE] LED switched %s\n", LED_state() ? "on" : "off");
    }

    // Keys changed: update value and notify
    k = BTN_read(BTN_BOOT) | (BTN_read(BTN_KEY) << 1);
    if(k != keys) {
      keys = k;
      keysValue[0] = keys;
      BLE_setValue(keysHandle, keysValue, 1);
      BLE_notify(keysHandle, keysValue, 1);
      TERM_printf("Keys: BOOT %s, KEY %s\n", (keys & 1) ? "pressed" : "released",
                  (keys & 2) ? "pressed" : "released");
      BLE_delay(20);                      // debounce
    }

    // Temperature update
    if(BLE_millis() - lastTemp >= TEMP_INTERVAL) {
      lastTemp = BLE_millis();
      updateTemp();
    }

    // LED: short flash every 2 seconds while advertising
    if(!BLE_connected() && (BLE_millis() - lastFlash >= 2000)) {
      LED_flash(50);
      lastFlash = BLE_millis();
    }
    LED_update();
  }
}
