// ===================================================================================
// Basic I2C Master Functions for STM32L0xx (DMA, TX only)                    * v1.0 *
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
// I2C_MAP   0     1     2     3
// SCL-pin  PA9   PB6   PA4   No mapping
// SDA-pin  PA10  PB7   PA13  No mapping
// (PB6/PB7 are not available on TSSOP20 packages, PA13 is also SWDIO - map 2
//  disables the debug interface!)
//
// I2C1 TX DMA channel (RM0451, Table 45): 2 (all devices), 4 (category 1 only,
// STM32L010x4) or 6 (category 2, 3 and 5 only, STM32L010x6/x8/xB).
//
// I2C clock rates up to 100kHz use Standard-mode and up to 400kHz Fast-mode (requires
// F_CPU >= 4MHz). The STM32L010 does not support Fast-mode Plus (RM0451, Table 2).
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
#define I2C_MAP           0       // I2C pin mapping (see above)
#define I2C_DMA_CHANNEL   2       // DMA channel (2, 4 or 6, see above)

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
#if   I2C_DMA_CHANNEL == 2
  #define I2C_DMA_CHAN    DMA1_Channel2
  #define I2C_DMA_CSEL    4                     // DMA1_CSELR C2S position
#elif I2C_DMA_CHANNEL == 4
  #define I2C_DMA_CHAN    DMA1_Channel4
  #define I2C_DMA_CSEL    12                    // DMA1_CSELR C4S position
#elif I2C_DMA_CHANNEL == 6
  #define I2C_DMA_CHAN    DMA1_Channel6
  #define I2C_DMA_CSEL    20                    // DMA1_CSELR C6S position
#else
  #error Wrong I2C_DMA_CHANNEL!
#endif

#ifdef __cplusplus
};
#endif
