// ===================================================================================
// SPI Master Functions with DMA for CH32V103                                 * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// SPI_init()               Init SPI with defined clock rate (see below)
// SPI_transfer(d)          Transmit and receive one data byte
// SPI_write(d)             Transmit one data byte
// SPI_read()               Receive one data byte (sends 0xFF)
// SPI_writeBuffer(buf,len) Transmit buffer (*buf) with length (len) using DMA
// SPI_readBuffer(buf,len)  Receive buffer (*buf) with length (len), sends 0xFF
//
// SPI_busy()               Check if SPI is busy (DMA transfer or transmission ongoing)
// SPI_ready()              Check if SPI is ready to write
// SPI_available()          Check if something was received
// SPI_enable()             Enable SPI module
// SPI_disable()            Disable SPI module
// SPI_setBAUD(n)           Set prescaler BR (SPI clock = F_CPU / (2 << n), n = 0..7)
// SPI_setCPOL(n)           0: SCK low in idle, 1: SCK high in idle
// SPI_setCPHA(n)           Start sampling from 0: first clock edge, 1: second clock edge
//
// SPI pin mapping (set below in SPI parameters):
// ----------------------------------------------
// SPI_MAP     0      1      2
// Module    SPI1   SPI1   SPI2 (*)
// SCK-pin    PA5    PB3    PB13
// MISO-pin   PA6    PB4    PB14
// MOSI-pin   PA7    PB5    PB15
//
// (*) SPI2 is not available on CH32V103C6.
//
// Notes:
// ------
// - Slave select pins (NSS) must be defined and controlled by the application.
// - SPI clock rate (max. 36MHz) must be defined below; the next lower possible
//   rate F_CPU / (2 << SPI_PRESC) is used (SPI2 runs with PCLK1 = F_CPU).
// - Mode 0 (CPOL = 0, CPHA = 0), 8-bit, MSB first by default.
//
// SPI_writeBuffer() uses DMA1 channel 3 (SPI1) or 5 (SPI2) and returns immediately.
// The buffer must not be changed until SPI_busy() returns 0. Use SPI_busy() before
// releasing the slave select pin. Received data is ignored during DMA transfers.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// SPI Parameters
#ifndef SPI_MAP
#define SPI_MAP             0         // SPI pin mapping (see above)
#endif
#ifndef SPI_CLKRATE
#define SPI_CLKRATE         8000000   // SPI clock rate in Hz (max. 36MHz)
#endif

// SPI module selection
#if SPI_MAP < 2
  #define SPIx          SPI1
#else
  #define SPIx          SPI2
#endif

// SPI prescaler (BR[2:0]): SPI_CLKRATE = F_CPU / (2 << SPI_PRESC), max 36MHz
#ifndef SPI_PRESC
  #if   (F_CPU /   2) <= SPI_CLKRATE
    #define SPI_PRESC   0
  #elif (F_CPU /   4) <= SPI_CLKRATE
    #define SPI_PRESC   1
  #elif (F_CPU /   8) <= SPI_CLKRATE
    #define SPI_PRESC   2
  #elif (F_CPU /  16) <= SPI_CLKRATE
    #define SPI_PRESC   3
  #elif (F_CPU /  32) <= SPI_CLKRATE
    #define SPI_PRESC   4
  #elif (F_CPU /  64) <= SPI_CLKRATE
    #define SPI_PRESC   5
  #elif (F_CPU / 128) <= SPI_CLKRATE
    #define SPI_PRESC   6
  #else
    #define SPI_PRESC   7
  #endif
#endif
#if (F_CPU / (2 << SPI_PRESC)) > 36000000
  #warning SPI clock rate exceeds 36MHz (datasheet maximum)
#endif

// SPI Functions and Macros
#define SPI_busy()          (SPI_DMA_CHAN->CNTR || !(SPIx->STATR & SPI_STATR_TXE) || (SPIx->STATR & SPI_STATR_BSY))
#define SPI_ready()         (SPIx->STATR & SPI_STATR_TXE)
#define SPI_available()     (SPIx->STATR & SPI_STATR_RXNE)
#define SPI_enable()        SPIx->CTLR1 |=  SPI_CTLR1_SPE
#define SPI_disable()       SPIx->CTLR1 &= ~SPI_CTLR1_SPE
#define SPI_setCPOL(n)      ((n)?(SPIx->CTLR1|=SPI_CTLR1_CPOL):(SPIx->CTLR1&=~SPI_CTLR1_CPOL))
#define SPI_setCPHA(n)      ((n)?(SPIx->CTLR1|=SPI_CTLR1_CPHA):(SPIx->CTLR1&=~SPI_CTLR1_CPHA))
#define SPI_setBAUD(n)      SPIx->CTLR1 = (SPIx->CTLR1&~SPI_CTLR1_BR) | (((n)&7)<<3)

void SPI_init(void);
uint8_t SPI_transfer(uint8_t data);
#define SPI_write(d)        (void)SPI_transfer(d)  // transmit one data byte
#define SPI_read()          SPI_transfer(0xff)     // receive one data byte
void SPI_writeBuffer(const uint8_t* buf, uint16_t len);
void SPI_readBuffer(uint8_t* buf, uint16_t len);

// DMA channel define (SPI1_TX: DMA1 channel 3, SPI2_TX: DMA1 channel 5)
#if SPI_MAP < 2
  #define SPI_DMA_CHAN  DMA1_Channel3
#else
  #define SPI_DMA_CHAN  DMA1_Channel5
#endif

#ifdef __cplusplus
};
#endif
