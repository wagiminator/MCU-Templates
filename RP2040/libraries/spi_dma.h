// ===================================================================================
// SPI Master Functions with DMA for RP2040 (SPI0/SPI1)                       * v1.1 *
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
// SPI_ready()              Check if SPI is ready to write (TX FIFO not full)
// SPI_available()          Check if something was received (RX FIFO not empty)
// SPI_enable()             Enable SPI module
// SPI_disable()            Disable SPI module
// SPI_setBAUD(n)           Set clock divider (SCK = F_CPU / (SPI_PRESC * (n + 1)), n = 0..255)
// SPI_setCPOL(n)           0: SCK low in idle, 1: SCK high in idle
// SPI_setCPHA(n)           Start sampling from 0: first clock edge, 1: second clock edge
//
// SPI pins (set below in SPI parameters):
// ---------------------------------------
// SPI0 SCK:  GP2, GP6,  GP18, GP22   MOSI (TX): GP3, GP7,  GP19, GP23
//      MISO (RX): GP0, GP4,  GP16, GP20
// SPI1 SCK:  GP10, GP14, GP26        MOSI (TX): GP11, GP15, GP27
//      MISO (RX): GP8, GP12, GP24, GP28
//
// Notes:
// ------
// - The SPI module (SPI0 or SPI1) is selected automatically by the SCK pin. All pins
//   must belong to the same SPI module. Set the MISO pin to -1 if it is not used.
// - SPI_writeBuffer() uses the DMA and returns immediately. The buffer must not be
//   changed until SPI_busy() returns 0. Use SPI_busy() before releasing the slave
//   select pin. Received data is ignored during DMA transfers.
// - The DMA channel can be set below (SPI_DMA_CH).
// - Slave select pins (NSS) must be defined and controlled by the application.
// - SPI clock rate must be defined below (max. F_CPU / 2). The clock rate is set to
//   the next possible value below or equal to the defined rate.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// SPI parameters
#define SPI_SCK_PIN       GP18            // SCK pin (see above)
#define SPI_MOSI_PIN      GP19            // MOSI pin (see above)
#define SPI_MISO_PIN      GP16            // MISO pin (see above, -1: not used)
#define SPI_CLKRATE       8000000         // SPI clock rate in Hz
#define SPI_CPOL          0               // 0: SCK low in idle, 1: high in idle
#define SPI_CPHA          0               // 0: sample on first edge, 1: second edge
#define SPI_DMA_CH        0               // DMA channel (0..11)

// SPI instance (selected by SCK pin, evaluated by the compiler)
#define SPI_NUM           ((SPI_SCK_PIN >> 3) & 1)
#define SPI_DEV           (SPI_NUM ? SPI1 : SPI0)
#define SPI_RESET         (SPI_NUM ? RESETS_RESET_SPI1 : RESETS_RESET_SPI0)
#define SPI_IRQn          (SPI_NUM ? SPI1_IRQ_IRQn : SPI0_IRQ_IRQn)
#define SPI_DREQ_TX       (SPI_NUM ? DREQ_SPI1_TX : DREQ_SPI0_TX)
#define SPI_DREQ_RX       (SPI_NUM ? DREQ_SPI1_RX : DREQ_SPI0_RX)

_Static_assert(((SPI_SCK_PIN & 3) == 2) && ((SPI_MOSI_PIN & 3) == 3) &&
               ((SPI_MISO_PIN < 0) || ((SPI_MISO_PIN & 3) == 0)), "Wrong SPI pins");
_Static_assert((((SPI_MOSI_PIN >> 3) & 1) == SPI_NUM) &&
               ((SPI_MISO_PIN < 0) || (((SPI_MISO_PIN >> 3) & 1) == SPI_NUM)),
               "SPI pins must belong to the same SPI module");

// SPI clock prescaler (even, 2..254) and divider (SCK = F_CPU / (PRESC * (DIV + 1)))
#define SPI_PRESC         ((((F_PERI / SPI_CLKRATE) + 511) / 512) * 2 < 2 ? 2 :      \
                           (((F_PERI / SPI_CLKRATE) + 511) / 512) * 2)
#define SPI_DIV           ((F_PERI + SPI_PRESC * SPI_CLKRATE - 1) / (SPI_PRESC * SPI_CLKRATE) - 1)

// SPI macros
#define SPI_busy()        (DMA_busy(SPI_DMA_CH) || (SPI_DEV->SSPSR & SPI_SSPSR_BSY))
#define SPI_ready()       (SPI_DEV->SSPSR & SPI_SSPSR_TNF)
#define SPI_available()   (SPI_DEV->SSPSR & SPI_SSPSR_RNE)
#define SPI_enable()      REG_SET(SPI_DEV->SSPCR1, SPI_SSPCR1_SSE)
#define SPI_disable()     REG_CLR(SPI_DEV->SSPCR1, SPI_SSPCR1_SSE)
#define SPI_setBAUD(n)    REG_WRITE_MASK(SPI_DEV->SSPCR0, (uint32_t)(n) << SPI_SSPCR0_SCR_Pos, \
                                         SPI_SSPCR0_SCR_Msk)
#define SPI_setCPOL(n)    ( (n) ? REG_SET(SPI_DEV->SSPCR0, SPI_SSPCR0_SPO)            \
                                : REG_CLR(SPI_DEV->SSPCR0, SPI_SSPCR0_SPO) )
#define SPI_setCPHA(n)    ( (n) ? REG_SET(SPI_DEV->SSPCR0, SPI_SSPCR0_SPH)            \
                                : REG_CLR(SPI_DEV->SSPCR0, SPI_SSPCR0_SPH) )

// SPI functions
void SPI_init(void);                      // init SPI with defined clock rate
uint8_t SPI_transfer(uint8_t data);       // transmit and receive one data byte
void SPI_write(uint8_t data);             // transmit one data byte
#define SPI_read()          SPI_transfer(0xff)     // receive one data byte
void SPI_writeBuffer(const uint8_t* buf, uint16_t len); // transmit buffer using DMA
void SPI_readBuffer(uint8_t* buf, uint16_t len);        // receive buffer

#ifdef __cplusplus
};
#endif
