// ===================================================================================
// Basic SPI Master Functions (SERCOM SPI, TX only) for SAMD11                * v1.1 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "spi_tx.h"
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
  #ifdef SPI_PMUX
  PIN_output(SPI_SCK_PIN);
  PIN_alternate(SPI_SCK_PIN, SPI_PMUX);
  PIN_output(SPI_MOSI_PIN);
  PIN_alternate(SPI_MOSI_PIN, SPI_PMUX);
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
                         | SERCOM_SPI_CTRLA_DOPO(1)           // MOSI PAD2, SCK PAD3
                         | SERCOM_SPI_CTRLA_DIPO(SPI_DIPO);   // MISO pad (unused)
  SPI_SER->SPI.CTRLB.reg = 0;                                 // receiver disabled
  SPI_sync();
  SPI_SER->SPI.BAUD.reg  = SPI_BAUD_VAL;                      // set clock rate
  SPI_SER->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;          // enable SPI
  SPI_sync();
}

static uint8_t SPI_txflag = 0;                  // something was transmitted

// Check if SPI is busy (transmission ongoing)
uint8_t SPI_busy(void) {
  if(!SPI_txflag) return 0;                     // nothing transmitted yet
  return !(SPI_SER->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_TXC);
}

// Transmit one data byte
void SPI_write(uint8_t data) {
  while(!SPI_ready());                          // wait for transmit buffer empty
  SPI_SER->SPI.DATA.reg = data;                 // send data byte (clears TXC)
  SPI_txflag = 1;
}

// ===================================================================================
// Buffer Functions
// ===================================================================================

// Send buffer (*buf) with length (len) via SPI
void SPI_writeBuffer(const uint8_t* buf, uint16_t len) {
  while(len--) SPI_write(*buf++);
}
