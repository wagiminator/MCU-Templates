// ===================================================================================
// SPI Master Functions with DMA for STM32G0xx                                * v1.0 *
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
// SPI_setBAUD(n)           Set BAUD rate (see below)
// SPI_setCPOL(n)           0: SCK low in idle, 1: SCK high in idle
// SPI_setCPHA(n)           Start sampling from 0: first clock edge, 1: second clock edge
//
// SPI pin mapping (set below in SPI parameters):
// ----------------------------------------------
// SPI_MAP    0     1     2     3     4
// SCK-pin   PA5   PA1   PA5   PB3   no mapping
// MISO-pin  PA6   PA6   PA11  PB4   no mapping
// MOSI-pin  PA7   PA2   PA12  PB5   no mapping
//
// Slave select pins (NSS) must be defined and controlled by the application.
// SPI clock rate must be defined below (max. 32MHz in master mode).
//
// SPI_writeBuffer() uses the DMA channel defined below (1 - 5) and returns
// immediately. The buffer must not be changed until SPI_busy() returns 0. Use
// SPI_busy() before releasing the slave select pin. Received data is ignored during
// DMA transfers.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// SPI Parameters
#define SPI_PRESC           5     // SPI_CLKRATE = F_CPU / (2 << SPI_PRESC)
#define SPI_MAP             0     // SPI pin mapping (see above)
#define SPI_DMA_CHANNEL     2     // DMA channel (1 - 5)

// SPI Functions and Macros
#define SPI_busy()          (SPI_DMA_CHAN->CNDTR || !(SPI1->SR & SPI_SR_TXE) || (SPI1->SR & SPI_SR_BSY))
#define SPI_ready()         (SPI1->SR & SPI_SR_TXE)
#define SPI_available()     (SPI1->SR & SPI_SR_RXNE)

#define SPI_enable()        (SPI1->CR1 |=  SPI_CR1_SPE)
#define SPI_disable()       (SPI1->CR1 &= ~SPI_CR1_SPE)

// BR, CPOL and CPHA should only be changed while no communication is ongoing
#define SPI_setCPOL(n)      ((n)?(SPI1->CR1|=SPI_CR1_CPOL):(SPI1->CR1&=~SPI_CR1_CPOL))
#define SPI_setCPHA(n)      ((n)?(SPI1->CR1|=SPI_CR1_CPHA):(SPI1->CR1&=~SPI_CR1_CPHA))
#define SPI_setBAUD(n)      (SPI1->CR1 = (SPI1->CR1&~SPI_CR1_BR) | (((n)&7)<<3))

void SPI_init(void);
uint8_t SPI_transfer(uint8_t data);
#define SPI_write(d)        (void)SPI_transfer(d)  // transmit one data byte
#define SPI_read()          SPI_transfer(0xff)     // receive one data byte
void SPI_writeBuffer(const uint8_t* buf, uint16_t len);
void SPI_readBuffer(uint8_t* buf, uint16_t len);

// DMA channel defines
#if   SPI_DMA_CHANNEL == 1
  #define SPI_DMA_CHAN    DMA1_Channel1
  #define SPI_DMA_MUX     DMAMUX1_Channel0
#elif SPI_DMA_CHANNEL == 2
  #define SPI_DMA_CHAN    DMA1_Channel2
  #define SPI_DMA_MUX     DMAMUX1_Channel1
#elif SPI_DMA_CHANNEL == 3
  #define SPI_DMA_CHAN    DMA1_Channel3
  #define SPI_DMA_MUX     DMAMUX1_Channel2
#elif SPI_DMA_CHANNEL == 4
  #define SPI_DMA_CHAN    DMA1_Channel4
  #define SPI_DMA_MUX     DMAMUX1_Channel3
#elif SPI_DMA_CHANNEL == 5
  #define SPI_DMA_CHAN    DMA1_Channel5
  #define SPI_DMA_MUX     DMAMUX1_Channel4
#else
  #error Wrong SPI_DMA_CHANNEL (1 - 5)!
#endif

#ifdef __cplusplus
};
#endif
