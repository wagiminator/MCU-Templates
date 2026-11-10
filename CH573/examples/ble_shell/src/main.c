// ===================================================================================
// Project:   CH573F Examples - Bluetooth LE Shell
// Version:   v1.0
// Year:      2026
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Command line via Bluetooth LE. The board advertises as BLE_NAME and offers a
// virtual serial port (Nordic UART Service), which can be used with smartphone
// apps like "Serial Bluetooth Terminal" (Android), "Bluefruit Connect" (Android/
// iOS) or "nRF Toolbox" (Android/iOS), or with a Web Bluetooth terminal in Chrome.
// After connecting, the same commands as in the USB shell are available (type
// "help"), plus:
//
//   msg <text>    send text to the PC (USB serial port)
//
// Text lines typed in the USB terminal on the PC are sent to the smartphone, so the
// example also works as a simple USB <-> Bluetooth LE message bridge. Pressing a
// key sends a message to the smartphone. Connection events are reported via USB.
// While no device is connected, the LED flashes briefly every 2 seconds.
//
// Tested template functions: BLE handler (WCH BLE library, TMOS, calibration),
// BLE host API (advertising, GATT server, notifications, MTU), BLE UART library,
// USB device (CDC) at the same time, keys, ADC.
//
// Settings in config.h: BLE_NAME, BLE_TX_POWER, BLE library settings, SYS_CLK32K.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "board.h"                        // board support (LED, keys, ADC, info)
#include "term.h"                         // terminal via USB serial port
#include "shell.h"                        // command line interpreter
#include "ble_uart.h"                     // BLE UART, BLE host API, BLE handler

char    usbLine[64];                      // line typed in USB terminal
uint8_t usbLen = 0;

// Print formatted string via BLE UART
#define BLE_printf(f, ...)  (printF(BUART_write, f, ##__VA_ARGS__), BUART_flush())

// ===================================================================================
// Additional Shell Commands (called by the shell)
// ===================================================================================
uint8_t SHELL_command(const char* cmd, const char* arg) {
  if(!strcmp(cmd, "help")) {
    BLE_printf("  msg <text>           send text to PC\n");
    return 1;
  }
  if(!strcmp(cmd, "info")) {
    uint8_t mac[6];
    BLE_getMAC(mac);
    BLE_printf("BLE address: %02x:%02x:%02x:%02x:%02x:%02x\nATT MTU:     %d\n",
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], BLE_getMTU());
    return 1;
  }
  if(!strcmp(cmd, "msg")) {
    TERM_printf("Smartphone: %s\n", arg);
    BLE_printf("Sent to PC\n");
    return 1;
  }
  return 0;
}

// ===================================================================================
// BLE Event Handler (called by the BLE handler)
// ===================================================================================
void BLE_EVENT_callback(uint8_t event) {
  switch(event) {
    case BLE_EVENT_CONNECTED:    TERM_print("[BLE] device connected\n"); break;
    case BLE_EVENT_DISCONNECTED: TERM_print("[BLE] device disconnected, advertising again\n"); break;
    case BLE_EVENT_MTU_CHANGED:  TERM_printf("[BLE] MTU changed to %d\n", BLE_getMTU()); break;
    default: break;
  }
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  uint8_t  listening = 0, connected = 0, ok;
  uint32_t lastFlash = 0;
  uint8_t  mac[6];

  // Setup
  BOARD_init();                           // init LED, keys, millis, ADC
  TERM_init();                            // init USB serial port
  ok = BUART_init();                      // init BLE and start advertising
  if(!ok) while(1) { LED_toggle(); DLY_ms(100); }   // error: blink LED quickly
  SHELL_init(BUART_write, BUART_flush, 0, "> ");

  // Loop
  while(1) {
    // USB terminal opened: show status
    if(TERM_connected() != connected) {
      connected = TERM_connected();
      if(connected) {
        BLE_delay(100);
        BLE_getMAC(mac);
        TERM_print("\n=== CH573F - Bluetooth LE Shell ===\n");
        TERM_printf("Advertising as '%s' (%02x:%02x:%02x:%02x:%02x:%02x)\n", BLE_NAME,
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        TERM_print("Connect with a BLE terminal app. Lines typed here are sent to the smartphone.\n");
      }
    }

    // Device connected and listening for notifications: send welcome message
    if(BUART_connected() != listening) {
      listening = !listening;
      if(listening) {
        LED_off();
        BLE_printf("=== %s - BLE Shell ===\nType 'help' for a list of commands.\n", BOARD_NAME);
        SHELL_prompt();
      }
    }

    // Characters received via Bluetooth: process in shell
    while(BUART_available()) SHELL_input(BUART_read());

    // Characters received via USB: collect line and send it to the smartphone
    if(TERM_available()) {
      char c = TERM_read();
      if((c == '\r') || (c == '\n')) {
        if(usbLen) {
          usbLine[usbLen] = 0;
          TERM_print("\n");
          if(listening) BLE_printf("\nPC: %s\n", usbLine);
          else TERM_print("(no device connected)\n");
          usbLen = 0;
        }
      }
      else if(((c == 0x08) || (c == 0x7F)) && usbLen) {
        usbLen--;
        TERM_print("\b \b");              // delete character in terminal
      }
      else if((c >= ' ') && (usbLen < sizeof(usbLine) - 1)) {
        usbLine[usbLen++] = c;
        TERM_putchar(c);                  // echo
        TERM_flush();
      }
    }

    // Key pressed: send message
    for(uint8_t k = BTN_BOOT; k <= BTN_KEY; k++) {
      if(BTN_pressed(k)) {
        TERM_printf("[%s pressed]\n", k ? "KEY" : "BOOT key");
        if(listening) {
          BLE_printf("\n[%s pressed]\n", k ? "KEY" : "BOOT key");
          SHELL_prompt();
        }
      }
    }

    // LED: short flash every 2 seconds while advertising
    if(!BLE_connected() && (BLE_millis() - lastFlash >= 2000)) {
      LED_flash(50);
      lastFlash = BLE_millis();
    }
    LED_update();
    BLE_poll();                           // process BLE events
  }
}
