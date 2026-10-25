// ===================================================================================
// Basic I2C Master Functions for CH32V002, V004, V005, V006, V007, M007      * v1.0 *
// ===================================================================================
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#include "i2c.h"

// Pin assignment for the selected mapping (CH32V00X RM: AFIO_PCFR1 I2C1_RM[2:0])
#if   I2C_MAP == 0                      // SDA: PC1, SCL: PC2
  #define I2C_SDA_GPIO  GPIOC
  #define I2C_SDA_PIN   1
  #define I2C_SCL_GPIO  GPIOC
  #define I2C_SCL_PIN   2
  #define I2C_PORT_EN   RCC_IOPCEN
#elif I2C_MAP == 1                      // SDA: PD0, SCL: PD1
  #define I2C_SDA_GPIO  GPIOD
  #define I2C_SDA_PIN   0
  #define I2C_SCL_GPIO  GPIOD
  #define I2C_SCL_PIN   1
  #define I2C_PORT_EN   RCC_IOPDEN
#elif I2C_MAP == 2                      // SDA: PC6 (V007/M007: PC4), SCL: PC5
  #define I2C_SDA_GPIO  GPIOC
  #if I2C_V007
    #define I2C_SDA_PIN 4
  #else
    #define I2C_SDA_PIN 6
  #endif
  #define I2C_SCL_GPIO  GPIOC
  #define I2C_SCL_PIN   5
  #define I2C_PORT_EN   RCC_IOPCEN
#elif I2C_MAP == 3                      // SDA: PB6, SCL: PB5
  #define I2C_SDA_GPIO  GPIOB
  #define I2C_SDA_PIN   6
  #define I2C_SCL_GPIO  GPIOB
  #define I2C_SCL_PIN   5
  #define I2C_PORT_EN   RCC_IOPBEN
#elif I2C_MAP == 4                      // SDA: PD1, SCL: PB3
  #define I2C_SDA_GPIO  GPIOD
  #define I2C_SDA_PIN   1
  #define I2C_SCL_GPIO  GPIOB
  #define I2C_SCL_PIN   3
  #define I2C_PORT_EN   (RCC_IOPDEN | RCC_IOPBEN)
#else
  #error Wrong I2C_MAP (0..4)
#endif

// I2C module clock in MHz (allowed range: 8..48)
#if (F_CPU < 8000000) || (F_CPU > 48000000)
  #error I2C needs a system clock between 8MHz and 48MHz
#endif
#define I2C_FREQ  (F_CPU / 1000000)

// Read/write flag
static uint8_t I2C_rwflag;

// Init I2C
void I2C_init(void) {
  // Enable AFIO, GPIO port(s) and I2C module clock
  RCC->PB2PCENR |= RCC_AFIOEN | I2C_PORT_EN;
  RCC->PB1PCENR |= RCC_I2C1EN;

  // Select pin mapping (I2C1_RM[2:0] = AFIO_PCFR1[5:3])
  AFIO->PCFR1 = (AFIO->PCFR1 & ~AFIO_PCFR1_I2C1_REMAP) | ((uint32_t)I2C_MAP << 3);

  // Set SDA and SCL pins to output, open-drain, 30MHz, multiplex
  I2C_SDA_GPIO->CFGLR = (I2C_SDA_GPIO->CFGLR & ~((uint32_t)0b1111 << (I2C_SDA_PIN << 2)))
                                             |  ((uint32_t)0b1101 << (I2C_SDA_PIN << 2));
  I2C_SCL_GPIO->CFGLR = (I2C_SCL_GPIO->CFGLR & ~((uint32_t)0b1111 << (I2C_SCL_PIN << 2)))
                                             |  ((uint32_t)0b1101 << (I2C_SCL_PIN << 2));

  // Setup and enable I2C
  I2C1->CTLR1     = 0;                            // disable I2C for configuration
  I2C1->CTLR2     = I2C_FREQ;                     // set module clock frequency in MHz
  #if I2C_CLKRATE > 100000                        // Fast mode ?
    I2C1->CKCFGR  = ((F_CPU + 3 * I2C_CLKRATE - 1) / (3 * I2C_CLKRATE))   // -> clock division factor (round up)
                  | I2C_CKCFGR_FS;                // -> enable fast mode (up to 400kHz)
  #else                                           // Standard mode?
    I2C1->CKCFGR  = ((F_CPU + 2 * I2C_CLKRATE - 1) / (2 * I2C_CLKRATE));  // -> clock division factor (round up)
  #endif
  I2C1->CTLR1   = I2C_CTLR1_PE;                   // enable I2C
}

// Start I2C transmission (addr must contain R/W bit)
void I2C_start(uint8_t addr) {
  while(I2C1->STAR2 & I2C_STAR2_BUSY);            // wait until bus ready
  I2C1->CTLR1 |= I2C_CTLR1_START                  // set START condition
               | I2C_CTLR1_ACK;                   // set ACK
  while(!(I2C1->STAR1 & I2C_STAR1_SB));           // wait for START generated
  I2C1->DATAR = addr;                             // send slave address + R/W bit
  while(!(I2C1->STAR1 & I2C_STAR1_ADDR));         // wait for address transmitted
  (void)I2C1->STAR2;                              // clear flags (read STAR1, then STAR2)
  I2C_rwflag = addr & 1;                          // set read/write flag
}

// Send data byte via I2C bus
void I2C_write(uint8_t data) {
  while(!(I2C1->STAR1 & I2C_STAR1_TXE));          // wait for last byte transmitted
  I2C1->DATAR = data;                             // send data byte
}

// Read data byte via I2C bus (ack=0 for last byte, ack>0 if more bytes to follow)
uint8_t I2C_read(uint8_t ack) {
  if(!ack) {                                      // last byte?
    I2C1->CTLR1 &= ~I2C_CTLR1_ACK;                // -> set NAK
    I2C1->CTLR1 |=  I2C_CTLR1_STOP;               // -> set STOP condition
  }
  while(!(I2C1->STAR1 & I2C_STAR1_RXNE));         // wait for data byte received
  return I2C1->DATAR;                             // return received data byte
}

// Stop I2C transmission (a read operation is stopped by I2C_read(0))
void I2C_stop(void) {
  if(!I2C_rwflag) {                               // for write operation only
    while(!(I2C1->STAR1 & I2C_STAR1_BTF));        // wait for last byte transmitted
    I2C1->CTLR1 |= I2C_CTLR1_STOP;                // set STOP condition
  }
}

// Send data buffer via I2C bus and stop
void I2C_writeBuffer(uint8_t* buf, uint16_t len) {
  while(len--) I2C_write(*buf++);                 // write buffer
  I2C_stop();                                     // stop transmission
}

// Read data via I2C bus to buffer and stop
void I2C_readBuffer(uint8_t* buf, uint16_t len) {
  while(len--) *buf++ = I2C_read(len > 0);
}
