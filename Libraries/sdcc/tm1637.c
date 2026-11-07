// ===================================================================================
// Basic TM1637 4-Digit 7-Segment LED Display Functions                       * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "tm1637.h"

// Number to segment code conversion table
const uint8_t TM_DATA[] = {
  0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
  0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71
};

// Bus control (open-drain emulation, lines are released for HIGH)
#define TM_CLK_low()      do {PIN_low(TM_PIN_CLK); PIN_output(TM_PIN_CLK);} while(0)
#define TM_CLK_high()     PIN_input(TM_PIN_CLK)
#define TM_DIO_low()      do {PIN_low(TM_PIN_DIO); PIN_output(TM_PIN_DIO);} while(0)
#define TM_DIO_high()     PIN_input(TM_PIN_DIO)
#define TM_delay()        DLY_us(5)

// Start condition (internal)
static void TM_start(void) {
  TM_DIO_low(); TM_delay();
  TM_CLK_low(); TM_delay();
}

// Stop condition (internal)
static void TM_stop(void) {
  TM_DIO_low();  TM_delay();
  TM_CLK_high(); TM_delay();
  TM_DIO_high(); TM_delay();
}

// Write one byte, LSB first, and clock in ACK (internal)
static void TM_write(uint8_t data) {
  uint8_t i;
  for(i = 8; i; i--, data >>= 1) {
    if(data & 1) TM_DIO_high();
    else         TM_DIO_low();
    TM_delay();
    TM_CLK_high(); TM_delay();
    TM_CLK_low();
  }
  TM_DIO_high(); TM_delay();                      // release DIO for ACK
  TM_CLK_high(); TM_delay();
  TM_CLK_low();  TM_delay();
}

// Send a single command (internal)
static void TM_command(uint8_t cmd) {
  TM_start();
  TM_write(cmd);
  TM_stop();
}

// Set display brightness (1-8), 0: display off
void TM_setBrightness(uint8_t bright) {
  if(bright > 8) bright = 8;
  TM_command(bright ? (0x87 + bright) : 0x80);
}

// Init TM1637 4-digit display
void TM_init(void) {
  TM_CLK_high();
  TM_DIO_high();
  TM_command(0x44);                               // fixed address mode
  TM_setBrightness(TM_BRIGHT);
}

// Print segment code to the specified digit (0-3, counting from right)
void TM_printSegment(uint8_t digit, uint8_t code) {
  TM_start();
  TM_write(0xC0 | (3 - (digit & 3)));             // address (0 = leftmost digit)
  TM_write(code);
  TM_stop();
}

// Print number to the specified digit (0-3, counting from right)
void TM_printDigit(uint8_t digit, uint8_t value) {
  TM_printSegment(digit, TM_DATA[value & 0x0f]);
}

// Print hexadecimal value
void TM_printH(uint16_t value) {
  uint8_t i;
  for(i = 0; i < 4; i++) {
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
