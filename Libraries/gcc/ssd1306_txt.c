// ===================================================================================
// SSD1306/SH1106 I2C OLED Text Functions                                     * v1.2 *
// ===================================================================================
//
// Collection of the most necessary functions for controlling an SSD1306/SH1106 I2C 
// OLED for the display of simple text.
//
// References:
// -----------
// - Neven Boyanov:         https://github.com/tinusaur/ssd1306xled
// - Stephen Denne:         https://github.com/datacute/Tiny4kOLED
// - David Johnson-Davies:  http://www.technoblogy.com/show?TV4
// - TinyOLEDdemo:          https://github.com/wagiminator/attiny13-tinyoleddemo
// - TinyTerminal:          https://github.com/wagiminator/ATtiny85-TinyTerminal
// - OLED Font Editor:      http://sourpuss.net/projects/fontedit/
//
// 2022 by Stefan Wagner:   https://github.com/wagiminator

#include "ssd1306_txt.h"

// ===================================================================================
// Standard ASCII 5x8 Font (chars 32 - 127)
// ===================================================================================
const uint8_t OLED_FONT[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00, 0x07, 0x00, 0x07, 0x00,
  0x14, 0x7F, 0x14, 0x7F, 0x14, 0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x23, 0x13, 0x08, 0x64, 0x62,
  0x36, 0x49, 0x55, 0x22, 0x50, 0x00, 0x04, 0x03, 0x00, 0x00, 0x00, 0x1C, 0x22, 0x41, 0x00,
  0x00, 0x41, 0x22, 0x1C, 0x00, 0x14, 0x08, 0x3E, 0x08, 0x14, 0x08, 0x08, 0x3E, 0x08, 0x08,
  0x00, 0x80, 0x60, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x60, 0x60, 0x00, 0x00,
  0x20, 0x10, 0x08, 0x04, 0x02, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x44, 0x42, 0x7F, 0x40, 0x40,
  0x42, 0x61, 0x51, 0x49, 0x46, 0x22, 0x41, 0x49, 0x49, 0x36, 0x18, 0x14, 0x12, 0x7F, 0x10,
  0x2F, 0x49, 0x49, 0x49, 0x31, 0x3E, 0x49, 0x49, 0x49, 0x32, 0x03, 0x01, 0x71, 0x09, 0x07,
  0x36, 0x49, 0x49, 0x49, 0x36, 0x26, 0x49, 0x49, 0x49, 0x3E, 0x00, 0x36, 0x36, 0x00, 0x00,
  0x00, 0x80, 0x68, 0x00, 0x00, 0x00, 0x08, 0x14, 0x22, 0x00, 0x14, 0x14, 0x14, 0x14, 0x14,
  0x00, 0x22, 0x14, 0x08, 0x00, 0x02, 0x01, 0x51, 0x09, 0x06, 0x3E, 0x41, 0x5D, 0x55, 0x5E,
  0x7C, 0x12, 0x11, 0x12, 0x7C, 0x7F, 0x49, 0x49, 0x49, 0x36, 0x3E, 0x41, 0x41, 0x41, 0x22,
  0x7F, 0x41, 0x41, 0x22, 0x1C, 0x7F, 0x49, 0x49, 0x49, 0x41, 0x7F, 0x09, 0x09, 0x09, 0x01,
  0x3E, 0x41, 0x49, 0x49, 0x3A, 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x41, 0x41, 0x7F, 0x41, 0x41,
  0x20, 0x40, 0x41, 0x3F, 0x01, 0x7F, 0x08, 0x14, 0x22, 0x41, 0x7F, 0x40, 0x40, 0x40, 0x40,
  0x7F, 0x02, 0x0C, 0x02, 0x7F, 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x3E, 0x41, 0x41, 0x41, 0x3E,
  0x7F, 0x09, 0x09, 0x09, 0x06, 0x3E, 0x41, 0x41, 0xC1, 0xBE, 0x7F, 0x09, 0x19, 0x29, 0x46,
  0x26, 0x49, 0x49, 0x49, 0x32, 0x01, 0x01, 0x7F, 0x01, 0x01, 0x3F, 0x40, 0x40, 0x40, 0x3F,
  0x1F, 0x20, 0x40, 0x20, 0x1F, 0x3F, 0x40, 0x38, 0x40, 0x3F, 0x63, 0x14, 0x08, 0x14, 0x63,
  0x07, 0x08, 0x70, 0x08, 0x07, 0x61, 0x51, 0x49, 0x45, 0x43, 0x00, 0x7F, 0x41, 0x41, 0x00,
  0x02, 0x04, 0x08, 0x10, 0x20, 0x00, 0x41, 0x41, 0x7F, 0x00, 0x08, 0x04, 0x02, 0x04, 0x08,
  0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x06, 0x09, 0x09, 0x06, 0x20, 0x54, 0x54, 0x54, 0x78,
  0x7F, 0x44, 0x44, 0x44, 0x38, 0x38, 0x44, 0x44, 0x44, 0x28, 0x38, 0x44, 0x44, 0x44, 0x7F,
  0x38, 0x54, 0x54, 0x54, 0x18, 0x08, 0xFE, 0x09, 0x01, 0x02, 0x18, 0xA4, 0xA4, 0xA4, 0x78,
  0x7F, 0x04, 0x04, 0x04, 0x78, 0x00, 0x44, 0x7D, 0x40, 0x00, 0x00, 0x80, 0x84, 0x7D, 0x00,
  0x41, 0x7F, 0x10, 0x28, 0x44, 0x00, 0x41, 0x7F, 0x40, 0x00, 0x7C, 0x04, 0x7C, 0x04, 0x78,
  0x7C, 0x04, 0x04, 0x04, 0x78, 0x38, 0x44, 0x44, 0x44, 0x38, 0xFC, 0x24, 0x24, 0x24, 0x18,
  0x18, 0x24, 0x24, 0x24, 0xFC, 0x7C, 0x08, 0x04, 0x04, 0x08, 0x08, 0x54, 0x54, 0x54, 0x20,
  0x04, 0x3F, 0x44, 0x40, 0x20, 0x3C, 0x40, 0x40, 0x40, 0x3C, 0x1C, 0x20, 0x40, 0x20, 0x1C,
  0x3C, 0x40, 0x30, 0x40, 0x3C, 0x44, 0x28, 0x10, 0x28, 0x44, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C,
  0x44, 0x64, 0x54, 0x4C, 0x44, 0x08, 0x08, 0x36, 0x41, 0x41, 0x00, 0x00, 0xFF, 0x00, 0x00,
  0x41, 0x41, 0x36, 0x08, 0x08, 0x08, 0x04, 0x08, 0x10, 0x08, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

// ===================================================================================
// OLED Control Functions
// ===================================================================================

// Screen offsets
#if OLED_SH1106 == 1
  #define OLED_XOFF ((128 - OLED_WIDTH) / 2) + 2
#else
  #define OLED_XOFF ((128 - OLED_WIDTH) / 2)
#endif

// OLED initialisation sequence
const uint8_t OLED_INIT_CMD[] = {
  OLED_MULTIPLEX,  OLED_HEIGHT - 1,               // set multiplex ratio
  OLED_CHARGEPUMP, 0x14,                          // set DC-DC enable  
  OLED_MEMORYMODE, 0x00,                          // set horizontal addressing mode
  #if OLED_WIDTH == 128 && OLED_HEIGHT == 32
  OLED_COMPINS,    0x02,                          // set com pins
  #else
  OLED_COMPINS,    0x12,                          // set com pins
  #endif
  #if OLED_FLIP_SCREEN > 0
  OLED_XFLIP_ON, OLED_YFLIP_ON,                   // flip screen
  #endif
  OLED_DISPLAY_ON                                 // display on
};

// OLED init function
void OLED_init(void) {
  #if OLED_INIT_I2C > 0
  I2C_init();                                     // initialize I2C first
  #endif
  #if OLED_BOOT_TIME > 0
  DLY_ms(OLED_BOOT_TIME);                         // time for the OLED to boot up
  #endif
  I2C_start(OLED_ADDR << 1);                      // start transmission to OLED
  I2C_write(OLED_CMD_MODE);                       // set command mode
  I2C_writeBuffer((uint8_t*)OLED_INIT_CMD, sizeof(OLED_INIT_CMD)); // send the command bytes
}

// Switch display on/off (0: display off, 1: display on)
void OLED_display(uint8_t val) {
  I2C_start(OLED_ADDR << 1);                      // start transmission to OLED
  I2C_write(OLED_CMD_MODE);                       // set command mode
  I2C_write(val ? OLED_DISPLAY_ON : OLED_DISPLAY_OFF); // set display power
  I2C_stop();                                     // stop transmission
}

// Set display contrast (0-255)
void OLED_contrast(uint8_t val) {
  I2C_start(OLED_ADDR << 1);                      // start transmission to OLED
  I2C_write(OLED_CMD_MODE);                       // set command mode
  I2C_write(OLED_CONTRAST);                       // contrast command
  I2C_write(val);                                 // set contrast value
  I2C_stop();                                     // stop transmission
}

// Invert display (0: inverse off, 1: inverse on)
void OLED_invert(uint8_t val) {
  I2C_start(OLED_ADDR << 1);                      // start transmission to OLED
  I2C_write(OLED_CMD_MODE);                       // set command mode
  I2C_write(val ? OLED_INVERT_ON : OLED_INVERT_OFF); // set invert mode
  I2C_stop();                                     // stop transmission
}

// Flip display (0: flip off, 1: flip on)
void OLED_flip(uint8_t xflip, uint8_t yflip) {
  I2C_start(OLED_ADDR << 1);                      // start transmission to OLED
  I2C_write(OLED_CMD_MODE);                       // set command mode
  I2C_write(xflip ? OLED_XFLIP_ON : OLED_XFLIP_OFF); // set x-flip
  I2C_write(yflip ? OLED_YFLIP_ON : OLED_YFLIP_OFF); // set y-flip
  I2C_stop();                                     // stop transmission
}

// Scroll display vertically
void OLED_vscroll(uint8_t y) {
  I2C_start(OLED_ADDR << 1);                      // start transmission to OLED
  I2C_write(OLED_CMD_MODE);                       // set command mode
  I2C_write(OLED_OFFSET);                         // offset command
  I2C_write(y);                                   // set y-scroll
  I2C_stop();                                     // stop transmission
}

// ===================================================================================
// OLED Text Functions
// ===================================================================================

// OLED global variables
uint8_t OLED_x, OLED_y;

// OLED clear line
void OLED_clearLine(uint8_t y) {
  uint8_t i;
  OLED_cursor(0, y);                              // set cursor to line start
  I2C_start(OLED_ADDR << 1);                      // start transmission to OLED
  I2C_write(OLED_DAT_MODE);                       // set data mode
  for(i=OLED_WIDTH; i; i--) I2C_write(0x00);      // clear line
  I2C_stop();                                     // stop transmission
  OLED_cursor(0, y);                              // re-set cursor to line start
}

// OLED clear screen
void OLED_clear(void) {
  uint8_t y = OLED_HEIGHT / 8;
  while(y--) OLED_clearLine(y);                   // clear all lines
}

// OLED set cursor to specified position
void OLED_cursor(uint8_t x, uint8_t y) {
  if(y >= OLED_HEIGHT / 8) y = 0;                 // limit y
  uint8_t xp = OLED_XOFF + x;                     // add offset
  I2C_start(OLED_ADDR << 1);                      // start transmission to OLED
  I2C_write(OLED_CMD_MODE);                       // set command mode
  I2C_write(OLED_PAGE + y);                       // set line
  I2C_write(xp & 0xf);                            // set column
  I2C_write((xp >> 4) | 0x10);
  I2C_stop();                                     // stop transmission
  OLED_x = x;                                     // set variables
  OLED_y = y;
}

// OLED plot a single character
void OLED_plotChar(char c) {
  uint16_t ptr = c - 32;                          // character pointer
  ptr += ptr << 2;                                // -> ptr = (ch - 32) * 5;
  I2C_start(OLED_ADDR << 1);                      // start transmission to OLED
  I2C_write(OLED_DAT_MODE);                       // set data mode
  I2C_write(0x00);                                // write space between characters
  I2C_writeBuffer((uint8_t*)&OLED_FONT[ptr], 5);  // write character
  OLED_x += 6;                                    // move cursor
  if(OLED_x > OLED_WIDTH - 6) OLED_cursor(0, OLED_y + 1);
}

// OLED write a character or handle control characters
void OLED_write(char c) {
  c &= 0x7f;                                      // ignore top bit
  if(c >= 32) OLED_plotChar(c);                   // normal character
  else if(c == '\n') OLED_cursor(0, OLED_y + 1);  // new line
  else if(c == '\r') OLED_cursor(0, OLED_y);      // carriage return
}
