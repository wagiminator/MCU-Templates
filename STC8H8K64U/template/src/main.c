// ===================================================================================
// Project:   Example for STC8H8K64U
// Version:   v1.0
// Year:      2023
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Blink example.
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with stc8usb is installed.
// - Press and hold BOOT button while connecting the MCU board to your PC via USB.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "gpio.h"                       // GPIO functions
#include "delay.h"                      // delay functions

#define PIN_LED   P34                   // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================

void main() {
  // Setup
  P_SW2 |= 0x80;                        // enable access to extended registers (MUST!)
  PIN_output(PIN_LED);                  // set LED pin as output

  // Loop
  while (1) {
    PIN_toggle(PIN_LED);                // toggle LED
    DLY_ms(100);                        // wait 100ms
  }
}
