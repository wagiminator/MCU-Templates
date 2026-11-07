// ===================================================================================
// Basic HD44780 Character LCD Functions via PCF8574 I2C Backpack             * v1.0 *
// ===================================================================================
//
// For the common 16x2, 20x4 etc. character LCD modules with PCF8574(A) I2C adapter.
//
// Functions available:
// --------------------
// LCD_init()               init LCD (and I2C if LCD_INIT_I2C is set)
// LCD_clear()              clear display, cursor to home position
// LCD_cursor(x,y)          set cursor to column x, row y (starting at 0)
// LCD_write(c)             write character at cursor position ('\n': next line)
// LCD_print(str)           print string at cursor position
// LCD_backlight(v)         switch backlight (0: off, 1: on)
// LCD_display(v)           switch display (0: off, 1: on)
// LCD_createChar(n,*p)     define custom character n (0-7) with 8-byte bitmap
//
// If print functions are activated (see below, print.h must be included):
// -----------------------------------------------------------------------
// LCD_printf(f, ...)       printf (supports %s, %c, %d, %u, %x, %b, %02d, %%)
// LCD_printD(n)            print decimal value
// LCD_printS(s)            print string
//
// Notes:
// ------
// - Expected PCF8574 wiring (most adapters): P0=RS, P1=RW, P2=E, P3=backlight,
//   P4..P7=D4..D7.
// - The LCD is written only (RW is low), so it works with 3.3V MCUs even if the LCD
//   is supplied with 5V (keep I2C pull-ups at 3.3V).
// - Custom characters 1..7 are printed with character codes 1..7, custom character 0
//   with code 8, e.g. LCD_print("\x08").
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"                      // choose your I2C library
#include "system.h"
#ifdef __SDCC
#include "delay.h"
#endif

// LCD parameters
#define LCD_INIT_I2C          1       // 1: init I2C with LCD_init()
#define LCD_ADDR              0x27    // I2C address (PCF8574: 0x27, PCF8574A: 0x3F)
#define LCD_COLS              16      // number of columns
#define LCD_ROWS              2       // number of rows
#define LCD_PRINT             0       // 1: include print functions (needs print.h)

// LCD functions
void LCD_init(void);                  // init LCD
void LCD_clear(void);                 // clear display
void LCD_cursor(uint8_t x, uint8_t y);// set cursor position
void LCD_write(char c);               // write character
void LCD_print(const char* str);      // print string
void LCD_backlight(uint8_t on);       // switch backlight
void LCD_display(uint8_t on);         // switch display
void LCD_createChar(uint8_t n, const uint8_t* bmp); // define custom character

// Additional print functions (if activated, see above)
#if LCD_PRINT == 1
#include "print.h"
#define LCD_printD(n)         printD(LCD_write, n)    // print decimal as string
#define LCD_printS(s)         printS(LCD_write, s)    // print string
#define LCD_printf(f, ...)    printF(LCD_write, f, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif
