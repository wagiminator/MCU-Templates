// ===================================================================================
// I2C Master Functions with DMA for CH32V203                                 * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "i2c_dma.h"

// Select I2C module
#if I2C_MAP < 2
  #define I2Cx        I2C1
  #define I2Cx_EN     RCC_I2C1EN
  #define I2Cx_RST    RCC_I2C1RST
#else
  #define I2Cx        I2C2
  #define I2Cx_EN     RCC_I2C2EN
  #define I2Cx_RST    RCC_I2C2RST
#endif

// DMA channel (I2C1_TX: DMA1 channel 6, I2C2_TX: DMA1 channel 4)
#if I2C_MAP < 2
  #define I2C_DMA_CHAN        DMA1_Channel6
  #define I2C_DMA_IRQn        DMA1_Channel6_IRQn
  #define I2C_DMA_IRQHandler  DMA1_Channel6_IRQHandler
  #define I2C_DMA_CGIF        DMA_CGIF6
#else
  #define I2C_DMA_CHAN        DMA1_Channel4
  #define I2C_DMA_IRQn        DMA1_Channel4_IRQn
  #define I2C_DMA_IRQHandler  DMA1_Channel4_IRQHandler
  #define I2C_DMA_CGIF        DMA_CGIF4
#endif

// I2C module clock (PCLK1 = F_CPU) in MHz; FREQ[5:0] must be 2..36 (RM)
#define I2C_PCLK_MHZ  (F_CPU / 1000000)
#define I2C_FREQ      (I2C_PCLK_MHZ > 36 ? 36 : I2C_PCLK_MHZ)
#if I2C_CLKRATE > 100000 && F_CPU < 4000000
  #warning I2C fast mode (400kHz) requires PCLK1 >= 4MHz
#elif F_CPU < 2000000
  #warning I2C requires PCLK1 >= 2MHz
#endif

// Clock control value (CCR[11:0] counts PCLK1 cycles, min. 1 in fast and 4 in std mode)
#if I2C_CLKRATE > 100000
  #define I2C_CCR     ((F_CPU / (3 * I2C_CLKRATE)) < 1 ? 1 : (F_CPU / (3 * I2C_CLKRATE)))
#else
  #define I2C_CCR     ((F_CPU / (2 * I2C_CLKRATE)) < 4 ? 4 : (F_CPU / (2 * I2C_CLKRATE)))
#endif
#if I2C_CCR > 4095
  #error I2C clock rate too low for this F_CPU
#endif

// Read/write flag and "data byte written" flag
static uint8_t I2C_rwflag, I2C_txflag;
volatile uint8_t I2C_DMA_busy;                    // DMA transfer ongoing flag

// Init I2C
void I2C_init(void) {
  // Setup GPIO pins (alternate function output, open-drain, 10MHz)
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPBEN;
  #if I2C_MAP == 0
    // PB6 (SCL), PB7 (SDA)
    AFIO->PCFR1  &= ~AFIO_PCFR1_I2C1_REMAP;
    GPIOB->CFGLR  = (GPIOB->CFGLR & ~(((uint32_t)0b1111<<(6<<2)) | ((uint32_t)0b1111<<(7<<2))))
                                  |  (((uint32_t)0b1101<<(6<<2)) | ((uint32_t)0b1101<<(7<<2)));
  #elif I2C_MAP == 1
    // PB8 (SCL), PB9 (SDA)
    AFIO->PCFR1  |= AFIO_PCFR1_I2C1_REMAP;
    GPIOB->CFGHR  = (GPIOB->CFGHR & ~(((uint32_t)0b1111<<(0<<2)) | ((uint32_t)0b1111<<(1<<2))))
                                  |  (((uint32_t)0b1101<<(0<<2)) | ((uint32_t)0b1101<<(1<<2)));
  #elif I2C_MAP == 2
    // PB10 (SCL), PB11 (SDA)
    GPIOB->CFGHR  = (GPIOB->CFGHR & ~(((uint32_t)0b1111<<(2<<2)) | ((uint32_t)0b1111<<(3<<2))))
                                  |  (((uint32_t)0b1101<<(2<<2)) | ((uint32_t)0b1101<<(3<<2)));
  #else
    #error Wrong I2C_MAP
  #endif

  // Setup and enable I2C
  RCC->APB1PCENR  |=  I2Cx_EN;                    // enable I2C module clock
  RCC->APB1PRSTR  |=  I2Cx_RST;                   // reset I2C module after pin setup
  RCC->APB1PRSTR  &= ~I2Cx_RST;                   // (clears a stuck BUSY flag)
  I2Cx->CTLR2      =  I2C_FREQ;                   // set module clock rate (MHz)
  #if I2C_CLKRATE > 100000
    I2Cx->CKCFGR   =  I2C_CCR                     // set clock division factor (duty 1:2)
                   |  I2C_CKCFGR_FS;              // enable fast mode (400kHz)
    I2Cx->RTR      = (I2C_FREQ * 300 / 1000) + 1; // set maximum rise time (300ns)
  #else
    I2Cx->CKCFGR   =  I2C_CCR;                    // set clock division, standard mode
    I2Cx->RTR      =  I2C_FREQ + 1;               // set maximum rise time (1000ns)
  #endif
  I2Cx->CTLR1      =  I2C_CTLR1_PE;               // enable I2C

  // Setup DMA
  RCC->AHBPCENR |= RCC_DMA1EN;                    // enable DMA module clock
  I2C_DMA_CHAN->PADDR = (uint32_t)&I2Cx->DATAR;   // peripheral address
  I2C_DMA_CHAN->CFGR  = DMA_CFGR1_MINC            // increment memory address
                      | DMA_CFGR1_DIR             // memory to I2C
                      | DMA_CFGR1_TCIE;           // transfer complete interrupt enable
  DMA1->INTFCR        = I2C_DMA_CGIF;             // clear interrupt flags
  I2C_DMA_busy        = 0;
  NVIC_EnableIRQ(I2C_DMA_IRQn);                   // enable the DMA IRQ
}

// Start I2C transmission (addr must contain R/W bit)
void I2C_start(uint8_t addr) {
  while(I2C_DMA_busy);                            // wait for DMA transfer
  while(I2Cx->CTLR1 & I2C_CTLR1_STOP);            // wait for pending STOP condition
  if(I2Cx->STAR2 & I2C_STAR2_MSL) {               // still master -> repeated start?
    if(!I2C_rwflag && I2C_txflag)                 // after write operation?
      while(!(I2Cx->STAR1 & I2C_STAR1_BTF));      // -> wait for last byte transmitted
  }
  else while(I2Cx->STAR2 & I2C_STAR2_BUSY);       // wait until bus ready
  I2Cx->CTLR1 |= I2C_CTLR1_START                  // set START condition
              |  I2C_CTLR1_ACK;                   // set ACK
  while(!(I2Cx->STAR1 & I2C_STAR1_SB));           // wait for START generated
  I2Cx->DATAR = addr;                             // send slave address + R/W bit
  while(!(I2Cx->STAR1 & I2C_STAR1_ADDR));         // wait for address transmitted
  (void)I2Cx->STAR2;                              // clear flags
  I2C_txflag = 0;                                 // no data byte written yet
  I2C_rwflag = addr & 1;                          // set read/write flag
}

// Send data byte via I2C bus
void I2C_write(uint8_t data) {
  while(!(I2Cx->STAR1 & I2C_STAR1_TXE));          // wait for last byte transmitted
  I2Cx->DATAR = data;                             // send data byte
  I2C_txflag = 1;                                 // data byte written
}

// Read data byte via I2C bus (ack=0 for last byte, ack>0 if more bytes to follow)
uint8_t I2C_read(uint8_t ack) {
  if(!ack) {                                      // last byte?
    I2Cx->CTLR1 &= ~I2C_CTLR1_ACK;                // -> set NAK
    I2Cx->CTLR1 |=  I2C_CTLR1_STOP;               // -> set STOP condition
  }
  while(!(I2Cx->STAR1 & I2C_STAR1_RXNE));         // wait for data byte received
  return I2Cx->DATAR;                             // return received data byte
}

// Stop I2C transmission
void I2C_stop(void) {
  if(!I2C_rwflag) {                               // for write operation only
    if(I2C_txflag) while(!(I2Cx->STAR1 & I2C_STAR1_BTF)); // wait for last byte
    I2Cx->CTLR1 |= I2C_CTLR1_STOP;                // set STOP condition
  }                                               // (read stops with last byte)
}

// Send data buffer via I2C bus using DMA and stop (returns immediately)
void I2C_writeBuffer(uint8_t* buf, uint16_t len) {
  if(!len) {                                      // nothing to send?
    I2C_stop();                                   // -> just stop
    return;
  }
  if(I2C_txflag) while(!(I2Cx->STAR1 & I2C_STAR1_TXE)); // wait for last byte
  I2C_DMA_busy         = 1;                       // set busy flag
  I2C_DMA_CHAN->CFGR  &= ~DMA_CFGR1_EN;           // disable DMA channel
  I2C_DMA_CHAN->CNTR   = len;                     // number of bytes to be transfered
  I2C_DMA_CHAN->MADDR  = (uint32_t)buf;           // memory address
  I2C_DMA_CHAN->CFGR  |= DMA_CFGR1_EN;            // enable DMA channel
  I2Cx->CTLR2         |= I2C_CTLR2_DMAEN;         // enable DMA request
}

// DMA interrupt service routine (transfer complete -> STOP)
void I2C_DMA_IRQHandler(void) __attribute__((interrupt));
void I2C_DMA_IRQHandler(void) {
  I2Cx->CTLR2         &= ~I2C_CTLR2_DMAEN;        // disable DMA request
  I2C_DMA_CHAN->CFGR  &= ~DMA_CFGR1_EN;           // disable DMA channel
  DMA1->INTFCR         = I2C_DMA_CGIF;            // clear interrupt flags
  while(!(I2Cx->STAR1 & I2C_STAR1_BTF));          // wait for last byte transmitted
  I2Cx->CTLR1         |= I2C_CTLR1_STOP;          // set STOP condition
  I2C_txflag           = 0;                       // transmission closed
  I2C_rwflag           = 1;                       // (I2C_stop() does nothing)
  I2C_DMA_busy         = 0;                       // transfer finished
}

// Read data via I2C bus to buffer and stop
void I2C_readBuffer(uint8_t* buf, uint16_t len) {
  while(len--) *buf++ = I2C_read(len > 0);        // last byte with NAK and STOP
}
