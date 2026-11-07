// ===================================================================================
// Basic I2C Slave Functions (SERCOM I2CS) for SAMD11                         * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// I2C_init()               init I2C slave device with parameters set below
// I2C_busy()               check if I2C communication is currently on-going
// I2C_changed()            check if something was changed in the slave registers
// I2C_clear()              clear the changed flag
// I2C_REG[]                slave register array
//
// I2C pin mapping (set below in I2C parameters):
// ----------------------------------------------
// I2C_MAP   0     1     2     3     4
// SERCOM    0     1     2     2     1
// SDA-pin  PA14  PA22  PA14  PA22  PA30
// SCL-pin  PA15  PA23  PA15  PA23  PA31
//
// Notes:
// ------
// - Mappings 1 to 3 are only available on SAMD11D, SERCOM2 only exists on SAMD11D.
// - PA30/PA31 are the SWD pins (SWCLK/SWDIO).
// - The slave works with clock rates up to 400kHz (SERCOM clocked by GCLK0).
//
// External pull-up resistors (4k7 - 10k) are mandatory!
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// I2C Parameters
#ifndef I2C_MAP
  #define I2C_MAP       0           // I2C pin mapping (see above)
#endif
#ifndef I2C_ADDR
  #define I2C_ADDR      0x36        // I2C address of the device
#endif
#ifndef I2C_REG_SIZE
  #define I2C_REG_SIZE  10          // number of 8-bit registers
#endif

#define I2C_REG_SLCT    0           // 0: transmission will always start with register 0
                                    // 1: first byte written is register address (pointer),
                                    //    a following read continues at this address

// Interrupt enable check
#if SYS_USE_VECTORS == 0
  #error Interrupt vector table must be enabled (SYS_USE_VECTORS in system.h)!
#endif

// I2C pin mapping (SERCOM, SDA/SCL pin, peripheral function)
#if   I2C_MAP == 0
  #define I2C_SERCOM        0
  #define I2C_SDA_PIN       14
  #define I2C_SCL_PIN       15
  #define I2C_PMUX          2           // C
#elif I2C_MAP == 1
  #define I2C_SERCOM        1
  #define I2C_SDA_PIN       22
  #define I2C_SCL_PIN       23
  #define I2C_PMUX          2           // C
#elif I2C_MAP == 2
  #define I2C_SERCOM        2
  #define I2C_SDA_PIN       14
  #define I2C_SCL_PIN       15
  #define I2C_PMUX          3           // D
#elif I2C_MAP == 3
  #define I2C_SERCOM        2
  #define I2C_SDA_PIN       22
  #define I2C_SCL_PIN       23
  #define I2C_PMUX          3           // D
#elif I2C_MAP == 4
  #define I2C_SERCOM        1
  #define I2C_SDA_PIN       30
  #define I2C_SCL_PIN       31
  #define I2C_PMUX          2           // C
#else
  #define I2C_SERCOM        0           // pins must be set by the application
#endif

#if defined(SAMD11C) && (I2C_MAP >= 1) && (I2C_MAP <= 3)
  #error Pin mapping not available on SAMD11C
#endif

#if (I2C_SERCOM == 2) && defined(SAMD11C)
  #error SERCOM2 is not available on SAMD11C
#endif

#if   I2C_SERCOM == 0
  #define I2C_SER           SERCOM0
  #define I2C_GCLK_ID       SERCOM0_GCLK_ID_CORE
  #define I2C_APBMASK       PM_APBCMASK_SERCOM0
  #define I2C_IRQn          SERCOM0_IRQn
  #define I2C_IRQHandler    SERCOM0_IRQHandler
  #define I2C_DMAC_ID_TX    SERCOM0_DMAC_ID_TX
#elif I2C_SERCOM == 1
  #define I2C_SER           SERCOM1
  #define I2C_GCLK_ID       SERCOM1_GCLK_ID_CORE
  #define I2C_APBMASK       PM_APBCMASK_SERCOM1
  #define I2C_IRQn          SERCOM1_IRQn
  #define I2C_IRQHandler    SERCOM1_IRQHandler
  #define I2C_DMAC_ID_TX    SERCOM1_DMAC_ID_TX
#else
  #define I2C_SER           SERCOM2
  #define I2C_GCLK_ID       SERCOM2_GCLK_ID_CORE
  #define I2C_APBMASK       PM_APBCMASK_SERCOM2
  #define I2C_IRQn          SERCOM2_IRQn
  #define I2C_IRQHandler    SERCOM2_IRQHandler
  #define I2C_DMAC_ID_TX    SERCOM2_DMAC_ID_TX
#endif
// I2C Slave Registers
extern volatile uint8_t I2C_REG[];        // slave register array
extern volatile uint8_t I2C_FLAG_changed; // slave register change flag

// I2C Slave Functions and Macros
void I2C_init(void);                // I2C slave init function

#define I2C_busy()      (I2C_SER->I2CS.STATUS.reg & SERCOM_I2CS_STATUS_CLKHOLD)
#define I2C_changed()   (I2C_FLAG_changed)
#define I2C_clear()     I2C_FLAG_changed = 0

#ifdef __cplusplus
};
#endif
