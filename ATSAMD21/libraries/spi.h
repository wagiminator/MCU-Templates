// ===================================================================================
// Basic SPI Master Functions (SERCOM SPI) for SAMD21                         * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// SPI_init()               Init SPI with defined clock rate (see below)
// SPI_transfer(d)          Transmit and receive one data byte
// SPI_write(d)             Transmit one data byte
// SPI_read()               Receive one data byte (sends 0xFF)
// SPI_writeBuffer(buf,len) Transmit buffer (*buf) with length (len)
// SPI_readBuffer(buf,len)  Receive buffer (*buf) with length (len), sends 0xFF
//
// SPI_busy()               Check if SPI is busy (transmit buffer not empty)
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
// SPI_MAP   0     1     2     3     4     5     6     
// SERCOM    0     1     2     3     4     5     1     
// SCK-pin   PA7   PA17  PA15  PA17  PB11  PB23  PA19  No mapping
// MISO-pin  PA4   PA19  PA8   PA19  PA12  PB2   PA16  No mapping
// MOSI-pin  PA6   PA16  PA14  PA16  PB10  PB22  PA18  No mapping
// Device    EGJ   EGJ   EGJ   EGJ   GJ    GJ    EGJ   
//
// Notes:
// ------
// - Device row: mapping available on SAMD21E (E), SAMD21G (G) and SAMD21J (J).
// - Mapping 4 corresponds to the SPI header of the Arduino Zero.
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
#define SPI_MAP             2           // SPI pin mapping (see above)

// SPI pin mapping (SERCOM, pins, peripheral functions, pads)
#if   SPI_MAP == 0
  #define SPI_SERCOM        0
  #define SPI_SCK_PIN       7           // PA7
  #define SPI_SCK_PMUX      3           // D
  #define SPI_MISO_PIN      4           // PA4
  #define SPI_MISO_PMUX     3           // D
  #define SPI_MOSI_PIN      6           // PA6
  #define SPI_MOSI_PMUX     3           // D
  #define SPI_DOPO          1           // data out pinout
  #define SPI_DIPO          0           // MISO on PAD0
#elif SPI_MAP == 1
  #define SPI_SERCOM        1
  #define SPI_SCK_PIN       17          // PA17
  #define SPI_SCK_PMUX      2           // C
  #define SPI_MISO_PIN      19          // PA19
  #define SPI_MISO_PMUX     2           // C
  #define SPI_MOSI_PIN      16          // PA16
  #define SPI_MOSI_PMUX     2           // C
  #define SPI_DOPO          0           // data out pinout
  #define SPI_DIPO          3           // MISO on PAD3
#elif SPI_MAP == 2
  #define SPI_SERCOM        2
  #define SPI_SCK_PIN       15          // PA15
  #define SPI_SCK_PMUX      2           // C
  #define SPI_MISO_PIN      8           // PA8
  #define SPI_MISO_PMUX     3           // D
  #define SPI_MOSI_PIN      14          // PA14
  #define SPI_MOSI_PMUX     2           // C
  #define SPI_DOPO          1           // data out pinout
  #define SPI_DIPO          0           // MISO on PAD0
#elif SPI_MAP == 3
  #define SPI_SERCOM        3
  #define SPI_SCK_PIN       17          // PA17
  #define SPI_SCK_PMUX      3           // D
  #define SPI_MISO_PIN      19          // PA19
  #define SPI_MISO_PMUX     3           // D
  #define SPI_MOSI_PIN      16          // PA16
  #define SPI_MOSI_PMUX     3           // D
  #define SPI_DOPO          0           // data out pinout
  #define SPI_DIPO          3           // MISO on PAD3
#elif SPI_MAP == 4
  #define SPI_SERCOM        4
  #define SPI_SCK_PIN       43          // PB11
  #define SPI_SCK_PMUX      3           // D
  #define SPI_MISO_PIN      12          // PA12
  #define SPI_MISO_PMUX     3           // D
  #define SPI_MOSI_PIN      42          // PB10
  #define SPI_MOSI_PMUX     3           // D
  #define SPI_DOPO          1           // data out pinout
  #define SPI_DIPO          0           // MISO on PAD0
#elif SPI_MAP == 5
  #define SPI_SERCOM        5
  #define SPI_SCK_PIN       55          // PB23
  #define SPI_SCK_PMUX      3           // D
  #define SPI_MISO_PIN      34          // PB2
  #define SPI_MISO_PMUX     3           // D
  #define SPI_MOSI_PIN      54          // PB22
  #define SPI_MOSI_PMUX     3           // D
  #define SPI_DOPO          1           // data out pinout
  #define SPI_DIPO          0           // MISO on PAD0
#elif SPI_MAP == 6
  #define SPI_SERCOM        1
  #define SPI_SCK_PIN       19          // PA19
  #define SPI_SCK_PMUX      2           // C
  #define SPI_MISO_PIN      16          // PA16
  #define SPI_MISO_PMUX     2           // C
  #define SPI_MOSI_PIN      18          // PA18
  #define SPI_MOSI_PMUX     2           // C
  #define SPI_DOPO          1           // data out pinout
  #define SPI_DIPO          0           // MISO on PAD0
#else
  #define SPI_SERCOM        0           // pins must be set by the application
  #define SPI_DOPO          1
  #define SPI_DIPO          0
#endif

#if (SPI_MAP == 4) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (SPI_MAP == 5) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif

#if (SPI_SERCOM > 3) && defined(SAMD21E)
  #error SERCOM4 and SERCOM5 are not available on SAMD21E
#endif

#if   SPI_SERCOM == 0
  #define SPI_SER            SERCOM0
  #define SPI_GCLK_ID        SERCOM0_GCLK_ID_CORE
  #define SPI_APBMASK        PM_APBCMASK_SERCOM0
  #define SPI_IRQn           SERCOM0_IRQn
  #define SPI_IRQHandler     SERCOM0_IRQHandler
  #define SPI_DMAC_ID_TX     SERCOM0_DMAC_ID_TX
#elif SPI_SERCOM == 1
  #define SPI_SER            SERCOM1
  #define SPI_GCLK_ID        SERCOM1_GCLK_ID_CORE
  #define SPI_APBMASK        PM_APBCMASK_SERCOM1
  #define SPI_IRQn           SERCOM1_IRQn
  #define SPI_IRQHandler     SERCOM1_IRQHandler
  #define SPI_DMAC_ID_TX     SERCOM1_DMAC_ID_TX
#elif SPI_SERCOM == 2
  #define SPI_SER            SERCOM2
  #define SPI_GCLK_ID        SERCOM2_GCLK_ID_CORE
  #define SPI_APBMASK        PM_APBCMASK_SERCOM2
  #define SPI_IRQn           SERCOM2_IRQn
  #define SPI_IRQHandler     SERCOM2_IRQHandler
  #define SPI_DMAC_ID_TX     SERCOM2_DMAC_ID_TX
#elif SPI_SERCOM == 3
  #define SPI_SER            SERCOM3
  #define SPI_GCLK_ID        SERCOM3_GCLK_ID_CORE
  #define SPI_APBMASK        PM_APBCMASK_SERCOM3
  #define SPI_IRQn           SERCOM3_IRQn
  #define SPI_IRQHandler     SERCOM3_IRQHandler
  #define SPI_DMAC_ID_TX     SERCOM3_DMAC_ID_TX
#elif SPI_SERCOM == 4
  #define SPI_SER            SERCOM4
  #define SPI_GCLK_ID        SERCOM4_GCLK_ID_CORE
  #define SPI_APBMASK        PM_APBCMASK_SERCOM4
  #define SPI_IRQn           SERCOM4_IRQn
  #define SPI_IRQHandler     SERCOM4_IRQHandler
  #define SPI_DMAC_ID_TX     SERCOM4_DMAC_ID_TX
#elif SPI_SERCOM == 5
  #define SPI_SER            SERCOM5
  #define SPI_GCLK_ID        SERCOM5_GCLK_ID_CORE
  #define SPI_APBMASK        PM_APBCMASK_SERCOM5
  #define SPI_IRQn           SERCOM5_IRQn
  #define SPI_IRQHandler     SERCOM5_IRQHandler
  #define SPI_DMAC_ID_TX     SERCOM5_DMAC_ID_TX
#endif

// SPI BAUD register value: SCK = F_CPU / (2 * (BAUD + 1))
#define SPI_BAUD_VAL       ((F_CPU + 2 * SPI_CLKRATE - 1) / (2 * SPI_CLKRATE) - 1)
#if SPI_BAUD_VAL > 255
  #error SPI_CLKRATE too low
#endif

// SPI Functions and Macros
#define SPI_busy()          (!(SPI_SER->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_DRE))
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
#define SPI_write(d)        (void)SPI_transfer(d)  // transmit one data byte
#define SPI_read()          SPI_transfer(0xff)     // receive one data byte
void SPI_writeBuffer(const uint8_t* buf, uint16_t len);
void SPI_readBuffer(uint8_t* buf, uint16_t len);

#ifdef __cplusplus
};
#endif
