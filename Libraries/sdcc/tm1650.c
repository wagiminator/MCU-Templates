// ===================================================================================
// Basic TM1650 4-Digit 8-Segment LED Display Functions                       * v1.1 *
// ===================================================================================
// 2023 by Stefan Wagner: https://github.com/wagiminator

#include "tm1650.h"

// Number to segment code conversion table
const uint8_t TM_DATA[] = {
  0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
  0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71
};

// Write value to specified TM1650 address
void TM_write(uint8_t addr, uint8_t value) {
  I2C_start(addr);
  I2C_write(value);
  I2C_stop();
}

// Set display brightness
void TM_setBrightness(uint8_t bright) {
  TM_write(TM_ADDR_MODE, TM_BRIGHTNESS(bright) | TM_DISPLAY_ON);
}

// Init TM1650 4-digit 8-segment display
void TM_init(void) {
  #if TM_INIT_I2C > 0
  I2C_init();
  #endif
  TM_write(TM_ADDR_MODE, TM_BRIGHTNESS(TM_BRIGHT) | TM_DISPLAY_ON);
}

// Print segment code to the specified digit (0-3, counting from right)
void TM_printSegment(uint8_t digit, uint8_t code) {
  TM_write(TM_ADDR_DIG4 - (digit << 1), code);
}

// Print number to the specified digit (0-3, counting from right)
void TM_printDigit(uint8_t digit, uint8_t value) {
  TM_write(TM_ADDR_DIG4 - (digit << 1), TM_DATA[value & 0x0f]);
}

// Print hexadecimal value
void TM_printH(uint16_t value) {
  uint8_t i;
  for(i=0; i<4; i++) {
    TM_printDigit(i, value & 0xf);
    value >>= 4;
  }
}

// Print positive decimal value
void TM_printD(uint16_t value) {
  TM_print((value > 9999) ? 9999 : (int16_t)value, 0xff);
}

// Print value with decimal at dotpos
void TM_print(int16_t value, uint8_t dotpos) {
  uint8_t  digit = 0;                             // start with rightmost digit
  uint8_t  neg   = (value < 0);
  uint16_t v     = neg ? -(uint16_t)value : (uint16_t)value;
  do {                                            // print digits from right to left
    uint8_t code = TM_DATA[v % 10];
    v /= 10;
    if(digit == dotpos && dotpos) code |= 0x80;   // add decimal point
    TM_printSegment(digit++, code);
  } while((v || (dotpos < 4 && digit <= dotpos)) && digit < 4);
  if(neg && digit < 4) TM_printSegment(digit++, TM_CHAR_NEG);
  while(digit < 4) TM_printSegment(digit++, 0);   // leading spaces
}
