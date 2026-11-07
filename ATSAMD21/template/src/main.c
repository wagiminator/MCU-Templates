// ===================================================================================
// Project:   Example for ATSAMD21
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
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with tinyocd is
//   installed. If necessary, grant access permissions for the SWD programmer.
// - Connect the SWD programmer (e.g. CMSIS-DAP) to the MCU board.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"
#include "gpio.h"

#define PIN_LED   PA8           // LED on SAMD21E DevBoard (Arduino Zero: PA17)

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
