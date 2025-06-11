// ===================================================================================
// SSD1306/SH1106 I2C OLED Graphics Functions                                 * v1.6 *
// ===================================================================================
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#include "ssd1306_gfx.h"

// ===================================================================================
// Screen Buffer
// ===================================================================================
uint8_t __attribute__ ((aligned(4))) OLED_buffer[OLED_WIDTH * OLED_HEIGHT / 8];

#if OLED_DOUBLEBUF == 0
  #define OLED_drawbuffer OLED_buffer
  #define OLED_sendbuffer OLED_buffer
#else
  uint8_t __attribute__ ((aligned(4))) OLED_buffer2[OLED_WIDTH * OLED_HEIGHT / 8];
  uint8_t* OLED_drawbuffer = OLED_buffer;
  uint8_t* OLED_sendbuffer = OLED_buffer2;
#endif

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
// 13x32 7-Segment Font (0 - 9)
// ===================================================================================
#if OLED_SEG_FONT == 1
const uint8_t OLED_FONT_SEG[] = {
  0xFC, 0xF9, 0xF3, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0xF3, 0xF9, 0xFC, // 0
  0x7F, 0x3F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x3F, 0x7F, 
  0xFF, 0xFE, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFE, 0xFF, 
  0x1F, 0x4F, 0x67, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x67, 0x4F, 0x1F, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF8, 0xFC, // 1
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x3F, 0x7F, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFE, 0xFF, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x1F, 
  0x00, 0x01, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0xF3, 0xF9, 0xFC, // 2
  0x00, 0x00, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x9F, 0x3F, 0x7F, 
  0xFF, 0xFE, 0xFC, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 
  0x1F, 0x4F, 0x67, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x60, 0x40, 0x00, 
  0x00, 0x01, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0xF3, 0xF9, 0xFC, // 3
  0x00, 0x00, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x9F, 0x3F, 0x7F, 
  0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFC, 0xFE, 0xFF, 
  0x00, 0x40, 0x60, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x67, 0x4F, 0x1F, 
  0xFC, 0xF8, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF8, 0xFC, // 4
  0x7F, 0x3F, 0x9F, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x9F, 0x3F, 0x7F, 
  0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFC, 0xFE, 0xFF, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x1F, 
  0xFC, 0xF9, 0xF3, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x01, 0x00, // 5
  0x7F, 0x3F, 0x9F, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFC, 0xFE, 0xFF, 
  0x00, 0x40, 0x60, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x67, 0x4F, 0x1F, 
  0xFC, 0xF9, 0xF3, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x01, 0x00, // 6
  0x7F, 0x3F, 0x9F, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 
  0xFF, 0xFE, 0xFC, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFC, 0xFE, 0xFF, 
  0x1F, 0x4F, 0x67, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x67, 0x4F, 0x1F, 
  0xFC, 0xF9, 0xF3, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0xF3, 0xF9, 0xFC, // 7
  0x7F, 0x3F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x3F, 0x7F, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFE, 0xFF, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x1F, 
  0xFC, 0xF9, 0xF3, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0xF3, 0xF9, 0xFC, // 8
  0x7F, 0x3F, 0x9F, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x9F, 0x3F, 0x7F, 
  0xFF, 0xFE, 0xFC, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFC, 0xFE, 0xFF, 
  0x1F, 0x4F, 0x67, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x67, 0x4F, 0x1F, 
  0xFC, 0xF9, 0xF3, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0xF3, 0xF9, 0xFC, // 9
  0x7F, 0x3F, 0x9F, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x9F, 0x3F, 0x7F, 
  0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFC, 0xFE, 0xFF, 
  0x00, 0x40, 0x60, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x67, 0x4F, 0x1F
};
#endif

// ===================================================================================
// 5x16 7-Segment Font (0 - 9)
// ===================================================================================
#if OLED_SEG_FONT == 2
const uint8_t OLED_FONT_SEG[] = {
  0x7C, 0x02, 0x02, 0x02, 0x7C, 0x1F, 0x20, 0x20, 0x20, 0x1F, // 0
  0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x1F, // 1
  0x00, 0x82, 0x82, 0x82, 0x7C, 0x1F, 0x20, 0x20, 0x20, 0x00, // 2
  0x00, 0x82, 0x82, 0x82, 0x7C, 0x00, 0x20, 0x20, 0x20, 0x1F, // 3
  0x7C, 0x80, 0x80, 0x80, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x1F, // 4
  0x7C, 0x82, 0x82, 0x82, 0x00, 0x00, 0x20, 0x20, 0x20, 0x1F, // 5
  0x7C, 0x82, 0x82, 0x82, 0x00, 0x1F, 0x20, 0x20, 0x20, 0x1F, // 6
  0x7C, 0x02, 0x02, 0x02, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x1F, // 7
  0x7C, 0x82, 0x82, 0x82, 0x7C, 0x1F, 0x20, 0x20, 0x20, 0x1F, // 8
  0x7C, 0x82, 0x82, 0x82, 0x7C, 0x00, 0x20, 0x20, 0x20, 0x1F  // 9
};
#endif

// ===================================================================================
// OLED Control Functions
// ===================================================================================

// Screen offsets
#if OLED_SH1106 == 1
  #define OLED_XOFF ((128 - OLED_WIDTH) / 2) + 2
#else
  #define OLED_XOFF ((128 - OLED_WIDTH) / 2)
#endif
#define OLED_YOFF 0

// OLED initialisation sequence
const uint8_t OLED_INIT_CMD[] = {
  OLED_MULTIPLEX,  OLED_HEIGHT - 1,               // set multiplex ratio
  OLED_CHARGEPUMP, 0x14,                          // set DC-DC enable  
  OLED_MEMORYMODE, 0x00,                          // set horizontal addressing mode
  #if OLED_SH1106 == 0 && OLED_WIDTH != 64
  OLED_COLUMNS, OLED_XOFF, OLED_XOFF + OLED_WIDTH  - 1, // set start and end column
  OLED_PAGES,   OLED_YOFF, OLED_YOFF + OLED_HEIGHT - 1, // set start and end page
  #endif
  #if OLED_WIDTH == 128 && OLED_HEIGHT == 32
  OLED_COMPINS,    0x02,                          // set com pins
  #else
  OLED_COMPINS,    0x12,                          // set com pins
  #endif
  #if OLED_XFLIP > 0
  OLED_XFLIP_ON,                                  // flip screen in X-direction
  #endif
  #if OLED_YFLIP > 0
  OLED_YFLIP_ON,                                  // flip screen in Y-direction
  #endif
  #if OLED_INVERT > 0
  OLED_INVERT_ON,                                 // invert screen
  #endif
  OLED_DISPLAY_ON                                 // display on
};

// Init OLED
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
  I2C_stop();                                     // stop transmission
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

// Set home postition
void OLED_home(uint8_t x, uint8_t y) {
  I2C_start(OLED_ADDR << 1);                      // start transmission to OLED
  I2C_write(OLED_CMD_MODE);                       // set command mode
  I2C_write(OLED_PAGE        | (y >> 3));         // set line
  I2C_write(OLED_COLUMN_LOW  | (x & 0xf));        // set column
  I2C_write(OLED_COLUMN_HIGH | (x >> 4));
  I2C_stop();                                     // stop transmission
}

// Refresh screen buffer (send buffer via I2C)
void OLED_refresh(void) {
  #if OLED_DOUBLEBUF > 0
  uint8_t* temp = OLED_drawbuffer;                // switch buffers
  OLED_drawbuffer = OLED_sendbuffer;
  OLED_sendbuffer = temp;
  #endif

  #if OLED_SH1106 == 1 || OLED_WIDTH == 64
  uint8_t* buffer = OLED_sendbuffer;
  for(uint8_t y=0; y<OLED_HEIGHT; y+=8) {         // each page must be transmitted individually
    OLED_home(OLED_XOFF, OLED_YOFF + y);          // set page
    I2C_start(OLED_ADDR << 1);                    // start transmission to OLED
    I2C_write(OLED_DAT_MODE);                     // set command mode
    I2C_writeBuffer(buffer, OLED_WIDTH);          // transmit page
    buffer += OLED_WIDTH;                         // increase buffer pointer
  }
  #else
  OLED_home(OLED_XOFF, OLED_YOFF);                // set start address
  I2C_start(OLED_ADDR << 1);                      // start transmission to OLED
  I2C_write(OLED_DAT_MODE);                       // set command mode
  I2C_writeBuffer(OLED_sendbuffer, sizeof(OLED_buffer)); // send screen buffer using DMA
  #endif
}

// ===================================================================================
// OLED Graphics Functions
// ===================================================================================

// Clear OLED screen buffer
void OLED_clear(void) {
  uint32_t* ptr = (uint32_t*)OLED_drawbuffer;
  uint32_t  cnt = sizeof(OLED_buffer) >> 2;
  while(cnt--) *ptr++ = (uint32_t)0;
}

// Copy OLED screen buffer
void OLED_copy(void) {
  uint32_t* sptr = (uint32_t*)OLED_sendbuffer;
  uint32_t* dptr = (uint32_t*)OLED_drawbuffer;
  uint32_t  cnt  = sizeof(OLED_buffer) >> 2;
  while(cnt--) *dptr++ = *sptr++;
}

// Get pixel color at (x,y) (0: pixel cleared, 1: pixel set)
uint8_t OLED_getPixel(int16_t x, int16_t y) {
  #if OLED_PORTRAIT == 0
  if((x < 0) || (x >= OLED_WIDTH) || (y < 0) || (y >= OLED_HEIGHT)) return 0;
  return((OLED_drawbuffer[((uint16_t)y >> 3) * OLED_WIDTH + x] >> (y & 7)) & 1);
  #else
  x = (int16_t)(OLED_HEIGHT - 1) - x;
  if((x < 0) || (x >= OLED_HEIGHT) || (y < 0) || (y >= OLED_WIDTH)) return 0;
  return((OLED_drawbuffer[((uint16_t)x >> 3) * OLED_WIDTH + y] >> (x & 7)) & 1);
  #endif
}

// Set pixel at position (x,y) with color (0: clear pixel, 1: set pixel, 2: invert pixel)
void OLED_setPixel(int16_t x, int16_t y, uint8_t color) {
  #if OLED_PORTRAIT == 0
  if((x < 0) || (x >= OLED_WIDTH) || (y < 0) || (y >= OLED_HEIGHT)) return;
  switch(color) {
    case 0: OLED_drawbuffer[((uint16_t)y >> 3) * OLED_WIDTH + x] &= ~((uint8_t)1 << (y & 7));
            break;
    case 1: OLED_drawbuffer[((uint16_t)y >> 3) * OLED_WIDTH + x] |=  ((uint8_t)1 << (y & 7));
            break;
    case 2: OLED_drawbuffer[((uint16_t)y >> 3) * OLED_WIDTH + x] ^=  ((uint8_t)1 << (y & 7));
            break;
  }
  #else
  if((x < 0) || (x >= OLED_HEIGHT) || (y < 0) || (y >= OLED_WIDTH)) return;
  x = (int16_t)(OLED_HEIGHT - 1) - x;
  switch(color) {
    case 0: OLED_drawbuffer[((uint16_t)x >> 3) * OLED_WIDTH + y] &= ~((uint8_t)1 << (x & 7));
            break;
    case 1: OLED_drawbuffer[((uint16_t)x >> 3) * OLED_WIDTH + y] |=  ((uint8_t)1 << (x & 7));
            break;
    case 2: OLED_drawbuffer[((uint16_t)x >> 3) * OLED_WIDTH + y] ^=  ((uint8_t)1 << (x & 7));
            break;
  }
  #endif
}

// Draw vertical line starting from (x,y), height (h), color (0: cleared, 1: set)
void OLED_drawVLine(int16_t x, int16_t y, int16_t h, uint8_t color) {
  for(int16_t i=y; i<y+h; i++) OLED_setPixel(x, i, color);
}

// Draw horizontal line starting from (x,y), width (w), color (0: cleared, 1: set)
void OLED_drawHLine(int16_t x, int16_t y, int16_t w, uint8_t color) {
  for(int16_t i=x; i<x+w; i++) OLED_setPixel(i, y, color);
}

// Draw line from position (x0,y0) to (x1,y1) with color (0: cleared, 1: set)
// (Bresenham's line algorithm)
void OLED_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color) {
  int16_t dx = OLED_abs(x1 - x0);
  int16_t sx = x0 < x1 ? 1 : -1;
  int16_t dy = -OLED_abs(y1 - y0);
  int16_t sy = y0 < y1 ? 1 : -1;
  int16_t error = dx + dy;
    
  while(1) {
    OLED_setPixel(x0, y0, color);
    if(x0 == x1 && y0 == y1) break;
    int16_t e2 = error * 2;
    if(e2 >= dy) {
      if(x0 == x1) break;
      error += dy;
      x0 += sx;
    }
    if(e2 <= dx) {
      if(y0 == y1) break;
      error += dx;
      y0 += sy;
    }
  }
}

// Draw rectangle starting from (x,y), width (w), height (h), color (0: cleared, 1: set)
void OLED_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color) {
  OLED_drawHLine(x    , y,     w, color);
  OLED_drawHLine(x    , y+h-1, w, color);
  OLED_drawVLine(x    , y,     h, color);
  OLED_drawVLine(x+w-1, y,     h, color);
}

// Draw filled rectangle starting from (x,y), width (w), height (h), color
void OLED_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color) {
  for(int16_t i=x; i<x+w; i++) OLED_drawVLine(i, y, h, color);
}

// Draw circle, center at position (x0,y0), radius (r), color (0: cleared, 1: set)
// (midpoint circle algorithm)
void OLED_drawCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -(r << 1);
  int16_t x = 0;
  int16_t y = r;

  while(x <= y) {
    OLED_setPixel(x0 + x, y0 + y, color);
    OLED_setPixel(x0 - x, y0 + y, color);
    OLED_setPixel(x0 + x, y0 - y, color);
    OLED_setPixel(x0 - x, y0 - y, color);
    OLED_setPixel(x0 + y, y0 + x, color);
    OLED_setPixel(x0 - y, y0 + x, color);
    OLED_setPixel(x0 + y, y0 - x, color);
    OLED_setPixel(x0 - y, y0 - x, color);

    if(f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    ddF_x += 2;
    f += ddF_x;
    x++;
  }
}

// Draw filled circle, center at position (x0,y0), radius (r), color
// (midpoint circle algorithm)
void OLED_fillCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -(r << 1);
  int16_t x = 0;
  int16_t y = r;

  while(x <= y) {
    OLED_drawVLine(x0 - x, y0 - y, (y << 1) + 1, color);
    OLED_drawVLine(x0 + x, y0 - y, (y << 1) + 1, color);
    OLED_drawVLine(x0 - y, y0 - x, (x << 1) + 1, color);
    OLED_drawVLine(x0 + y, y0 - x, (x << 1) + 1, color);

    if(f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  }
}

// ===================================================================================
// OLED Bitmap Functions
// ===================================================================================

// Draw a complete screen
void OLED_drawScreen(const uint8_t* bmp) {
  uint32_t* ptr1 = (uint32_t*)bmp;
  uint32_t* ptr2 = (uint32_t*)OLED_buffer;
  uint32_t  cnt = sizeof(OLED_buffer) >> 2;
  while(cnt--) *ptr2++ = *ptr1++;
}

// Draw bitmap at position (x0,y0), width (w), hight (h), pointer to bitmap (*bmp)
void OLED_drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint8_t* bmp) {
  for(int16_t y=y0; y<y0+h; y+=8) {
    for(int16_t x=x0; x<x0+w; x++) {
      uint8_t line = *bmp++;
      for(int16_t i=y; i<y+8; i++, line>>=1) {
        OLED_setPixel(x, i, line & 1);
      }
    }
  }
}

// Draw sprite (bitmap with transparent background)
void OLED_drawSprite(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint8_t* bmp) {
  for(int16_t y=y0; y<y0+h; y+=8) {
    for(int16_t x=x0; x<x0+w; x++) {
      uint8_t line = *bmp++;
      for(int16_t i=y; i<y+8; i++, line>>=1) {
        if(line & 1) OLED_setPixel(x, i, 1);
      }
    }
  }
}

// ===================================================================================
// OLED Text Functions
// ===================================================================================

// Variables
int16_t OLED_cx, OLED_cy;                           // cursor position
uint8_t OLED_ci, OLED_cs = 1;                       // inversion and size

// Set cursor position
void OLED_cursor(int16_t x, int16_t y) {
  OLED_cx = x; OLED_cy = y;
}

// Set text size
void OLED_textsize(uint8_t size) {
  OLED_cs = size;
}

// OLED set text invert
void OLED_textinvert(uint8_t yes) {
  OLED_ci = yes;
}

// Converts bit pattern abcdefgh into aabbccddeeffgghh
uint16_t OLED_stretch(uint16_t x) {
  x = (x & 0xF0)<<4 | (x & 0x0F);
  x = (x<<2 | x) & 0x3333;
  x = (x<<1 | x) & 0x5555;
  return x | x<<1;
}

// Write a character
void OLED_write(char c) {
  c &= 0x7f;
  if(c >= 32) {
    uint16_t ptr = c - 32;
    ptr += ptr << 2;

    // Standard character, if necessary enlarged
    if(OLED_cs <= 8) {
      for(uint8_t i=6; i; i--) {
        uint8_t line, col;
        int16_t y1 = OLED_cy;
        line = OLED_FONT[ptr++];
        if(i == 1) line = 0;
        if(OLED_ci) line = ~line;
        for(uint8_t j=0; j<8; j++, line>>=1) {
          col = line & 1;
          if(OLED_cs == 1) OLED_setPixel(OLED_cx, y1++, col);
          else {
            OLED_fillRect(OLED_cx, y1, OLED_cs, OLED_cs, col);
            y1 += OLED_cs;
          }
        }
        OLED_cx += OLED_cs;
      }
      return;
    }

    // Double-sized, smoothed character (10x16, David Johnson-Davies' Smooth Big Text algorithm)
    if(OLED_cs == OLED_SMOOTH) {
      uint16_t col0L, col0R, col1L, col1R;
      uint8_t col0 = OLED_FONT[ptr++];
      col0L = OLED_stretch(col0);
      col0R = col0L;
      for(uint8_t col=5; col; col--) {
        uint8_t col1 = OLED_FONT[ptr++];
        if(col == 1) col1 = 0;
        col1L = OLED_stretch(col1);
        col1R = col1L;    
        for(int8_t i=6; i>=0; i--) {
          for(int8_t j=1; j<3; j++) {
            if(((col0>>i & 0b11) == (3 - j)) && ((col1>>i & 0b11) == j)) {
              col0R = col0R | 1<<((i << 1) + j);
              col1L = col1L | 1<<((i << 1) + 3 - j);
            }
          }
        }
        int16_t y1 = OLED_cy;
        if(OLED_ci) {
          col0L = ~col0L;
          col0R = ~col0R;
        }
        for(int8_t i=16; i; i--, col0L>>=1, col0R>>=1) {
          OLED_setPixel(OLED_cx,   y1,   col0L & 1);
          OLED_setPixel(OLED_cx+1, y1++, col0R & 1);
        }
        col0 = col1; col0L = col1L; col0R = col1R; OLED_cx += 2;
      }
      OLED_fillRect(OLED_cx, OLED_cy, 2, 16, OLED_ci);
      OLED_cx += 2;
      return;
    }

    // V-stretched character (5x16)
    for(uint8_t col=6; col; col--) {
      uint8_t col0 = OLED_FONT[ptr++];
      if(col == 1) col0 = 0;
      if(OLED_ci) col0 = ~col0;
      int16_t y1 = OLED_cy;
      for(uint8_t i=8; i; i--, col0>>=1) {
        OLED_setPixel(OLED_cx, y1++, col0 & 1);
        OLED_setPixel(OLED_cx, y1++, col0 & 1);
      }
      OLED_cx++;
    }
    return;
  }

  // New line
  if(c == '\n') {
    OLED_cx = 0;
    if(OLED_cs <= 8) OLED_cy += OLED_cs << 3;
    else OLED_cy += 16;
    return;
  }

  // Carriage return
  if(c == '\r') OLED_cx = 0;
}

// Print string (str)
void OLED_print(char* str) {
  while(*str) OLED_write(*str++);
}

// ===================================================================================
// OLED 7-Segment Functions
// ===================================================================================

// Print value as 7-segment digits (BCD conversion by substraction method)
void OLED_printSegment(uint16_t value, uint8_t digits, uint8_t lead, uint8_t decimal) {
  static const uint16_t DIVIDER[] = {1, 10, 100, 1000, 10000};
  uint8_t leadflag = 0;                           // flag for leading spaces
  while(digits--) {                               // for all digits digits
    uint8_t digitval = 0;                         // start with digit value 0
    uint16_t divider = DIVIDER[digits];           // read current divider
    while(value >= divider) {                     // if current divider fits into the value
      leadflag = 1;                               // end of leading spaces
      digitval++;                                 // increase digit value
      value -= divider;                           // decrease value by divider
    }
    if(digits == decimal) leadflag++;             // end leading characters before decimal
    if(leadflag || !lead) {
      #if OLED_SEG_FONT == 0
      OLED_write(digitval + '0');
      #elif OLED_SEG_FONT == 1
      uint16_t ptr = (uint16_t)digitval;          // character pointer
      ptr = (ptr << 5) + (ptr << 4) + (ptr << 2); // -> ptr = c * 13 * 4;
      OLED_drawBitmap(OLED_cx, OLED_cy, 13, 32, (uint8_t*)&OLED_FONT_SEG[ptr]);
      #elif OLED_SEG_FONT == 2
      uint16_t ptr = (uint16_t)digitval;          // character pointer
      ptr = (ptr << 3) + (ptr << 1);              // -> ptr = c * 5 * 2;
      OLED_drawBitmap(OLED_cx, OLED_cy, 5, 16, (uint8_t*)&OLED_FONT_SEG[ptr]);
      #endif
    }
    #if OLED_SEG_FONT == 0
    else OLED_write(' ');
    #elif OLED_SEG_FONT == 1
    OLED_cx += OLED_SEG_SPACE + 13;
    #elif OLED_SEG_FONT == 2
    OLED_cx += OLED_SEG_SPACE + 5;
    #endif
    if(decimal && (digits == decimal)) {
      #if OLED_SEG_FONT == 0
      OLED_write('.');
      #elif OLED_SEG_FONT == 1
      OLED_fillRect(OLED_cx, OLED_cy + 28, 3, 3, 1);  // print decimal point
      OLED_cx += OLED_SEG_SPACE + 3;
      #elif OLED_SEG_FONT == 2
      OLED_fillRect(OLED_cx, OLED_cy + 12, 2, 2, 1);  // print decimal point
      OLED_cx += OLED_SEG_SPACE + 2;
      #endif
    }
  }
}
