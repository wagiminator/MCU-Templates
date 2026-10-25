// ===================================================================================
// Basic I2C Slave Functions for CH32V002, V004, V005, V006, V007, M007       * v1.0 *
// ===================================================================================
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#include "i2c_slave.h"

// Pin assignment for the selected mapping (CH32V00X RM: AFIO_PCFR1 I2C1_RM[2:0])
#ifndef I2C_V007
  #define I2C_V007 0
#endif
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

// I2C module clock in MHz (allowed range: 8..48)
#if (F_CPU < 8000000) || (F_CPU > 48000000)
  #error I2C needs a system clock between 8MHz and 48MHz
#endif
#define I2C_FREQ  (F_CPU / 1000000)


// I2C slave registers
uint8_t I2C_REG_ptr;                              // register pointer
volatile uint8_t I2C_REG[I2C_REG_SIZE];           // register array
volatile uint8_t I2C_FLAG_changed = 0;            // register change flag

#if I2C_REG_SLCT > 0
uint8_t I2C_FLAG_reg;
#endif

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
  I2C1->CTLR2     = I2C_FREQ                      // set module clock frequency in MHz
                  | I2C_CTLR2_ITBUFEN             // enable buffer interrupt
                  | I2C_CTLR2_ITEVTEN             // enable event interrupt
                  | I2C_CTLR2_ITERREN;            // enable error interrupt
  NVIC_EnableIRQ(I2C1_EV_IRQn);                   // enable event interrupt
  NVIC_EnableIRQ(I2C1_ER_IRQn);                   // enable error interrupt
  #if I2C_CLKRATE > 100000                        // Fast mode ?
    I2C1->CKCFGR  = ((F_CPU + 3 * I2C_CLKRATE - 1) / (3 * I2C_CLKRATE))   // -> clock division factor (round up)
                  | I2C_CKCFGR_FS;                // -> enable fast mode (up to 400kHz)
  #else                                           // Standard mode?
    I2C1->CKCFGR  = ((F_CPU + 2 * I2C_CLKRATE - 1) / (2 * I2C_CLKRATE));  // -> clock division factor (round up)
  #endif
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
