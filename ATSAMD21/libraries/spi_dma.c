// ===================================================================================
// SPI Master Functions with DMA (SERCOM SPI, DMAC) for SAMD21                * v1.1 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "spi_dma.h"
#include "gpio.h"

// Change CTRLA register (SPI is disabled while changing enable-protected bits)
void SPI_setCTRLA(uint32_t mask, uint32_t val) {
  uint32_t ctrla = SPI_SER->SPI.CTRLA.reg;
  SPI_SER->SPI.CTRLA.reg = ctrla & ~SERCOM_SPI_CTRLA_ENABLE;
  SPI_sync();
  ctrla = (ctrla & ~mask) | val;
  SPI_SER->SPI.CTRLA.reg = ctrla & ~SERCOM_SPI_CTRLA_ENABLE;
  if(ctrla & SERCOM_SPI_CTRLA_ENABLE) SPI_SER->SPI.CTRLA.reg = ctrla;
  SPI_sync();
}

// Set BAUD register (SCK = F_CPU / (2 * (n + 1)))
void SPI_setBAUD(uint8_t n) {
  uint32_t ctrla = SPI_SER->SPI.CTRLA.reg;
  SPI_setCTRLA(SERCOM_SPI_CTRLA_ENABLE, 0);     // BAUD is enable-protected
  SPI_SER->SPI.BAUD.reg = n;
  SPI_setCTRLA(SERCOM_SPI_CTRLA_ENABLE, ctrla & SERCOM_SPI_CTRLA_ENABLE);
}

// Init SPI
void SPI_init(void) {
  // Set GPIO pins
  #ifdef SPI_SCK_PMUX
  PIN_output(SPI_SCK_PIN);
  PIN_alternate(SPI_SCK_PIN, SPI_SCK_PMUX);
  PIN_output(SPI_MOSI_PIN);
  PIN_alternate(SPI_MOSI_PIN, SPI_MOSI_PMUX);
  PIN_input_PU(SPI_MISO_PIN);
  PIN_alternate(SPI_MISO_PIN, SPI_MISO_PMUX);
  #else
    #warning No automatic pin mapping for SPI
  #endif

  // Enable SERCOM clocks
  PM->APBCMASK.reg |= SPI_APBMASK;              // enable SERCOM module clock
  GCLK_setCH(SPI_GCLK_ID, GCLK_SYS);            // GCLK0 -> SERCOM

  // Setup and enable SPI master, mode 0, MSB first, 8-bit
  SPI_SER->SPI.CTRLA.reg = SERCOM_SPI_CTRLA_SWRST;
  while(SPI_SER->SPI.CTRLA.reg & SERCOM_SPI_CTRLA_SWRST);
  SPI_SER->SPI.CTRLA.reg = SERCOM_SPI_CTRLA_MODE_SPI_MASTER   // master mode
                         | SERCOM_SPI_CTRLA_DOPO(SPI_DOPO)    // MOSI/SCK pads
                         | SERCOM_SPI_CTRLA_DIPO(SPI_DIPO);   // MISO pad
  SPI_SER->SPI.CTRLB.reg = SERCOM_SPI_CTRLB_RXEN;             // enable receiver
  SPI_sync();
  SPI_SER->SPI.BAUD.reg  = SPI_BAUD_VAL;                      // set clock rate
  SPI_SER->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;          // enable SPI
  SPI_sync();

  // Setup DMA channel (trigger: SERCOM TX, one byte per trigger)
  DMA_init();                                                 // init DMAC
  DMAC->CHID.reg     = DMAC_CHID_ID(SPI_DMA_CHANNEL);         // select channel
  DMAC->CHCTRLA.reg  = DMAC_CHCTRLA_SWRST;                    // reset channel
  while(DMAC->CHCTRLA.reg & DMAC_CHCTRLA_SWRST);
  DMAC->CHCTRLB.reg  = DMAC_CHCTRLB_TRIGSRC(SPI_DMAC_ID_TX)   // trigger: SERCOM TX
                     | DMAC_CHCTRLB_TRIGACT_BEAT              // one beat per trigger
                     | DMAC_CHCTRLB_LVL(0);                   // priority level 0
  DMA_DESC(SPI_DMA_CHANNEL).DSTADDR.reg  = (uint32_t)&SPI_SER->SPI.DATA.reg;
  DMA_DESC(SPI_DMA_CHANNEL).DESCADDR.reg = 0;                 // single block
}

static uint8_t SPI_txflag = 0;                  // something was transmitted

// Check if SPI is busy (DMA transfer or transmission ongoing)
uint8_t SPI_busy(void) {
  uint32_t en;
  if(!SPI_txflag) return 0;                     // nothing transmitted yet
  DMAC->CHID.reg = DMAC_CHID_ID(SPI_DMA_CHANNEL);
  en = DMAC->CHCTRLA.reg & DMAC_CHCTRLA_ENABLE; // DMA channel still active?
  if(en) return 1;
  return !(SPI_SER->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_TXC);
}

// Transmit buffer using DMA (returns immediately)
void SPI_writeBuffer(const uint8_t* buf, uint16_t len) {
  if(!len) return;
  while(SPI_busy());                            // wait for previous transfer
  SPI_SER->SPI.CTRLB.reg &= ~SERCOM_SPI_CTRLB_RXEN; // ignore received data
  SPI_sync();
  DMA_DESC(SPI_DMA_CHANNEL).BTCTRL.reg  = DMAC_BTCTRL_VALID         // valid descriptor
                                        | DMAC_BTCTRL_BEATSIZE_BYTE // 8-bit transfers
                                        | DMAC_BTCTRL_SRCINC;       // increment source
  DMA_DESC(SPI_DMA_CHANNEL).BTCNT.reg   = len;                      // number of bytes
  DMA_DESC(SPI_DMA_CHANNEL).SRCADDR.reg = (uint32_t)buf + len;      // end address
  SPI_txflag = 1;
  DMAC->CHID.reg    = DMAC_CHID_ID(SPI_DMA_CHANNEL);
  DMAC->CHCTRLA.reg = DMAC_CHCTRLA_ENABLE;      // start transfer
}

// Transfer one data byte (read and write)
uint8_t SPI_transfer(uint8_t data) {
  while(SPI_busy());                            // wait for DMA transfer
  if(!(SPI_SER->SPI.CTRLB.reg & SERCOM_SPI_CTRLB_RXEN)) {
    SPI_SER->SPI.CTRLB.reg |= SERCOM_SPI_CTRLB_RXEN; // enable receiver again
    SPI_sync();
  }
  SPI_SER->SPI.DATA.reg = data;                 // send data byte
  while(!SPI_available());                      // wait for data byte received
  return SPI_SER->SPI.DATA.reg;                 // return received data byte
}

// Transmit one data byte
void SPI_write(uint8_t data) {
  (void)SPI_transfer(data);
}

// ===================================================================================
// Buffer Functions
// ===================================================================================

// Receive buffer (*buf) with length (len) via SPI
void SPI_readBuffer(uint8_t* buf, uint16_t len) {
  while(len--) *buf++ = SPI_transfer(0xff);
}
