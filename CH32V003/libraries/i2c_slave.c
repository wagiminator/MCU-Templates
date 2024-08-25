// ===================================================================================
// Basic I2C Slave Functions for CH32V003                                     * v1.0 *
// ===================================================================================
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#include "i2c_slave.h"

// I2C slave registers
uint8_t I2C_REG_ptr;                              // register pointer
volatile uint8_t I2C_REG[I2C_REG_SIZE];           // register array
volatile uint8_t I2C_FLAG_changed = 0;            // register change flag

#if I2C_REG_SLCT > 0
uint8_t I2C_FLAG_reg;
#endif

// Init I2C
void I2C_init(void) {
  // Setup GPIO pins
  #if I2C_MAP == 0
    // Set pin PC1 (SDA) and PC2 (SCL) to output, open-drain, 10MHz, multiplex
    RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN;
    GPIOC->CFGLR = (GPIOC->CFGLR & ~(((uint32_t)0b1111<<(1<<2)) | ((uint32_t)0b1111<<(2<<2))))
                                 |  (((uint32_t)0b1101<<(1<<2)) | ((uint32_t)0b1101<<(2<<2)));
  #elif I2C_MAP == 1
    // Set pin PD0 (SDA) and PD1 (SCL) to output, open-drain, 10MHz, multiplex
    RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPDEN;
    AFIO->PCFR1    |= 1<<1;
    GPIOD->CFGLR = (GPIOD->CFGLR & ~(((uint32_t)0b1111<<(0<<2)) | ((uint32_t)0b1111<<(1<<2))))
                                 |  (((uint32_t)0b1101<<(0<<2)) | ((uint32_t)0b1101<<(1<<2)));
  #elif I2C_MAP == 2
    // Set pin PC6 (SDA) and PC5 (SCL) to output, open-drain, 10MHz, multiplex
    RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN;
    AFIO->PCFR1    |= 1<<22;
    GPIOC->CFGLR = (GPIOC->CFGLR & ~(((uint32_t)0b1111<<(6<<2)) | ((uint32_t)0b1111<<(5<<2))))
                                 |  (((uint32_t)0b1101<<(6<<2)) | ((uint32_t)0b1101<<(5<<2)));
  #else
    #warning Wrong I2C REMAP
  #endif

  // Setup and enable I2C
  RCC->APB1PCENR |= RCC_I2C1EN;                   // enable I2C module clock
  I2C1->CTLR2     = 4                             // set input clock rate
                  | I2C_CTLR2_ITBUFEN             // enable buffer interrupt
                  | I2C_CTLR2_ITEVTEN             // enable event interrupt
                  | I2C_CTLR2_ITERREN;            // enable error interrupt
  NVIC_EnableIRQ(I2C1_EV_IRQn);                   // enable event interrupt
  NVIC_EnableIRQ(I2C1_ER_IRQn);                   // enable error interrupt
  I2C1->CKCFGR    = (F_CPU / (3 * I2C_CLKRATE))   // set clock division factor 1:2
                  | I2C_CKCFGR_FS;                // enable fast mode (400kHz)
  I2C1->OADDR1    = I2C_ADDR << 1;                // set I2C address
  I2C1->OADDR2    = 0;                            // no secondary address
  I2C1->CTLR1     = I2C_CTLR1_ACK                 // enable auto-acknowledge
                  | I2C_CTLR1_PE;                 // enable I2C
}

// I2C slave interrupt service routine
void I2C1_EV_IRQHandler(void) __attribute__((interrupt));
void I2C1_EV_IRQHandler(void) {
  uint16_t star1, star2 __attribute__((unused));
  star1 = I2C1->STAR1;
  star2 = I2C1->STAR2;

  // Address match handler
  if(star1 & I2C_STAR1_ADDR) {
    I2C_REG_ptr   = 0;                            // reset register pointer
    #if I2C_REG_SLCT > 0
    I2C_FLAG_reg  = 1;                            // first byte will be reg ptr
    #endif
  }

  // Byte received handler
  if(star1 & I2C_STAR1_RXNE) {
    #if I2C_REG_SLCT > 0
    if(I2C_FLAG_reg) {
      I2C_REG_ptr = I2C1->DATAR;                  // read register address (pointer)
      I2C_FLAG_reg = 0;                           // next bytes will be data
    }
    else
    #endif
    {
      I2C_REG[I2C_REG_ptr++] = I2C1->DATAR;       // read register value from master
      I2C_FLAG_changed = 1;                       // set register changed flag
    }
  }

  // Byte requested handler
  if(star1 & I2C_STAR1_TXE) {
    I2C1->DATAR = I2C_REG[I2C_REG_ptr++];         // send register value to master
  }

  // Stop condition handler
  if(star1 & I2C_STAR1_STOPF) {
    I2C1->CTLR1 &= ~(I2C_CTLR1_STOP);             // clear stop flag
  }

  // Wrap around register pointer
  if(I2C_REG_ptr >= I2C_REG_SIZE) {
    I2C_REG_ptr = 0;
  }
}

// I2C error interrupt service routine
void I2C1_ER_IRQHandler(void) __attribute__((interrupt));
void I2C1_ER_IRQHandler(void) {
  I2C1->STAR1 &= ~(I2C_STAR1_BERR | I2C_STAR1_ARLO | I2C_STAR1_AF);
}
