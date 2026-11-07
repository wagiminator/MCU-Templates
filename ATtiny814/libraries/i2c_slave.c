// ===================================================================================
// Basic I2C Slave Functions for tinyAVR 0-Series and 1-Series                * v1.1 *
// ===================================================================================
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#include "i2c_slave.h"

// Missing in some device headers (0-series)
#ifndef PORTMUX_TWI0_bm
  #define PORTMUX_TWI0_bm 0x10
#endif

// I2C slave registers
static uint8_t I2C_REG_ptr;                       // register pointer
volatile uint8_t I2C_REG[I2C_REG_SIZE];           // register array
volatile uint8_t I2C_FLAG_changed = 0;            // register change flag
volatile uint8_t I2C_FLAG_busy = 0;               // I2C busy flag
static uint8_t I2C_FLAG_first;                    // first data byte of transaction

// I2C slave command macros
#define I2C_complete() TWI0.SCTRLB = TWI_SCMD_COMPTRANS_gc
#define I2C_response() TWI0.SCTRLB = TWI_SCMD_RESPONSE_gc
#define I2C_sendACK()  TWI0.SCTRLB = TWI_ACKACT_ACK_gc  | TWI_SCMD_RESPONSE_gc
#define I2C_sendNACK() TWI0.SCTRLB = TWI_ACKACT_NACK_gc | TWI_SCMD_RESPONSE_gc
#define I2C_put(x)     TWI0.SDATA  = (x)
#define I2C_get()      TWI0.SDATA

// I2C slave status macros
#define I2C_isAddr()  ((TWI0.SSTATUS & TWI_APIF_bm) && (TWI0.SSTATUS & TWI_AP_bm))
#define I2C_isData()  (TWI0.SSTATUS & TWI_DIF_bm)
#define I2C_isStop()  ((TWI0.SSTATUS & TWI_APIF_bm) && (~TWI0.SSTATUS & TWI_AP_bm))
#define I2C_isIn()    (~TWI0.SSTATUS & TWI_DIR_bm)
#define I2C_isOut()   (TWI0.SSTATUS & TWI_DIR_bm)
#define I2C_isNACK()  (TWI0.SSTATUS & TWI_RXACK_bm)
#define I2C_isError() (TWI0.SSTATUS & (TWI_COLL_bm | TWI_BUSERR_bm))

// I2C slave init
void I2C_init(void) {
  // Setup GPIO pins
  #if I2C_MAP == 0
    PORTMUX.CTRLB &= ~PORTMUX_TWI0_bm;
  #elif I2C_MAP == 1 && defined(PORTB)
    PORTMUX.CTRLB |= PORTMUX_TWI0_bm;
  #else
    #warning No automatic pin mapping for TWI0
  #endif

  // Setup and enable I2C slave
  TWI0.SADDR  = I2C_ADDR << 1;                    // set address (LSB is R/W bit)
  TWI0.SCTRLA = TWI_DIEN_bm                       // data interrupt enable
              | TWI_APIEN_bm                      // address or stop interrupt enable
              | TWI_PIEN_bm                       // stop interrupt enable
              | TWI_ENABLE_bm;                    // enable I2C slave
  INT_enable();                                   // global interrupt enable
}

// I2C slave interrupt service routine
ISR(TWI0_TWIS_vect) { 
  // Bus error or collision: abort transaction
  if(I2C_isError()) {
    TWI0.SSTATUS = TWI_COLL_bm | TWI_BUSERR_bm;   // clear error flags
    I2C_complete();                               // wait for next start condition
    I2C_FLAG_busy = 0;
    return;
  }

  // Address match interrupt handler
  if(I2C_isAddr()) {                              // address match?
    I2C_sendACK();                                // send ACK to master
    #if I2C_REG_SLCT == 0
    I2C_REG_ptr = 0;                              // always start with register 0
    #endif
    I2C_FLAG_first = 1;                           // next byte is the first one
    I2C_FLAG_busy  = 1;                           // set I2C busy flag
    return;                                       // quit ISR
  }
  
  // Data interrupt handler
  if(I2C_isData()) {                              // data transmission?
    if(I2C_isOut()) {                             // slave writing to master?
      if(!I2C_FLAG_first && I2C_isNACK()) {       // master sent NACK?
        I2C_complete();                           // -> transaction is complete
        return;
      }
      I2C_put(I2C_REG[I2C_REG_ptr]);              // send register value to master
      I2C_response();                             // no ACK needed here
    }
    else {                                        // slave reading from master?
      uint8_t data = I2C_get();                   // get data byte
      #if I2C_REG_SLCT > 0
      if(I2C_FLAG_first) {                        // first byte is register address
        I2C_REG_ptr = (data < I2C_REG_SIZE) ? data : 0;
        I2C_FLAG_first = 0;
        I2C_sendACK();                            // send ACK to master
        return;                                   // don't increase pointer
      }
      #endif
      I2C_REG[I2C_REG_ptr] = data;                // write register value from master
      I2C_FLAG_changed = 1;                       // set register changed flag
      I2C_sendACK();                              // send ACK to master
    }
    I2C_FLAG_first = 0;
    if(++I2C_REG_ptr >= I2C_REG_SIZE)             // increase pointer...
      I2C_REG_ptr = 0;                            // ...or wrap around
    return;                                       // quit ISR
  }

  // Stop condition interrupt handler
  if(I2C_isStop()) {                              // stop condition?
    I2C_complete();                               // complete transaction
    I2C_FLAG_busy = 0;                            // clear I2C busy flag
  }
}
