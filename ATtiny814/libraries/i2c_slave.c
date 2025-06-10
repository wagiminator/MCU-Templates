// ===================================================================================
// Basic I2C Slave Functions for tinyAVR 0-Series and 1-Series                * v1.0 *
// ===================================================================================
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#include "i2c_slave.h"

// I2C slave registers
uint8_t I2C_REG_ptr;                              // register pointer
volatile uint8_t I2C_REG[I2C_REG_SIZE];           // register array
volatile uint8_t I2C_FLAG_changed = 0;            // register change flag
volatile uint8_t I2C_FLAG_busy = 0;               // I2C busy flag

#if I2C_REG_SLCT > 0
uint8_t I2C_FLAG_reg;
#endif

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

// I2C slave init
void I2C_init(void) {
  // Setup GPIO pins
  #if I2C_MAP == 0

  #elif I2C_MAP == 1
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
  // Address match interrupt handler
  if(I2C_isAddr()) {                              // address match?
    I2C_sendACK();                                // send ACK to master
    I2C_REG_ptr = 0;                              // reset register pointer
    I2C_FLAG_busy = 1;                            // set I2C busy flag
    #if I2C_REG_SLCT > 0
    I2C_FLAG_reg  = 1;                            // first byte will be reg ptr
    #endif
    return;                                       // quit ISR
  }
  
  // Data interrupt handler
  if(I2C_isData()) {                              // data transmission?
    if(I2C_isOut()) {                             // slave writing to master?
      I2C_put(I2C_REG[I2C_REG_ptr]);              // send register value to master
      I2C_response();                             // no ACK needed here
    }
    else {                                        // slave reading from master?
      #if I2C_REG_SLCT > 0
      if(I2C_FLAG_reg) {
        I2C_REG_ptr = I2C_get();;                 // read register address (pointer)
        I2C_FLAG_reg = 0;                         // next bytes will be data
      }
      else
      #endif
      {
        I2C_REG[I2C_REG_ptr] = I2C_get();         // read register value from master
        I2C_FLAG_changed = 1;                     // set register changed flag
      }
      I2C_sendACK();                              // send ACK to master
    }
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
