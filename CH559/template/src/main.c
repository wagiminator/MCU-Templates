// ===================================================================================
// Project:   Example for CH559
// Version:   v1.0
// Year:      2022
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
// References:
// -----------
// - Deqing Sun: https://github.com/DeqingSun/ch55xduino
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with PyUSB is installed. In addition, Linux
//   requires access rights to the USB bootloader.
// - Press the BOOT button on the MCU board and keep it pressed while connecting it
//   via USB to your PC.
// - Run 'make flash' immediatly afterwards.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include "system.h"                               // system functions
#include "delay.h"                                // delay functions
#include "gpio.h"                                 // GPIO functions

// Pin definitions
#define PIN_LED   P25                             // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  CLK_config();                                   // configure system clock
  PIN_output(PIN_LED);                            // set LED pin as output

  // Loop
  while(1) {
    PIN_toggle(PIN_LED);                          // toggle LED
    DLY_ms(100);                                  // wait a bit
  }
}
