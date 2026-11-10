// ===================================================================================
// SPI Master Functions with Interrupt for megaAVR 0-Series                   * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "spi_int.h"
#include <avr/interrupt.h>

// Interrupt driven buffer transfer
volatile uint8_t SPI_INT_busy = 0;            // buffer transfer ongoing flag
static const uint8_t* volatile SPI_INT_ptr;   // pointer to next byte
static volatile uint16_t SPI_INT_len;         // remaining bytes

// SPI init
void SPI_init(void) {
  #if SPI_MAP == 0
    VPORTA.DIR |= (1<<4) | (1<<6);              // MOSI (PA4), SCK (PA6) as output
    PORTA.PIN5CTRL |= PORT_PULLUPEN_bm;         // pullup on MISO (PA5)
    PORTMUX.TWISPIROUTEA = (PORTMUX.TWISPIROUTEA & ~PORTMUX_SPI0_gm) | PORTMUX_SPI0_DEFAULT_gc;
  #elif SPI_MAP == 1
    VPORTC.DIR |= (1<<0) | (1<<2);              // MOSI (PC0), SCK (PC2) as output
    PORTC.PIN1CTRL |= PORT_PULLUPEN_bm;         // pullup on MISO (PC1)
    PORTMUX.TWISPIROUTEA = (PORTMUX.TWISPIROUTEA & ~PORTMUX_SPI0_gm) | PORTMUX_SPI0_ALT1_gc;
  #elif SPI_MAP == 2 && defined(TCB3)
    VPORTE.DIR |= (1<<0) | (1<<2);              // MOSI (PE0), SCK (PE2) as output
    PORTE.PIN1CTRL |= PORT_PULLUPEN_bm;         // pullup on MISO (PE1)
    PORTMUX.TWISPIROUTEA = (PORTMUX.TWISPIROUTEA & ~PORTMUX_SPI0_gm) | PORTMUX_SPI0_ALT2_gc;
  #else
    #warning No automatic pin mapping for SPI0
  #endif

  SPI0.CTRLA = SPI_CLK2X_bm                     // double speed
             | SPI_ENABLE_bm                    // enable SPI
             | SPI_MASTER_bm                    // master mode
             | (SPI_PRESC << SPI_PRESC_gp);     // prescaler
  SPI0.CTRLB = SPI_SSD_bm;                      // disable SS line
  INT_enable();                                 // global interrupt enable
}

// SPI transmit and receive a byte
uint8_t SPI_transfer(uint8_t data) {
  while(SPI_INT_busy);                          // wait for buffer transfer
  SPI0.DATA = data;                             // start exchanging data byte
  while(~SPI0.INTFLAGS & SPI_IF_bm);            // wait for transfer to complete
  return SPI0.DATA;                             // return received byte
}

// ===================================================================================
// Buffer Functions
// ===================================================================================

// Transmit buffer (*buf) with length (len) (interrupt driven, returns immediately)
void SPI_writeBuffer(const uint8_t* buf, uint16_t len) {
  while(SPI_INT_busy);                          // wait for previous transfer
  if(!len) return;                              // nothing to send
  SPI_INT_ptr  = buf + 1;                       // set buffer pointer
  SPI_INT_len  = len - 1;                       // set number of remaining bytes
  SPI_INT_busy = 1;                             // set busy flag
  SPI0.INTCTRL = SPI_IE_bm;                     // enable interrupt
  SPI0.DATA    = *buf;                          // send first byte
}

// SPI interrupt service routine (send next byte)
ISR(SPI0_INT_vect) {
  (void)SPI0.DATA;                              // discard received byte
  if(SPI_INT_len) {                             // more bytes to send?
    SPI0.DATA = *SPI_INT_ptr++;                 // send next byte
    SPI_INT_len--;
  }
  else {
    SPI0.INTCTRL = 0;                           // disable interrupt
    SPI_INT_busy = 0;                           // transfer finished
  }
}

// Receive buffer (*buf) with length (len) via SPI
void SPI_readBuffer(uint8_t* buf, uint16_t len) {
  while(len--) *buf++ = SPI_transfer(0xff);
}
