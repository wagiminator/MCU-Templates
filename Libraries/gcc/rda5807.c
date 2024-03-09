// ===================================================================================
// RDA5807 Basic Functions                                                    * v1.0 *
// ===================================================================================
// 2022 by Stefan Wagner:   https://github.com/wagiminator

#include "rda5807.h"

// RDA register definitions
uint16_t RDA_read_regs[6];                        // RDA registers for reading
uint16_t RDA_write_regs[6] = {                    // RDA registers for writing:
  0b1101001000001101,                             // RDA register 0x02 preset
  0b0001010111000000,                             // RDA register 0x03 preset
  0b0000101000000000,                             // RDA register 0x04 preset
  0b1000100010000000,                             // RDA register 0x05 preset
  0b0000000000000000,                             // RDA register 0x06 preset
  0b0000000000000000                              // RDA register 0x07 preset
};

// RDA variables
char RDA_stationName[9];                          // string for the station name
char RDA_rdsStationName[8];                       // just for internal use

// RDA write specified register
void RDA_writeReg(uint8_t reg) {
  I2C_start(RDA_ADDR_INDEX);                      // start I2C for index write to RDA
  I2C_write(0x02 + reg);                          // set the register to write
  I2C_write(RDA_write_regs[reg] >> 8);            // send high byte
  I2C_write(RDA_write_regs[reg]);                 // send low byte
  I2C_stop();                                     // stop I2C
}

// RDA write all registers
void RDA_writeAllRegs(void) {
  I2C_start(RDA_ADDR_SEQ);                        // start I2C for sequential write to RDA
  for(uint8_t i=0; i<6; i++) {                    // write to 6 registers
    I2C_write(RDA_write_regs[i] >> 8);            // send high byte
    I2C_write(RDA_write_regs[i]);                 // send low byte
  }
  I2C_stop();                                     // stop I2C
}

// RDA read all registers
void RDA_readAllRegs(void) {
  I2C_start(RDA_ADDR_SEQ | 1);                    // start I2C for sequential read from RDA
  for(uint8_t i=0; i<6; i++)                      // read 6 registers
    RDA_read_regs[i] = (uint16_t)(I2C_read(1) << 8) | I2C_read(5-i);
  I2C_stop();                                     // stop I2C
}

// RDA clear station
void RDA_resetStation(void) {
  for(uint8_t i=0; i<8; i++) RDA_stationName[i] = ' ';
}

// RDA initialize tuner
void RDA_init(void) {
  RDA_resetStation();                             // reset station available
  RDA_stationName[8] = 0;                         // set string terminator
  RDA_write_regs[RDA_REG_2] |=  0x0002;           // set soft reset
  RDA_writeReg(RDA_REG_2);                        // write to register 0x02
  RDA_write_regs[RDA_REG_2] &= ~0x0002;           // clear soft reset
  RDA_write_regs[RDA_REG_5] |=  RDA_INIT_VOL;     // set start volume
  RDA_writeAllRegs();                             // write all registers
}

// RDA set volume
void RDA_setVolume(uint8_t vol) {
  RDA_write_regs[RDA_REG_5] &= ~0x000F;           // clear volume bits
  RDA_write_regs[RDA_REG_5] |=  vol;              // set volume
  RDA_writeReg(RDA_REG_5);                        // write to register 0x05
}

// RDA tune to a specified channel
void RDA_setChannel(uint16_t chan) {
  RDA_resetStation();
  RDA_write_regs[RDA_REG_3] &= ~0xFFC0;           // clear channel
  RDA_write_regs[RDA_REG_3] |= (chan << 6) | 0x0010;  // set channel and tune enable
  RDA_writeReg(RDA_REG_3);                        // write register
}

// RDA seek next channel
void RDA_seekUp(void) {
  RDA_resetStation();                             // clear station name
  RDA_write_regs[RDA_REG_2] |=  0x0100;           // set seek enable bit
  RDA_writeReg(RDA_REG_2);                        // write to register 0x02
}

// RDA update status and handle RDS
void RDA_updateStatus(void) {
  RDA_readAllRegs();                              // read all registers

  // When tuned disable tuning and stop seeking
  if (!RDA_isTuning) {
    RDA_write_regs[RDA_REG_3] &= ~0x0010;         // clear tune enable flag
    RDA_writeReg(RDA_REG_3);
    RDA_write_regs[RDA_REG_2] &= ~0x0100;         // clear seek enable flag
    RDA_writeReg(RDA_REG_2);
  }

  // Check for RDS data
  if(RDA_hasRdsData) {                            // RDS ready?
    // Toggle RDS flag to request new data
    RDA_write_regs[RDA_REG_2] &= ~0x0008;         // clear RDS flag
    RDA_writeReg(RDA_REG_2);                      // write to register 0x02
    RDA_write_regs[RDA_REG_2] |=  0x0008;         // set RDS flag
    RDA_writeReg(RDA_REG_2);                      // write to register 0x02

    // Decode RDS message (station name)
    if(!RDA_rdsBlockE) {                                         // REG_B..F carrying blocks A-D?
      if( (RDA_read_regs[RDA_REG_D] & 0xF800) == 0x0000) {       // is it station name?
        uint8_t offset = (RDA_read_regs[RDA_REG_D] & 0x03) << 1; // get character position
        uint8_t c1 = RDA_read_regs[RDA_REG_F] >> 8;              // get character 1
        uint8_t c2 = RDA_read_regs[RDA_REG_F];                   // get character 2

        // Copy station name characters only if received twice in a row...
        if(RDA_rdsStationName[offset] == c1)                     // 1st char received twice?
             RDA_stationName[offset] = c1;                       // copy to station name
        else RDA_rdsStationName[offset] = c1;                    // save for next test
        if(RDA_rdsStationName[offset + 1] == c2)                 // 2nd char received twice?
             RDA_stationName[offset + 1] = c2;                   // copy to station name
        else RDA_rdsStationName[offset + 1] = c2;                // save for next test
      }
    }
  }
}

// Calculate frequency in 10kHz
uint16_t RDA_getFrequency(void) {
  return(8700 + (RDA_channel << 3) + (RDA_channel << 1));
}

// Wait until tuning completed
void RDA_waitTuning(void) {
  do {
    DLY_ms(100);
    RDA_updateStatus();
  } while(RDA_isTuning);
}
