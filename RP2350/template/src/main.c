// ===================================================================================
// Project:   Example for RP2350
// Version:   v1.0
// Year:      2026
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
// - Make sure GCC toolchain (gcc-arm-none-eabi), Python3 and rpprog are installed
//   (pip install rpprog).
// - Connect the board via USB while holding the BOOTSEL button.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"
#include "gpio.h"

#define PIN_LED   GP25          // LED on Raspberry Pi Pico 2

// ===================================================================================
// Main Function
// ===================================================================================
int main (void) {
  // Setup
  PIN_output(PIN_LED);

  // Loop
  while(1) {
    DLY_ms(100);
    PIN_toggle(PIN_LED);
  }
}
