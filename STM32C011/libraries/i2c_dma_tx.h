// ===================================================================================
// Basic I2C Master Functions for STM32C0xx (DMA, TX only)                    * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// I2C_init()               Init I2C with defined clock rate (see below)
// I2C_start(addr)          I2C start transmission, addr must contain R/W bit
//                          (call again without stop for a repeated start)
// I2C_write(b)             I2C transmit one data byte via I2C
// I2C_stop()               I2C stop transmission
// I2C_busy()               Check if I2C bus is busy (DMA transfer ongoing)
//
// I2C_sendBuffer(addr,buf,len) Send buffer (*buf) with length (len) to device (addr)
// I2C_writeBuffer(buf,len)     Write buffer (*buf) with length (len) via I2C and stop (DMA)
//
// I2C_writeBuffer() uses DMA and returns immediately; the buffer must not be changed
// until I2C_busy() returns 0. The transfer is finished by the I2C interrupt.
//
// I2C pin mapping (set below in I2C parameters):
// ----------------------------------------------
// I2C_MAP   0     1     2     3     4     5
// SCL-pin  PA9   PA11  PB6   PB8   PB7   No mapping
// SDA-pin  PA10  PA12  PB7   PB9   PC14  No mapping
// (PA11/PA12: PA9/PA10 are remapped to these pins by SYSCFG,
//  PB8/PB9 are only available on STM32C031)
//
// I2C clock rates up to 100kHz use Standard-mode, up to 400kHz Fast-mode and above
// Fast-mode Plus (requires F_CPU >= 17MHz). Fast-mode requires F_CPU >= 4MHz.
// The I2C kernel clock is PCLK (= F_CPU).
//
// External pull-up resistors (4k7 - 10k) are mandatory!
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// I2C parameters
#define I2C_CLKRATE       400000  // I2C bus clock rate (Hz)
#define I2C_MAP           2       // I2C pin mapping (see above)
#define I2C_DMA_CHANNEL   1       // DMA channel (1 - 3)

// Interrupt enable check
#if SYS_USE_VECTORS == 0
  #error Interrupt vector table must be enabled (SYS_USE_VECTORS in system.h)!
#endif

// I2C functions
void I2C_init(void);              // I2C init function
void I2C_start(uint8_t addr);     // I2C start transmission, addr must contain R/W bit
void I2C_stop(void);              // I2C stop transmission
void I2C_write(uint8_t data);     // I2C transmit one data byte via I2C

void I2C_writeBuffer(uint8_t* buf, uint16_t len);

#define I2C_sendBuffer(addr,buf,len)  (I2C_start(addr), I2C_writeBuffer(buf,len))

extern volatile uint8_t I2C_DMA_busy;  // DMA transfer ongoing flag
#define I2C_busy()        (I2C_DMA_busy || (I2C1->ISR & I2C_ISR_BUSY))

// DMA channel defines
#if   I2C_DMA_CHANNEL == 1
  #define I2C_DMA_CHAN    DMA1_Channel1
  #define I2C_DMA_MUX     DMAMUX1_Channel0
#elif I2C_DMA_CHANNEL == 2
  #define I2C_DMA_CHAN    DMA1_Channel2
  #define I2C_DMA_MUX     DMAMUX1_Channel1
#elif I2C_DMA_CHANNEL == 3
  #define I2C_DMA_CHAN    DMA1_Channel3
  #define I2C_DMA_MUX     DMAMUX1_Channel2
#else
  #error Wrong I2C_DMA_CHANNEL (1 - 3)!
#endif

#ifdef __cplusplus
};
#endif
