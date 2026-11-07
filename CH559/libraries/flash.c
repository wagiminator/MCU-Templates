// ===================================================================================
// Data Flash Functions for CH559                                             * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "flash.h"

// Execute flash command at address (internal)
static uint8_t FLASH_cmd(uint16_t addr, uint8_t cmd) {
  uint8_t ea = EA;
  EA = 0;                                     // disable interrupts
  ROM_ADDR_L  = (uint8_t)addr;                // set address
  ROM_ADDR_H  = (uint8_t)(addr >> 8);
  SAFE_MOD    = 0x55;
  SAFE_MOD    = 0xAA;                         // enter safe mode
  GLOBAL_CFG |= bDATA_WE;                     // enable data flash write
  SAFE_MOD    = 0;                            // exit safe mode
  if(ROM_STATUS & bROM_ADDR_OK) ROM_CTRL = cmd; // execute command (CPU halted)
  SAFE_MOD    = 0x55;
  SAFE_MOD    = 0xAA;                         // enter safe mode
  GLOBAL_CFG &= ~bDATA_WE;                    // disable data flash write
  SAFE_MOD    = 0;                            // exit safe mode
  EA = ea;                                    // restore interrupts
  return ROM_STATUS & (bROM_CMD_ERR | bROM_CMD_TOUT);
}

// Erase complete data flash
uint8_t FLASH_erase(void) {
  return FLASH_cmd(DATA_FLASH_ADDR, ROM_CMD_ERASE);
}

// Write 16-bit word at even address
uint8_t FLASH_write(uint16_t addr, uint16_t data) {
  if(addr >= FLASH_SIZE) return 1;
  ROM_DATA_L = (uint8_t)data;                 // set data
  ROM_DATA_H = (uint8_t)(data >> 8);
  return FLASH_cmd(DATA_FLASH_ADDR + (addr & ~1), ROM_CMD_PROG);
}
