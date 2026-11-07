// ===================================================================================
// I2C Functions (Software) for CH558 and CH559                               * v1.0 *
// ===================================================================================
//
// Simple I2C bitbanging for 400kHz slave devices. For system clock < 12MHz the 
// I2C clock frequency is slower. ACK bit of the slave is ignored. Clock stretching 
// by the slave is not allowed. The pins are used in open-drain mode (P0-P3 only), so
// the open-drain mode applies to the whole port (bPn_OC, reset default).
//
// PIN_SDA and PIN_SCL must be defined in config.h:
// PIN_SDA - pin connected to serial data of the I2C bus
// PIN_SCL - pin connected to serial clock of the I2C bus
// External pull-up resistors (4k7 - 10k) are mandatory!
//
// Further information:     https://github.com/wagiminator/ATtiny13-TinyOLEDdemo
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "i2c.h"
#include "gpio.h"
#include "config.h"

// ===================================================================================
// I2C Delay
// ===================================================================================
// (for 400kHz devices -> SCL low: min 1300ns, SCL high: min 600ns)
// I2C_delay() takes about 750ns including call and return (~12 cycles).
// The loop (djnz) takes about 5 cycles per iteration.
#define I2C_DLY_LOOPS   (((F_CPU / 1000000 * 750 / 1000) - 12) / 5)
#if I2C_DLY_LOOPS > 0
static void I2C_delay(void) {
  uint8_t i = I2C_DLY_LOOPS;
  while(--i);
}
  #define I2C_DELAY_H() I2C_delay()                         // SCL high >= 600ns
  #define I2C_DELAY_L() I2C_delay();I2C_delay()             // SCL low  >= 1300ns
#else
  #define I2C_DELAY_H()
  #define I2C_DELAY_L()
#endif

// ===================================================================================
// I2C Pin Macros
// ===================================================================================

// Check pin defines
#ifndef PIN_SDA
  #error PIN_SDA is undefinded
#endif
#ifndef PIN_SCL
  #error PIN_SCL is undefined
#endif

// I2C macros
#define I2C_SDA_HIGH()  PIN_high(PIN_SDA)   // release SDA -> pulled HIGH by resistor
#define I2C_SDA_LOW()   PIN_low(PIN_SDA)    // SDA LOW     -> pulled LOW  by MCU
#define I2C_SCL_HIGH()  PIN_high(PIN_SCL)   // release SCL -> pulled HIGH by resistor
#define I2C_SCL_LOW()   PIN_low(PIN_SCL)    // SCL LOW     -> pulled LOW  by MCU
#define I2C_SDA_READ()  PIN_read(PIN_SDA)   // read SDA pin
#define I2C_CLOCKOUT()  do {I2C_DELAY_L();I2C_SCL_HIGH();I2C_DELAY_H();I2C_SCL_LOW();} while(0)

// ===================================================================================
// I2C Functions
// ===================================================================================

// I2C init function
void I2C_init(void) {
  PIN_output_OD(PIN_SDA);                   // set SDA pin to open-drain OUTPUT
  PIN_output_OD(PIN_SCL);                   // set SCL pin to open-drain OUTPUT
}

// I2C transmit one data byte to the slave, ignore ACK bit, no clock stretching allowed
void I2C_write(uint8_t data) {
  uint8_t i;
  for(i=8; i; i--, data<<=1) {              // transmit 8 bits, MSB first
    (data & 0x80) ? (I2C_SDA_HIGH()) : (I2C_SDA_LOW());  // SDA HIGH if bit is 1
    I2C_CLOCKOUT();                         // clock out -> slave reads the bit
  }
  I2C_SDA_HIGH();                           // release SDA for ACK bit of slave
  I2C_CLOCKOUT();                           // 9th clock pulse is for the ignored ACK bit
}

// I2C start transmission
void I2C_start(uint8_t addr) {
  I2C_SDA_LOW();                            // start condition: SDA goes LOW first
  I2C_DELAY_H();                            // delay
  I2C_SCL_LOW();                            // start condition: SCL goes LOW second
  I2C_write(addr);                          // send slave address
}

// I2C restart transmission
void I2C_restart(uint8_t addr) {
  I2C_SDA_HIGH();                           // prepare SDA for HIGH to LOW transition
  I2C_DELAY_H();                            // delay
  I2C_SCL_HIGH();                           // restart condition: clock HIGH
  I2C_start(addr);                          // start again
}

// I2C stop transmission
void I2C_stop(void) {
  I2C_SDA_LOW();                            // prepare SDA for LOW to HIGH transition
  I2C_DELAY_H();                            // delay
  I2C_SCL_HIGH();                           // stop condition: SCL goes HIGH first
  I2C_DELAY_H();                            // delay
  I2C_SDA_HIGH();                           // stop condition: SDA goes HIGH second
}

// I2C receive one data byte from the slave (ack=0 for last byte, ack>0 if more bytes to follow)
uint8_t I2C_read(uint8_t ack) {
  uint8_t i;
  uint8_t data = 0;                         // variable for the received byte
  I2C_SDA_HIGH();                           // release SDA -> will be toggled by slave
  for(i=8; i; i--) {                        // receive 8 bits
    data <<= 1;                             // bits shifted in right (MSB first)
    I2C_DELAY_L();                          // delay (SCL low)
    I2C_SCL_HIGH();                         // clock HIGH
    I2C_DELAY_H();                          // delay (SCL high)
    if(I2C_SDA_READ()) data |= 1;           // read bit
    I2C_SCL_LOW();                          // clock LOW -> slave prepares next bit
  }
  if(ack) I2C_SDA_LOW();                    // pull SDA LOW to acknowledge (ACK)
  I2C_CLOCKOUT();                           // clock out -> slave reads ACK bit
  return data;                              // return the received byte
}
