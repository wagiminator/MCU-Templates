// ===================================================================================
// Basic SPI Master Functions (SERCOM SPI) for SAMD21                         * v1.1 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "spi.h"
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
}

// Transfer one data byte (read and write)
uint8_t SPI_transfer(uint8_t data) {
  while(!SPI_ready());                          // wait for transmit buffer empty
  SPI_SER->SPI.DATA.reg = data;                 // send data byte
  while(!SPI_available());                      // wait for data byte received
  return SPI_SER->SPI.DATA.reg;                 // return received data byte
}

// ===================================================================================
// Buffer Functions
// ===================================================================================

// Send buffer (*buf) with length (len) via SPI
void SPI_writeBuffer(const uint8_t* buf, uint16_t len) {
  while(len--) SPI_transfer(*buf++);
}

// Receive buffer (*buf) with length (len) via SPI
void SPI_readBuffer(uint8_t* buf, uint16_t len) {
  while(len--) *buf++ = SPI_transfer(0xff);
}
