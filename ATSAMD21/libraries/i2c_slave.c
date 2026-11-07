// ===================================================================================
// Basic I2C Slave Functions (SERCOM I2CS) for SAMD21                         * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "i2c_slave.h"
#include "gpio.h"

// I2C slave registers
uint8_t I2C_REG_ptr;                              // register pointer
volatile uint8_t I2C_REG[I2C_REG_SIZE];           // register array
volatile uint8_t I2C_FLAG_changed = 0;            // register change flag
#if I2C_REG_SLCT > 0
uint8_t I2C_FLAG_reg;                             // next received byte is reg pointer
#endif

// Acknowledge and continue (receive next byte / send data)
#define I2C_ACK_CMD()   (I2C_SER->I2CS.CTRLB.reg = SERCOM_I2CS_CTRLB_SMEN \
                                                 | SERCOM_I2CS_CTRLB_CMD(3))

// Init I2C
void I2C_init(void) {
  // Set GPIO pins
  #ifdef I2C_SDA_PMUX
  PIN_input_PU(I2C_SDA_PIN);
  PIN_alternate(I2C_SDA_PIN, I2C_SDA_PMUX);
  PIN_input_PU(I2C_SCL_PIN);
  PIN_alternate(I2C_SCL_PIN, I2C_SCL_PMUX);
  #else
    #warning No automatic pin mapping for I2C
  #endif

  // Enable SERCOM clocks
  PM->APBCMASK.reg |= I2C_APBMASK;                // enable SERCOM module clock
  GCLK_setCH(I2C_GCLK_ID, GCLK_SYS);              // GCLK0 -> SERCOM

  // Setup and enable I2C slave
  I2C_SER->I2CS.CTRLA.reg = SERCOM_I2CS_CTRLA_SWRST;
  while(I2C_SER->I2CS.CTRLA.reg & SERCOM_I2CS_CTRLA_SWRST);
  I2C_SER->I2CS.CTRLA.reg = SERCOM_I2CS_CTRLA_MODE_I2C_SLAVE    // I2C slave mode
                          | SERCOM_I2CS_CTRLA_SDAHOLD(2);       // SDA hold 300-600ns
  I2C_SER->I2CS.CTRLB.reg = SERCOM_I2CS_CTRLB_SMEN;             // smart mode
  I2C_SER->I2CS.ADDR.reg  = SERCOM_I2CS_ADDR_ADDR(I2C_ADDR);    // set I2C address
  I2C_SER->I2CS.INTENSET.reg = SERCOM_I2CS_INTENSET_PREC        // stop interrupt
                             | SERCOM_I2CS_INTENSET_AMATCH      // address match int
                             | SERCOM_I2CS_INTENSET_DRDY;       // data ready interrupt
  I2C_SER->I2CS.CTRLA.reg |= SERCOM_I2CS_CTRLA_ENABLE;          // enable I2C
  while(I2C_SER->I2CS.SYNCBUSY.reg & SERCOM_I2CS_SYNCBUSY_ENABLE);
  NVIC_EnableIRQ(I2C_IRQn);                                     // enable interrupt
}

// I2C slave interrupt service routine
void I2C_IRQHandler(void) {
  uint8_t  flags  = I2C_SER->I2CS.INTFLAG.reg;
  uint16_t status = I2C_SER->I2CS.STATUS.reg;
  if(I2C_REG_ptr >= I2C_REG_SIZE) I2C_REG_ptr = 0;  // keep pointer in range

  // Stop condition or address match (start/repeated start)
  if(flags & (SERCOM_I2CS_INTFLAG_PREC | SERCOM_I2CS_INTFLAG_AMATCH)) {
    if(flags & SERCOM_I2CS_INTFLAG_PREC)
      I2C_SER->I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_PREC; // clear stop flag
    if(flags & SERCOM_I2CS_INTFLAG_AMATCH) {
      #if I2C_REG_SLCT > 0
      if(!(status & SERCOM_I2CS_STATUS_DIR)) I2C_FLAG_reg = 1;  // write: first byte
                                                    // is reg ptr, read: continue at ptr
      #else
      I2C_REG_ptr = 0;                              // reset register pointer
      #endif
    }
    I2C_ACK_CMD();                                  // acknowledge address
  }

  // Data ready
  else if(flags & SERCOM_I2CS_INTFLAG_DRDY) {
    if(status & SERCOM_I2CS_STATUS_DIR) {           // master read?
      I2C_SER->I2CS.DATA.reg = I2C_REG[I2C_REG_ptr++];  // send register value
    }
    else {                                          // master write
      uint8_t data = I2C_SER->I2CS.DATA.reg;        // read received byte
      #if I2C_REG_SLCT > 0
      if(I2C_FLAG_reg) {
        I2C_REG_ptr = data;                         // set register address (pointer)
        if(I2C_REG_ptr >= I2C_REG_SIZE) I2C_REG_ptr = 0;  // keep pointer in range
        I2C_FLAG_reg = 0;                           // next bytes will be data
      }
      else
      #endif
      {
        I2C_REG[I2C_REG_ptr++] = data;              // write register value
        I2C_FLAG_changed = 1;                       // set register changed flag
      }
      I2C_ACK_CMD();                                // acknowledge, receive next byte
    }
  }

  // Wrap around register pointer
  if(I2C_REG_ptr >= I2C_REG_SIZE) I2C_REG_ptr = 0;
}
