// ===================================================================================
// Basic TM1637 4-Digit 7-Segment LED Display Functions                       * v1.0 *
// ===================================================================================
//
// Collection of the most necessary functions for controlling a 4-digit 7-segment LED
// display via TM1637 and a 2-wire interface (bit-banging, not I2C compatible).
// Same API as the TM1650 library.
//
// Functions available:
// --------------------
// TM_init()                      init TM1637 4-digit display
// TM_setBrightness(bright)       set display brightness (1-8), 0: display off
// TM_printSegment(digit, code)   print segment code to specified digit
// TM_printDigit(digit, val)      print number (0-15) to the specified digit
// TM_printH(val)                 print hexadecimal value on display
// TM_printD(val)                 print positive decimal value on display (0-9999)
// TM_print(val, dotpos)          print value (-999 - 9999) with decimal at dotpos
//
// Notes:
// ------
// - The digits are counted from right to left starting with 0.
// - dotpos > 3 means no decimal point. On clock displays bit 7 of digit 2 is the colon.
// - CLK and DIO need pull-up resistors (most modules have them on board).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"
#include "system.h"
#ifdef __SDCC
#include "delay.h"
#endif

// TM1637 parameters
#define TM_PIN_CLK        PA4                     // pin connected to CLK
#define TM_PIN_DIO        PA5                     // pin connected to DIO
#define TM_BRIGHT         4                       // initial display brightness (1-8)

// TM1637 character segment codes
#define TM_CHAR_NEG       0x40                    // "-"
#define TM_CHAR_DEG       0x63                    // "°"
#define TM_CHAR_A         0x77                    // "A"
#define TM_CHAR_C         0x39                    // "C"
#define TM_CHAR_H         0x76                    // "H"
#define TM_CHAR_h         0x74                    // "h"
#define TM_CHAR_L         0x38                    // "L"
#define TM_CHAR_P         0x73                    // "P"
#define TM_CHAR_U         0x3e                    // "U"

// TM1637 conversion table
extern const uint8_t TM_DATA[];                   // number to segment code table

// TM1637 functions
void TM_init(void);                               // init TM1637
void TM_setBrightness(uint8_t bright);            // set display brightness (1-8)
void TM_printSegment(uint8_t digit, uint8_t code);// print segment code to digit
void TM_printDigit(uint8_t digit, uint8_t value); // print number to digit
void TM_printH(uint16_t value);                   // print hexadecimal value
void TM_printD(uint16_t value);                   // print positive decimal value
void TM_print(int16_t value, uint8_t dotpos);     // print value with decimal at dotpos

#ifdef __cplusplus
}
#endif
