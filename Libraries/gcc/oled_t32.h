// ===================================================================================
// SSD1306 128x32 Pixels OLED Functions                                       * v1.0 *
// ===================================================================================
//
// Collection of the most necessary functions for controlling an SSD1306 128x32 pixels
// I2C OLED for the display of text.
//
// Functions available:
// --------------------
// OLED_init()              init OLED display
// OLED_contrast(v)         set contrast (0-255)
// OLED_clear()             clear screen
// OLED_clearLine()         clear rest of the current line
// OLED_cursor(x, y)        set the cursor to specified position
// OLED_print(str)          print a string
// OLED_println(str)        print a string with new line
//
// OLED_printVal(val, digits, decimal, char)    print value with number of digits,
//                                              decimal on specified position and
//                                              leading character
//
// References:
// -----------
// - Neven Boyanov:         https://github.com/tinusaur/ssd1306xled
// - Stephen Denne:         https://github.com/datacute/Tiny4kOLED
// - TinyOLEDdemo:          https://github.com/wagiminator/attiny13-tinyoleddemo
// - OLED Font Editor:      http://sourpuss.net/projects/fontedit/
//
// 2022 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c_soft.h"

// OLED definitions
#define OLED_ADDR         0x78    // OLED write address (0x3C << 1)
#define OLED_CMD_MODE     0x00    // set command mode
#define OLED_DAT_MODE     0x40    // set data mode

// OLED commands
#define OLED_COLUMN_LOW   0x00    // set lower 4 bits of start column (0x00 - 0x0F)
#define OLED_COLUMN_HIGH  0x10    // set higher 4 bits of start column (0x10 - 0x1F)
#define OLED_MEMORYMODE   0x20    // set memory addressing mode (following byte)
#define OLED_COLUMNS      0x21    // set start and end column (following 2 bytes)
#define OLED_PAGES        0x22    // set start and end page (following 2 bytes)
#define OLED_STARTLINE    0x40    // set display start line (0x40-0x7F = 0-63)
#define OLED_CONTRAST     0x81    // set display contrast (following byte, 0-255)
#define OLED_CHARGEPUMP   0x8D    // (following byte - 0x14:enable, 0x10: disable)
#define OLED_XFLIP_OFF    0xA0    // don't flip display horizontally
#define OLED_XFLIP        0xA1    // flip display horizontally
#define OLED_INVERT_OFF   0xA6    // set non-inverted display
#define OLED_INVERT       0xA7    // set inverse display
#define OLED_MULTIPLEX    0xA8    // set multiplex ratio (following byte)
#define OLED_DISPLAY_OFF  0xAE    // set display off (sleep mode)
#define OLED_DISPLAY_ON   0xAF    // set display on
#define OLED_PAGE         0xB0    // set start page (0xB0-0xB7 = 0-7)
#define OLED_YFLIP_OFF    0xC0    // don't flip display vertically
#define OLED_YFLIP        0xC8    // flip display vertically
#define OLED_OFFSET       0xD3    // set display offset (y-scroll: following byte)
#define OLED_CLOCK        0xD5    // set frequency (bits 7-4) and divider (bits 3-0)
#define OLED_PRECHARGE    0xD9    // set pre-charge period (following byte)
#define OLED_COMPINS      0xDA    // set COM pin config (following byte)

// OLED functions
void OLED_init(void);                             // OLED init function
void OLED_clear(void);                            // OLED clear screen
void OLED_clearLine(void);                        // OLED clear rest of the current line
void OLED_cursor(uint8_t xpos, uint8_t ypos);     // OLED set the cursor
void OLED_print(const char* str);                 // OLED print a string
void OLED_println(const char* str);               // OLED print a string with new line
void OLED_contrast(uint8_t val);                  // OLED set contrast

// OLED print value with number of digits, decimal on specified position and leading char
void OLED_printVal(uint16_t value, uint8_t digits, uint8_t decimal, char ch);

#ifdef __cplusplus
};
#endif
