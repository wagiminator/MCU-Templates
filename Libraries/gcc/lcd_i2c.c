// ===================================================================================
// Basic HD44780 Character LCD Functions via PCF8574 I2C Backpack             * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "lcd_i2c.h"

// PCF8574 pin assignment
#define LCD_RS                0x01    // register select (0: command, 1: data)
#define LCD_EN                0x04    // enable
#define LCD_BL                0x08    // backlight

// Variables
static uint8_t LCD_bl = LCD_BL;       // backlight state
static uint8_t LCD_x, LCD_y;          // cursor position

// Write 4 bits with enable pulse (internal)
static void LCD_write4(uint8_t data) {
  data |= LCD_bl;
  I2C_start((uint8_t)(LCD_ADDR << 1));
  I2C_write(data | LCD_EN);                       // E high
  I2C_write(data);                                // E low -> data is latched
  I2C_stop();
}

// Write byte as two nibbles (internal)
static void LCD_send(uint8_t data, uint8_t rs) {
  LCD_write4((data & 0xF0) | rs);
  LCD_write4((data << 4) | rs);
  DLY_us(40);                                     // execution time
}

// Send command (internal)
static void LCD_command(uint8_t cmd) {
  LCD_send(cmd, 0);
}

// Init LCD
void LCD_init(void) {
  #if LCD_INIT_I2C > 0
  I2C_init();
  #endif
  DLY_ms(50);                                     // power-on time
  LCD_write4(0x30); DLY_ms(5);                    // reset sequence (8-bit mode)
  LCD_write4(0x30); DLY_us(150);
  LCD_write4(0x30); DLY_us(150);
  LCD_write4(0x20); DLY_us(150);                  // switch to 4-bit mode
  LCD_command(LCD_ROWS > 1 ? 0x28 : 0x20);        // 4-bit, 1/2 lines, 5x8 font
  LCD_command(0x0C);                              // display on, cursor off
  LCD_command(0x06);                              // entry mode: increment, no shift
  LCD_clear();
}

// Clear display, cursor to home position
void LCD_clear(void) {
  LCD_command(0x01);
  DLY_ms(2);                                      // execution time
  LCD_x = 0; LCD_y = 0;
}

// Set cursor to column x, row y
void LCD_cursor(uint8_t x, uint8_t y) {
  static const uint8_t ROW_OFFSET[4] = {0x00, 0x40, LCD_COLS, 0x40 + LCD_COLS};
  if(y >= LCD_ROWS) y = 0;
  LCD_x = x; LCD_y = y;
  LCD_command(0x80 | (ROW_OFFSET[y & 3] + x));
}

// Write character at cursor position
void LCD_write(char c) {
  if(c == '\n') {                                 // new line
    LCD_cursor(0, LCD_y + 1);
    return;
  }
  if(c == '\r') {                                 // carriage return
    LCD_cursor(0, LCD_y);
    return;
  }
  if(LCD_x >= LCD_COLS) LCD_cursor(0, LCD_y + 1); // automatic line wrap
  LCD_send(c, LCD_RS);
  LCD_x++;
}

// Print string at cursor position
void LCD_print(const char* str) {
  while(*str) LCD_write(*str++);
}

// Switch backlight
void LCD_backlight(uint8_t on) {
  LCD_bl = on ? LCD_BL : 0;
  I2C_start((uint8_t)(LCD_ADDR << 1));
  I2C_write(LCD_bl);
  I2C_stop();
}

// Switch display
void LCD_display(uint8_t on) {
  LCD_command(on ? 0x0C : 0x08);
}

// Define custom character n (0-7) with 8-byte bitmap (5 bits per row)
void LCD_createChar(uint8_t n, const uint8_t* bmp) {
  uint8_t i;
  LCD_command(0x40 | ((n & 7) << 3));             // set CGRAM address
  for(i = 8; i; i--) LCD_send(*bmp++, LCD_RS);
  LCD_cursor(LCD_x, LCD_y);                       // back to DDRAM
}
