// ===================================================================================
// SPI Master Functions with DMA (SERCOM SPI, DMAC) for SAMD11                * v1.1 *
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
// SPI_setBAUD(n)           Set BAUD register (SCK = F_CPU / (2 * (n + 1)), n = 0..255)
// SPI_setCPOL(n)           0: SCK low in idle, 1: SCK high in idle
// SPI_setCPHA(n)           Start sampling from 0: first clock edge, 1: second clock edge
//
// SPI pin mapping (set below in SPI parameters):
// ----------------------------------------------
// SPI_MAP    0     1     2     3     4     5
// SERCOM     0     0     1     0     1     2
// SCK-pin   PA5   PA9   PA9   PA7   PA17  PA11  no mapping
// MISO-pin  PA14  PA5   PA31  PA5   PA22  PA14  no mapping
// MOSI-pin  PA4   PA8   PA8   PA6   PA16  PA10  no mapping
//
// Notes:
// ------
// - Mappings 3 to 5 are only available on SAMD11D, SERCOM2 only exists on SAMD11D.
// - PA31 is SWDIO.
// - SPI_writeBuffer() uses the DMA and returns immediately. The buffer must not be
//   changed until SPI_busy() returns 0. Use SPI_busy() before releasing the slave
//   select pin. Received data is ignored during DMA transfers.
// - Slave select pins (NSS) must be defined and controlled by the application.
// - SPI clock rate must be defined below (max. F_CPU / 2, 12MHz recommended).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// SPI Parameters
#define SPI_CLKRATE         4000000     // SPI clock rate in Hz (SCK = F_CPU / (2 * n))
#define SPI_MAP             0           // SPI pin mapping (see above)
#define SPI_DMA_CHANNEL     0           // DMA channel (0..5)

// SPI pin mapping (SERCOM, pins, peripheral function, MISO pad)
#if   SPI_MAP == 0
  #define SPI_SERCOM         0
  #define SPI_SCK_PIN        5
  #define SPI_MISO_PIN       14
  #define SPI_MOSI_PIN       4
  #define SPI_PMUX           2           // C
  #define SPI_DIPO           0           // MISO on PAD0
#elif   SPI_MAP == 1
  #define SPI_SERCOM         0
  #define SPI_SCK_PIN        9
  #define SPI_MISO_PIN       5
  #define SPI_MOSI_PIN       8
  #define SPI_PMUX           3           // D
  #define SPI_DIPO           1           // MISO on PAD1
#elif   SPI_MAP == 2
  #define SPI_SERCOM         1
  #define SPI_SCK_PIN        9
  #define SPI_MISO_PIN       31
  #define SPI_MOSI_PIN       8
  #define SPI_PMUX           2           // C
  #define SPI_DIPO           1           // MISO on PAD1
#elif   SPI_MAP == 3
  #define SPI_SERCOM         0
  #define SPI_SCK_PIN        7
  #define SPI_MISO_PIN       5
  #define SPI_MOSI_PIN       6
  #define SPI_PMUX           3           // D
  #define SPI_DIPO           1           // MISO on PAD1
#elif   SPI_MAP == 4
  #define SPI_SERCOM         1
  #define SPI_SCK_PIN        17
  #define SPI_MISO_PIN       22
  #define SPI_MOSI_PIN       16
  #define SPI_PMUX           2           // C
  #define SPI_DIPO           0           // MISO on PAD0
#elif   SPI_MAP == 5
  #define SPI_SERCOM         2
  #define SPI_SCK_PIN        11
  #define SPI_MISO_PIN       14
  #define SPI_MOSI_PIN       10
  #define SPI_PMUX           3           // D
  #define SPI_DIPO           0           // MISO on PAD0
#else
  #define SPI_SERCOM         0           // pins must be set by the application
  #define SPI_DIPO           0
#endif

#if defined(SAMD11C) && (SPI_MAP >= 3) && (SPI_MAP <= 5)
  #error Pin mapping not available on SAMD11C
#endif

#if (SPI_SERCOM == 2) && defined(SAMD11C)
  #error SERCOM2 is not available on SAMD11C
#endif

#if   SPI_SERCOM == 0
  #define SPI_SER            SERCOM0
  #define SPI_GCLK_ID        SERCOM0_GCLK_ID_CORE
  #define SPI_APBMASK        PM_APBCMASK_SERCOM0
  #define SPI_DMAC_ID_TX     SERCOM0_DMAC_ID_TX
#elif SPI_SERCOM == 1
  #define SPI_SER            SERCOM1
  #define SPI_GCLK_ID        SERCOM1_GCLK_ID_CORE
  #define SPI_APBMASK        PM_APBCMASK_SERCOM1
  #define SPI_DMAC_ID_TX     SERCOM1_DMAC_ID_TX
#else
  #define SPI_SER            SERCOM2
  #define SPI_GCLK_ID        SERCOM2_GCLK_ID_CORE
  #define SPI_APBMASK        PM_APBCMASK_SERCOM2
  #define SPI_DMAC_ID_TX     SERCOM2_DMAC_ID_TX
#endif

// SPI BAUD register value: SCK = F_CPU / (2 * (BAUD + 1))
#define SPI_BAUD_VAL       ((F_CPU + 2 * SPI_CLKRATE - 1) / (2 * SPI_CLKRATE) - 1)
#if SPI_BAUD_VAL > 255
  #error SPI_CLKRATE too low
#endif
// SPI Functions and Macros
uint8_t SPI_busy(void);
#define SPI_ready()         (SPI_SER->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_DRE)
#define SPI_available()     (SPI_SER->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_RXC)

#define SPI_sync()          while(SPI_SER->SPI.SYNCBUSY.reg)
#define SPI_enable()        SPI_setCTRLA(SERCOM_SPI_CTRLA_ENABLE, SERCOM_SPI_CTRLA_ENABLE)
#define SPI_disable()       SPI_setCTRLA(SERCOM_SPI_CTRLA_ENABLE, 0)

// BAUD, CPOL and CPHA should only be changed while no communication is ongoing
#define SPI_setCPOL(n)      SPI_setCTRLA(SERCOM_SPI_CTRLA_CPOL, (n) ? SERCOM_SPI_CTRLA_CPOL : 0)
#define SPI_setCPHA(n)      SPI_setCTRLA(SERCOM_SPI_CTRLA_CPHA, (n) ? SERCOM_SPI_CTRLA_CPHA : 0)
void SPI_setBAUD(uint8_t n);
void SPI_setCTRLA(uint32_t mask, uint32_t val);

void SPI_init(void);
uint8_t SPI_transfer(uint8_t data);
void SPI_write(uint8_t data);
#define SPI_read()          SPI_transfer(0xff)
void SPI_writeBuffer(const uint8_t* buf, uint16_t len);
void SPI_readBuffer(uint8_t* buf, uint16_t len);

#ifdef __cplusplus
};
#endif
