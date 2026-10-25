// ===================================================================================
// Basic SPI Master TX Functions for CH32V002, V004, V005, V006, V007, M007   * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// SPI_init()               Init SPI with defined clock rate (see below)
// SPI_write(d)             Transmit one data byte
//
// SPI_busy()               Check if SPI bus is busy
// SPI_ready()              Check if SPI is ready to write
// SPI_enable()             Enable SPI module
// SPI_disable()            Disable SPI module
// SPI_setBAUD(n)           Set BAUD rate (see below)
// SPI_setCPOL(n)           0: SCK low in idle, 1: SCK high in idle
// SPI_setCPHA(n)           Start sampling from 0: first clock edge, 1: second clock edge
//
// SPI pin mapping (set below in SPI parameters, value = AFIO_PCFR1 SPI1_RM):
// --------------------------------------------------------------------------
// SPI_MAP    0     1     2     3     4     5     6
// SCK-pin   PC5   PC5   PD2   PB1   PD4   PA1   PB5
// MOSI-pin  PC6   PC6   PD3   PC0   PD6   PA2   PB4
// NSS-pin   PC1   PC0   PC4   PB0   PD3   PC1   PC4   (not used, software controlled)
//
// Notes:
// ------
// - Slave select pins (NSS) must be defined and controlled by the application.
// - Not all pins are available on every package/chip variant.
// - PD1 is also the SWIO (programming/debug) pin, PD7 is the reset pin (if enabled).
// - SPI_CLKRATE = F_CPU / (2 << SPI_PRESC); the maximum is F_CPU / 2 (SPI_PRESC = 0).
// - Change CPOL, CPHA and BAUD only while the SPI module is disabled (SPI_disable()).
// - MISO is not used in this TX-only version and stays a free GPIO.
//
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// SPI Parameters
#define SPI_MAP             0     // SPI pin mapping (see above)
#define SPI_PRESC           5     // SPI_CLKRATE = F_CPU / (2 << SPI_PRESC)

// SPI Functions and Macros
#define SPI_busy()          (SPI1->STATR & SPI_STATR_BSY)
#define SPI_ready()         (SPI1->STATR & SPI_STATR_TXE)

#define SPI_enable()        SPI1->CTLR1 |=  SPI_CTLR1_SPE
#define SPI_disable()       SPI1->CTLR1 &= ~SPI_CTLR1_SPE
#define SPI_setCPOL(n)      ((n)?(SPI1->CTLR1|=SPI_CTLR1_CPOL):(SPI1->CTLR1&=~SPI_CTLR1_CPOL))
#define SPI_setCPHA(n)      ((n)?(SPI1->CTLR1|=SPI_CTLR1_CPHA):(SPI1->CTLR1&=~SPI_CTLR1_CPHA))
#define SPI_setBAUD(n)      SPI1->CTLR1 = (SPI1->CTLR1&~SPI_CTLR1_BR) | (((n)&7)<<3)

void SPI_init(void);
void SPI_write(uint8_t data);

#ifdef __cplusplus
};
#endif
