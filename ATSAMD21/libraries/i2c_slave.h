// ===================================================================================
// Basic I2C Slave Functions (SERCOM I2CS) for SAMD21                         * v1.0 *
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
// I2C_MAP   0     1     2     3     4     5     6     7     8     9     10    
// SERCOM    3     1     0     2     3     5     2     4     4     5     5     
// SDA-pin   PA22  PA16  PA8   PA8   PA16  PA22  PA12  PA12  PB12  PB16  PB30  No mapping
// SCL-pin   PA23  PA17  PA9   PA9   PA17  PA23  PA13  PA13  PB13  PB17  PB31  No mapping
// Device    EGJ   EGJ   EGJ   EGJ   EGJ   GJ    GJ    GJ    J     J     J     
//
// Notes:
// ------
// - Device row: mapping available on SAMD21E (E), SAMD21G (G) and SAMD21J (J).
// - Only pins with I2C capability are used (PA8/9, PA12/13, PA16/17, PA22/23, PB12/13,
//   PB16/17, PB30/31). Mapping 0 corresponds to the Arduino Zero (Wire).
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
  #define I2C_SERCOM        3
  #define I2C_SDA_PIN       22          // PA22
  #define I2C_SCL_PIN       23          // PA23
  #define I2C_SDA_PMUX      2           // C
  #define I2C_SCL_PMUX      2           // C
#elif I2C_MAP == 1
  #define I2C_SERCOM        1
  #define I2C_SDA_PIN       16          // PA16
  #define I2C_SCL_PIN       17          // PA17
  #define I2C_SDA_PMUX      2           // C
  #define I2C_SCL_PMUX      2           // C
#elif I2C_MAP == 2
  #define I2C_SERCOM        0
  #define I2C_SDA_PIN       8           // PA8
  #define I2C_SCL_PIN       9           // PA9
  #define I2C_SDA_PMUX      2           // C
  #define I2C_SCL_PMUX      2           // C
#elif I2C_MAP == 3
  #define I2C_SERCOM        2
  #define I2C_SDA_PIN       8           // PA8
  #define I2C_SCL_PIN       9           // PA9
  #define I2C_SDA_PMUX      3           // D
  #define I2C_SCL_PMUX      3           // D
#elif I2C_MAP == 4
  #define I2C_SERCOM        3
  #define I2C_SDA_PIN       16          // PA16
  #define I2C_SCL_PIN       17          // PA17
  #define I2C_SDA_PMUX      3           // D
  #define I2C_SCL_PMUX      3           // D
#elif I2C_MAP == 5
  #define I2C_SERCOM        5
  #define I2C_SDA_PIN       22          // PA22
  #define I2C_SCL_PIN       23          // PA23
  #define I2C_SDA_PMUX      3           // D
  #define I2C_SCL_PMUX      3           // D
#elif I2C_MAP == 6
  #define I2C_SERCOM        2
  #define I2C_SDA_PIN       12          // PA12
  #define I2C_SCL_PIN       13          // PA13
  #define I2C_SDA_PMUX      2           // C
  #define I2C_SCL_PMUX      2           // C
#elif I2C_MAP == 7
  #define I2C_SERCOM        4
  #define I2C_SDA_PIN       12          // PA12
  #define I2C_SCL_PIN       13          // PA13
  #define I2C_SDA_PMUX      3           // D
  #define I2C_SCL_PMUX      3           // D
#elif I2C_MAP == 8
  #define I2C_SERCOM        4
  #define I2C_SDA_PIN       44          // PB12
  #define I2C_SCL_PIN       45          // PB13
  #define I2C_SDA_PMUX      2           // C
  #define I2C_SCL_PMUX      2           // C
#elif I2C_MAP == 9
  #define I2C_SERCOM        5
  #define I2C_SDA_PIN       48          // PB16
  #define I2C_SCL_PIN       49          // PB17
  #define I2C_SDA_PMUX      2           // C
  #define I2C_SCL_PMUX      2           // C
#elif I2C_MAP == 10
  #define I2C_SERCOM        5
  #define I2C_SDA_PIN       62          // PB30
  #define I2C_SCL_PIN       63          // PB31
  #define I2C_SDA_PMUX      3           // D
  #define I2C_SCL_PMUX      3           // D
#else
  #define I2C_SERCOM        3           // pins must be set by the application
#endif

#if (I2C_MAP == 5) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (I2C_MAP == 6) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (I2C_MAP == 7) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (I2C_MAP == 8) && !(defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (I2C_MAP == 9) && !(defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (I2C_MAP == 10) && !(defined(SAMD21J))
  #error Pin mapping not available on this device
#endif

#if (I2C_SERCOM > 3) && defined(SAMD21E)
  #error SERCOM4 and SERCOM5 are not available on SAMD21E
#endif

#if   I2C_SERCOM == 0
  #define I2C_SER            SERCOM0
  #define I2C_GCLK_ID        SERCOM0_GCLK_ID_CORE
  #define I2C_APBMASK        PM_APBCMASK_SERCOM0
  #define I2C_IRQn           SERCOM0_IRQn
  #define I2C_IRQHandler     SERCOM0_IRQHandler
  #define I2C_DMAC_ID_TX     SERCOM0_DMAC_ID_TX
#elif I2C_SERCOM == 1
  #define I2C_SER            SERCOM1
  #define I2C_GCLK_ID        SERCOM1_GCLK_ID_CORE
  #define I2C_APBMASK        PM_APBCMASK_SERCOM1
  #define I2C_IRQn           SERCOM1_IRQn
  #define I2C_IRQHandler     SERCOM1_IRQHandler
  #define I2C_DMAC_ID_TX     SERCOM1_DMAC_ID_TX
#elif I2C_SERCOM == 2
  #define I2C_SER            SERCOM2
  #define I2C_GCLK_ID        SERCOM2_GCLK_ID_CORE
  #define I2C_APBMASK        PM_APBCMASK_SERCOM2
  #define I2C_IRQn           SERCOM2_IRQn
  #define I2C_IRQHandler     SERCOM2_IRQHandler
  #define I2C_DMAC_ID_TX     SERCOM2_DMAC_ID_TX
#elif I2C_SERCOM == 3
  #define I2C_SER            SERCOM3
  #define I2C_GCLK_ID        SERCOM3_GCLK_ID_CORE
  #define I2C_APBMASK        PM_APBCMASK_SERCOM3
  #define I2C_IRQn           SERCOM3_IRQn
  #define I2C_IRQHandler     SERCOM3_IRQHandler
  #define I2C_DMAC_ID_TX     SERCOM3_DMAC_ID_TX
#elif I2C_SERCOM == 4
  #define I2C_SER            SERCOM4
  #define I2C_GCLK_ID        SERCOM4_GCLK_ID_CORE
  #define I2C_APBMASK        PM_APBCMASK_SERCOM4
  #define I2C_IRQn           SERCOM4_IRQn
  #define I2C_IRQHandler     SERCOM4_IRQHandler
  #define I2C_DMAC_ID_TX     SERCOM4_DMAC_ID_TX
#elif I2C_SERCOM == 5
  #define I2C_SER            SERCOM5
  #define I2C_GCLK_ID        SERCOM5_GCLK_ID_CORE
  #define I2C_APBMASK        PM_APBCMASK_SERCOM5
  #define I2C_IRQn           SERCOM5_IRQn
  #define I2C_IRQHandler     SERCOM5_IRQHandler
  #define I2C_DMAC_ID_TX     SERCOM5_DMAC_ID_TX
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
