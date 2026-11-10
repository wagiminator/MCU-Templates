// ===================================================================================
// Project:   Raspberry Pi Pico Examples - USB Shell
// Version:   v1.0
// Year:      2026
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Command line via the native USB port. The RP2040 appears as a virtual serial port
// on the PC (USB CDC, e.g. /dev/ttyACM0 or COMx), which can be opened with any
// terminal program (e.g. "make monitor", PuTTY, Arduino serial monitor, web serial
// terminal). A welcome message is shown when the terminal is opened. Type "help" to
// get a list of commands. Pressing the BOOTSEL button toggles the LED and sends a
// message.
//
// Tested template functions: USB device (CDC), GPIO, BOOTSEL button, ADC,
// temperature sensor, VSYS measurement, timer (millis), ring oscillator, unique ID,
// reset reasons, software reset (watchdog) and bootloader entry (boot ROM).
//
// Settings in config.h: F_CPU, PIN_LED, USB descriptors.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "board.h"                        // board support (LED, button, ADC, info)
#include "term.h"                         // terminal via USB serial port
#include "shell.h"                        // command line interpreter

// Print welcome message
void printWelcome(void) {
  TERM_print("\n=== Raspberry Pi Pico - USB Shell ===\n");
  BOARD_printInfo(TERM_putchar);
  TERM_print("Type 'help' for a list of commands.\n");
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  uint8_t connected = 0;

  // Setup
  BOARD_init();                           // init LED, millis, ADC
  TERM_init();                            // init USB serial port
  SHELL_init(TERM_putchar, TERM_flush, 1, "pico> ");
  LED_flash(200);                         // short sign of life

  // Loop
  while(1) {
    // Terminal opened: show welcome message
    if(TERM_connected() != connected) {
      connected = TERM_connected();
      if(connected) {
        DLY_ms(100);                      // give the terminal some time
        printWelcome();
        SHELL_prompt();
      }
    }

    // Characters received via USB: feed into shell
    while(TERM_available()) SHELL_input(TERM_read());

    // BOOTSEL button pressed: toggle LED and send message
    if(BTN_pressed()) {
      LED_toggle();
      TERM_printf("\n[BOOTSEL button pressed, LED %s]\n", LED_state() ? "on" : "off");
      SHELL_prompt();
    }

    LED_update();                         // switch off LED after flash
  }
}
