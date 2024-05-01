// ===================================================================================
// RDA5807 Basic Functions                                                    * v1.0 *
// ===================================================================================
//
// Basic functions for the RDA5807 digital stereo FM tuner IC.
//
// Functions available:
// --------------------
// RDA_init()               RDA initialize tuner
// RDA_setVolume(vol)       set volume
// RDA_setChannel(chan)     tune to a specified channel
// RDA_seekUp()             seek next channel
// RDA_updateStatus()       update status and handle RDS
// RDA_getFrequency()       calculate frequency in units of 10kHz
// RDA_waitTuning()         wait until tuning completed
//
// RDA_stationName[]        contains current station name
//
// Further information:     https://github.com/wagiminator/ATtiny412-PocketRadio
// 2022 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "i2c_soft.h"

// RDA parameters
#define RDA_INIT_VOL    3             // volume on system start (0..15)

// RDA definitions
#define RDA_ADDR_SEQ    0x10          // RDA5807 I2C device address for sequential access
#define RDA_ADDR_INDEX  0x11          // RDA5807 I2C device address for indexed access

#ifndef RDA_INIT_VOL
#define RDA_INIT_VOL    1             // start volume (0..15)
#endif

// RDA state macros
#define RDA_hasRdsData        ( RDA_read_regs[RDA_REG_A] & 0x8000 )
#define RDA_isTuning          (~RDA_read_regs[RDA_REG_A] & 0x4000 )
#define RDA_tuningError       ( RDA_read_regs[RDA_REG_A] & 0x2000 )
#define RDA_hasRdsBlockE      ( RDA_read_regs[RDA_REG_A] & 0x0800 )
#define RDA_isStereo          ( RDA_read_regs[RDA_REG_A] & 0x0400 )
#define RDA_channel           ( RDA_read_regs[RDA_REG_A] & 0x03FF )
#define RDA_isTunedToChannel  ( RDA_read_regs[RDA_REG_B] & 0x0100 )
#define RDA_rdsBlockE         ( RDA_read_regs[RDA_REG_B] & 0x0010 )
#define RDA_rdsBlockErrors    ( RDA_read_regs[RDA_REG_B] & 0x000F )
#define RDA_signalStrength    ((RDA_read_regs[RDA_REG_B] & 0xFE00 ) >> 9 )

// RDA variables
enum{ RDA_REG_2, RDA_REG_3, RDA_REG_4, RDA_REG_5, RDA_REG_6, RDA_REG_7 };
enum{ RDA_REG_A, RDA_REG_B, RDA_REG_C, RDA_REG_D, RDA_REG_E, RDA_REG_F };
extern char RDA_stationName[];
extern uint16_t RDA_read_regs[];
extern uint16_t RDA_write_regs[];

// RDA functions
void RDA_init(void);                  // RDA initialize tuner
void RDA_setVolume(uint8_t vol);      // RDA set volume
void RDA_setChannel(uint16_t chan);   // RDA tune to a specified channel
void RDA_seekUp(void);                // RDA seek next channel
void RDA_updateStatus(void);          // RDA update status and handle RDS
uint16_t RDA_getFrequency(void);      // Calculate frequency in units of 10kHz
void RDA_waitTuning(void);            // Wait until tuning completed

#ifdef __cplusplus
};
#endif
