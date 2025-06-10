// ===================================================================================
// Basic SPI Master Functions for tinyAVR 0-Series and 1-Series               * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// SPI_init()               Init SPI with defined clock rate (see below)
// SPI_transfer(d)          Transmit and receive one data byte
//
// SPI_ready()              Check if SPI is ready for transfer
// SPI_enable()             Enable SPI module
// SPI_disable()            Disable SPI module
// SPI_MSB_first()          The MSB of the data byte is transmitted firstly
// SPI_LSB_first()          The MSB of the data byte is transmitted firstly
// SPI_setPRESC(n)          Set SPI clock prescaler (see below)
// SPI_setMODE(n)           Set phase and polarity (refer to datasheet)
//
// SPI0 pin mapping (set below in SPI parameters):
// -----------------------------------------------
// SPI_MAP     0      1      2
// MOSI-pin   PA1    PC2    No mapping
// MISO-pin   PA2    PC1    No mapping
// SCK-pin    PA3    PC0    No mapping
//
// Notes:
// ------
// Slave select pins (NSS) must be defined and controlled by the application.
// SPI clock rate must be defined below.
//
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "system.h"

// ===================================================================================
// SPI Parameters
// ===================================================================================
#define SPI_MAP           0             // SPI port pin map (0 - 1)
#define SPI_PRESC         0             // SPI clock divider (0:1, 1:4, 2:16, 3:32)

// ===================================================================================
// SPI Functions and Macros
// ===================================================================================
#define SPI_ready()       (SPI0.INTFLAGS & SPI_IF_bm)

#define SPI_enable()      SPI0.CTRLA |=  SPI_ENABLE_bm
#define SPI_disable()     SPI0.CTRLA &= ~SPI_ENABLE_bm
#define SPI_LSB_first()   SPI0.CTRLA |=  SPI_DORD_bm
#define SPI_MSB_first()   SPI0.CTRLA &= ~SPI_DORD_bm
#define SPI_setPRESC(n)   SPI0.CTRLA = (SPI0.CTRLA & ~SPI_PRESC_gm) | (((n) & 3) << SPI_PRESC_gp)
#define SPI_setMODE(n)    SPI0.CTRLB = (SPI0.CTRLB & ~SPI_MODE_gm)  | (((n) & 3) << SPI_MODE_gp)

void SPI_init(void);
uint8_t SPI_transfer(uint8_t data);

#ifdef __cplusplus
};
#endif
