// ===================================================================================
// I2C Master Functions with Interrupt for megaAVR 0-Series                   * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "i2c_int.h"
#include <avr/interrupt.h>

// BAUD calculation: f_SCL = F_CPU / (10 + 2 * BAUD + F_CPU * t_rise), t_rise ~ 250ns
#define I2C_BAUD_CALC   (((F_CPU / I2C_CLKRATE) - 10 - (F_CPU / 4000000)) / 2)
#if I2C_BAUD_CALC < 1
  #define I2C_BAUD      1                         // fastest possible at low F_CPU
#else
  #define I2C_BAUD      I2C_BAUD_CALC
#endif

// Read/write flag
static uint8_t I2C_rwflag;

// Interrupt driven buffer transfer
volatile uint8_t I2C_INT_busy = 0;              // buffer transfer ongoing flag
static uint8_t* volatile I2C_INT_ptr;           // pointer to next byte
static volatile uint16_t I2C_INT_len;           // remaining bytes
static volatile uint8_t  I2C_closed;            // transfer already stopped by ISR

// Init I2C
void I2C_init(void) {
  // Setup GPIO pins
  #if I2C_MAP == 0
    PORTMUX.TWISPIROUTEA = (PORTMUX.TWISPIROUTEA & ~PORTMUX_TWI0_gm) | PORTMUX_TWI0_DEFAULT_gc;
  #elif I2C_MAP == 1
    PORTMUX.TWISPIROUTEA = (PORTMUX.TWISPIROUTEA & ~PORTMUX_TWI0_gm) | PORTMUX_TWI0_ALT2_gc;
  #else
    #warning No automatic pin mapping for TWI0
  #endif

  // Setup and enable I2C
  TWI0.MBAUD   = I2C_BAUD;                        // set TWI master BAUD rate
  TWI0.MCTRLA  = TWI_ENABLE_bm;                   // enable TWI master
  TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;            // set bus state to idle
  INT_enable();                                   // global interrupt enable
}

// I2C start transmission
void I2C_start(uint8_t addr) {
  while(I2C_INT_busy);                            // wait for buffer transfer
  I2C_closed = 0;
  TWI0.MADDR = addr;                              // start sending address
  I2C_rwflag = addr & 1;                          // set read/write flag
}

// I2C restart transmission
void I2C_restart(uint8_t addr) {
  if(!I2C_rwflag) {                               // for write operation only
    while(~TWI0.MSTATUS & TWI_WIF_bm);            // wait for last transfer to complete
  }
  I2C_start(addr);                                // start sending address
}

// I2C stop transmission
void I2C_stop(void) {
  while(I2C_INT_busy);                            // wait for buffer transfer
  if(I2C_closed) return;                          // already stopped by ISR
  if(!I2C_rwflag) {                               // for write operation only
    while(~TWI0.MSTATUS & TWI_WIF_bm);            // wait for last transfer to complete
  }
  TWI0.MCTRLB = TWI_ACKACT_NACK_gc                // NACK after last byte (read only)
              | TWI_MCMD_STOP_gc;                 // send stop condition
}

// I2C transmit one data byte to the slave, ignore ACK bit
void I2C_write(uint8_t data) {
  while(I2C_INT_busy);                            // wait for buffer transfer
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

// Send data buffer via I2C bus and stop (interrupt driven, returns immediately)
void I2C_writeBuffer(uint8_t* buf, uint16_t len) {
  while(I2C_INT_busy);                            // wait for previous transfer
  if(!len) {                                      // nothing to send?
    I2C_stop();                                   // -> just stop
    return;
  }
  I2C_INT_ptr  = buf;                             // set buffer pointer
  I2C_INT_len  = len;                             // set number of bytes
  I2C_INT_busy = 1;                               // set busy flag
  TWI0.MCTRLA |= TWI_WIEN_bm;                     // enable write interrupt
}

// TWI master interrupt service routine (send next byte or STOP)
ISR(TWI0_TWIM_vect) {
  if(!I2C_INT_len || (TWI0.MSTATUS & (TWI_RXACK_bm | TWI_ARBLOST_bm | TWI_BUSERR_bm))) {
    TWI0.MCTRLA &= ~TWI_WIEN_bm;                  // disable write interrupt
    TWI0.MCTRLB  = TWI_MCMD_STOP_gc;              // send STOP condition
    I2C_closed   = 1;                             // transfer closed
    I2C_INT_busy = 0;                             // transfer finished
    return;
  }
  TWI0.MDATA = *I2C_INT_ptr++;                    // send next data byte
  I2C_INT_len--;
}

// Read data via I2C bus to buffer and stop
void I2C_readBuffer(uint8_t* buf, uint16_t len) {
  while(len--) *buf++ = I2C_read(len > 0);        // read buffer
  I2C_stop();                                     // stop transmission
}
