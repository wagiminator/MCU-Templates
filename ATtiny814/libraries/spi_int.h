// ===================================================================================
// SPI Master Functions with Interrupt for tinyAVR 0-Series and 1-Series      * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// SPI_init()               Init SPI with defined clock rate (see below)
// SPI_transfer(d)          Transmit and receive one data byte
// SPI_write(d)             Transmit one data byte
// SPI_read()               Receive one data byte (sends 0xFF)
// SPI_writeBuffer(buf,len) Transmit buffer (*buf) with length (len) (interrupt)
// SPI_readBuffer(buf,len)  Receive buffer (*buf) with length (len), sends 0xFF
//
// SPI_busy()               Check if buffer transfer is ongoing
// SPI_ready()              Check if SPI is ready for transfer
// SPI_enable()             Enable SPI module
// SPI_disable()            Disable SPI module
// SPI_MSB_first()          The MSB of the data byte is transmitted firstly
// SPI_LSB_first()          The LSB of the data byte is transmitted firstly
// SPI_setPRESC(n)          Set SPI clock prescaler (0:2, 1:8, 2:32, 3:64)
// SPI_setMODE(n)           Set SPI mode 0..3 (phase and polarity)
//
// SPI0 pin mapping (set below in SPI parameters):
// -----------------------------------------------
// SPI_MAP     0      1      2
// MOSI-pin   PA1    PC2    No mapping
// MISO-pin   PA2    PC1    No mapping
// SCK-pin    PA3    PC0    No mapping
// SS-pin     PA4    PC3    (PA0 on 8-pin devices, not used by this library)
// Map 1 is only available on 20-pin devices.
//
// Notes:
// ------
// Slave select pins (NSS) must be defined and controlled by the application.
// SPI clock rate must be defined below.
//
// SPI_writeBuffer() is interrupt driven and returns immediately; the buffer must
// not be changed until SPI_busy() returns 0. Use SPI_busy() before releasing the
// slave select pin. Received data is ignored. All other functions wait until the
// transfer is finished. Global interrupts are enabled by SPI_init().
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "system.h"

// ===================================================================================
// SPI Parameters
// ===================================================================================
#define SPI_MAP           0             // SPI port pin map (see above)
#define SPI_PRESC         0             // SPI clock divider (0:2, 1:8, 2:32, 3:64)

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
#define SPI_write(d)        (void)SPI_transfer(d)  // transmit one data byte
#define SPI_read()          SPI_transfer(0xff)     // receive one data byte
void SPI_writeBuffer(const uint8_t* buf, uint16_t len);
extern volatile uint8_t SPI_INT_busy;   // buffer transfer ongoing flag
#define SPI_busy()        (SPI_INT_busy)
void SPI_readBuffer(uint8_t* buf, uint16_t len);

#ifdef __cplusplus
};
#endif
