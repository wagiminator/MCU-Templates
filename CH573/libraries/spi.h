// ===================================================================================
// Basic SPI Master Functions for CH573/CH571                                 * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// SPI_init()               Init SPI0 with defined clock rate (see below), mode 0
// SPI_transfer(d)          Transmit and receive one data byte
// SPI_write(d)             Transmit one data byte
// SPI_read()               Receive one data byte (sends 0xFF)
// SPI_writeBuffer(buf,len) Transmit buffer (via FIFO)
// SPI_readBuffer(buf,len)  Receive buffer (via FIFO)
//
// SPI_busy()               Check if SPI is busy
// SPI_setDIV(n)            Set clock divider (SCK = F_CPU / n, n = 2..255)
// SPI_setMode0()           Set SPI mode 0 (CPOL=0, CPHA=0) (*)
// SPI_setMode3()           Set SPI mode 3 (CPOL=1, CPHA=1)
// SPI_setMSBfirst()        Transmit MSB first (*)
// SPI_setLSBfirst()        Transmit LSB first
//
// SPI pin mapping (set below in SPI parameters):
// ----------------------------------------------
// SPI_REMAP   0     1
// SCK-pin    PA13  PB13
// MOSI-pin   PA14  PB14
// MISO-pin   PA15  PB15
//
// Slave select pins (CS) must be defined and controlled by the application.
// The SPI0 of the CH573 only supports modes 0 and 3 in master mode.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// SPI Parameters
#define SPI_REMAP           0           // pin mapping (see above)
#define SPI_CLKRATE         4000000     // SPI clock rate in Hz

// SPI Macros
#define SPI_busy()          (!(R8_SPI0_INT_FLAG & RB_SPI_FREE))
#define SPI_setDIV(n)       R8_SPI0_CLOCK_DIV = (n)
#define SPI_setMode0()      R8_SPI0_CTRL_MOD &= ~RB_SPI_MST_SCK_MOD
#define SPI_setMode3()      R8_SPI0_CTRL_MOD |=  RB_SPI_MST_SCK_MOD
#define SPI_setMSBfirst()   R8_SPI0_CTRL_CFG &= ~RB_SPI_BIT_ORDER
#define SPI_setLSBfirst()   R8_SPI0_CTRL_CFG |=  RB_SPI_BIT_ORDER
#define SPI_write(d)        (void)SPI_transfer(d)
#define SPI_read()          SPI_transfer(0xFF)

// SPI Functions
void    SPI_init(void);
uint8_t SPI_transfer(uint8_t data);
void    SPI_writeBuffer(const uint8_t* buf, uint16_t len);
void    SPI_readBuffer(uint8_t* buf, uint16_t len);

#ifdef __cplusplus
};
#endif
