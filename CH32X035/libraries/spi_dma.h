// ===================================================================================
// SPI Master Functions with DMA for CH32X035/X034/X033                       * v1.0 *
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

// SPI pin mapping:
// ----------------
// SCK-pin   PA5
// MISO-pin  PA6
// MOSI-pin  PA7
//
// Slave select pins (NSS) must be defined and controlled by the application.
// SPI clock rate must be defined below.
//
// SPI_writeBuffer() uses DMA1 channel 3 and returns immediately. The buffer must not
// be changed until SPI_busy() returns 0. Use SPI_busy() before releasing the slave
// select pin. Received data is ignored during DMA transfers.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// SPI Parameters
#define SPI_PRESC           6     // SPI_CLKRATE = F_CPU / (2 << SPI_PRESC)

// SPI Functions and Macros
#define SPI_busy()          (SPI_DMA_CHAN->CNTR || !(SPI1->STATR & SPI_STATR_TXE) || (SPI1->STATR & SPI_STATR_BSY))
#define SPI_ready()         (SPI1->STATR & SPI_STATR_TXE)
#define SPI_available()     (SPI1->STATR & SPI_STATR_RXNE)

#define SPI_enable()        SPI1->CTLR1 |=  SPI_CTLR1_SPE
#define SPI_disable()       SPI1->CTLR1 &= ~SPI_CTLR1_SPE
#define SPI_setCPOL(n)      (n)?(SPI1->CTLR1|=SPI_CTLR1_CPOL):(SPI1->CTLR1&=~SPI_CTLR1_CPOL)
#define SPI_setCPHA(n)      (n)?(SPI1->CTLR1|=SPI_CTLR1_CPHA):(SPI1->CTLR1&=~SPI_CTLR1_CPHA)
#define SPI_setBAUD(n)      SPI1->CTLR1 = (SPI1->CTLR1&~SPI_CTLR1_BR) | (((n)&7)<<3)

void SPI_init(void);
uint8_t SPI_transfer(uint8_t data);
#define SPI_write(d)        (void)SPI_transfer(d)  // transmit one data byte
#define SPI_read()          SPI_transfer(0xff)     // receive one data byte
void SPI_writeBuffer(const uint8_t* buf, uint16_t len);
void SPI_readBuffer(uint8_t* buf, uint16_t len);

// DMA channel define (SPI1_TX is fixed to DMA1 channel 3)
#define SPI_DMA_CHAN        DMA1_Channel3

#ifdef __cplusplus
};
#endif
