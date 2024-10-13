// ===================================================================================
// Basic I2C Master Functions for tinyAVR 0-Series and 1-Series               * v1.2 *
// ===================================================================================
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#include "i2c.h"

#define I2C_BAUD  ((F_CPU / I2C_CLKRATE - 10) / 2);   // simplified BAUD calculation

// Read/write flag
uint8_t I2C_rwflag;

// Init I2C
void I2C_init(void) {
  // Setup GPIO pins
  #if I2C_MAP == 0

  #elif I2C_MAP == 1
    PORTMUX.CTRLB |= PORTMUX_TWI0_bm;
  #else
    #warning No automatic pin mapping for TWI0
  #endif

  // Setup and enable I2C
  TWI0.MBAUD   = I2C_BAUD;                        // set TWI master BAUD rate
  TWI0.MCTRLA  = TWI_ENABLE_bm;                   // enable TWI master
  TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;            // set bus state to idle
}

// I2C start transmission
void I2C_start(uint8_t addr) {
  TWI0.MADDR = addr;                              // start sending address
  I2C_rwflag = addr & 1;                          // set read/write flag
}

// I2C restart transmission
void I2C_restart(uint8_t addr) {
  if(!I2C_rwflag) {                               // for read operation only
    while(~TWI0.MSTATUS & TWI_WIF_bm);            // wait for last transfer to complete
  }
  I2C_start(addr);                                // start sending address
}

// I2C stop transmission
void I2C_stop(void) {
  if(!I2C_rwflag) {                               // for read operation only
    while(~TWI0.MSTATUS & TWI_WIF_bm);            // wait for last transfer to complete
  }
  TWI0.MCTRLB = TWI_MCMD_STOP_gc;                 // send stop condition
}

// I2C transmit one data byte to the slave, ignore ACK bit
void I2C_write(uint8_t data) {
  while(~TWI0.MSTATUS & TWI_WIF_bm);              // wait for last transfer to complete
  TWI0.MDATA = data;                              // start sending next data byte
}

// I2C receive one data byte from slave; ack=0: last byte, ack>0: more bytes to follow
uint8_t I2C_read(uint8_t ack) {
  while(~TWI0.MSTATUS & TWI_RIF_bm);              // wait for transfer to complete
  uint8_t data = TWI0.MDATA;                      // get received data byte
  if(ack) TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc;    // ACK:  read more bytes
  else    TWI0.MCTRLB = TWI_ACKACT_NACK_gc;       // NACK: this was the last byte
  return data;                                    // return received byte
}

// Send data buffer via I2C bus and stop
void I2C_writeBuffer(uint8_t* buf, uint16_t len) {
  while(len--) I2C_write(*buf++);                 // write buffer
  I2C_stop();                                     // stop transmission
}

// Read data via I2C bus to buffer and stop
void I2C_readBuffer(uint8_t* buf, uint16_t len) {
  while(len--) *buf++ = I2C_read(len > 0);        // read buffer
  I2C_stop();                                     // stop transmission
}
