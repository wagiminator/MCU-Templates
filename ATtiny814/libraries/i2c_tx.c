// ===================================================================================
// Basic I2C Master Functions (write only) for tinyAVR 0-Series and 1-Series  * v1.0 *
// ===================================================================================
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#include "i2c_tx.h"

#define I2C_BAUD  ((F_CPU / I2C_CLKRATE - 10) / 2);   // simplified BAUD calculation

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
  TWI0.MBAUD   = I2C_BAUD;                      // set TWI master BAUD rate
  TWI0.MCTRLA  = TWI_ENABLE_bm;                 // enable TWI master
  TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;          // set bus state to idle
}

// Start I2C transmission (addr must contain R/W bit)
void I2C_start(uint8_t addr) {
  TWI0.MADDR = addr;                            // start sending address
}

// Send data byte via I2C bus
void I2C_write(uint8_t data) {
  while(~TWI0.MSTATUS & TWI_WIF_bm);            // wait for last transfer to complete
  TWI0.MDATA = data;                            // start sending data byte 
}

// Stop I2C transmission
void I2C_stop(void) {
  while(~TWI0.MSTATUS & TWI_WIF_bm);            // wait for last transfer to complete
  TWI0.MCTRLB = TWI_MCMD_STOP_gc;               // send stop condition
}
