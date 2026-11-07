// ===================================================================================
// Basic W25Qxx SPI NOR Flash Functions                                       * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "w25q.h"

// W25Q commands
#define W25_CMD_WRITE_EN      0x06    // write enable
#define W25_CMD_READ_SR1      0x05    // read status register 1
#define W25_CMD_PAGE_PROG     0x02    // page program
#define W25_CMD_READ          0x03    // read data
#define W25_CMD_SECTOR_ERASE  0x20    // sector erase (4KB)
#define W25_CMD_CHIP_ERASE    0xC7    // chip erase
#define W25_CMD_POWER_DOWN    0xB9    // power down
#define W25_CMD_RELEASE_PD    0xAB    // release power down
#define W25_CMD_JEDEC_ID      0x9F    // read JEDEC ID

#define W25_select()          PIN_low(W25_PIN_CS)
#define W25_deselect()        PIN_high(W25_PIN_CS)

// Send single command (internal)
static void W25_command(uint8_t cmd) {
  W25_select();
  SPI_transfer(cmd);
  W25_deselect();
}

// Send command with 24-bit address, keep CS low (internal)
static void W25_commandAddr(uint8_t cmd, uint32_t addr) {
  W25_select();
  SPI_transfer(cmd);
  SPI_transfer(addr >> 16);
  SPI_transfer(addr >> 8);
  SPI_transfer(addr);
}

// Check if a write or erase operation is in progress
uint8_t W25_busy(void) {
  uint8_t status;
  W25_select();
  SPI_transfer(W25_CMD_READ_SR1);
  status = SPI_transfer(0);
  W25_deselect();
  return status & 0x01;
}

// Wait until write or erase operation is completed (internal)
static void W25_wait(void) {
  while(W25_busy());
}

// Init CS pin (and SPI), wake up flash
void W25_init(void) {
  W25_deselect();
  PIN_output(W25_PIN_CS);
  #if W25_INIT_SPI > 0
  SPI_init();
  #endif
  W25_sleep(0);
}

// Set power-down state (0: wake up, 1: power down)
void W25_sleep(uint8_t slp) {
  W25_command(slp ? W25_CMD_POWER_DOWN : W25_CMD_RELEASE_PD);
  DLY_us(5);
}

// Read JEDEC ID
uint32_t W25_readID(void) {
  uint32_t id;
  W25_select();
  SPI_transfer(W25_CMD_JEDEC_ID);
  id  = (uint32_t)SPI_transfer(0) << 16;
  id |= (uint16_t)SPI_transfer(0) << 8;
  id |= SPI_transfer(0);
  W25_deselect();
  return id;
}

// Read len bytes from address into buffer
void W25_read(uint32_t addr, uint8_t* buf, uint16_t len) {
  W25_commandAddr(W25_CMD_READ, addr);
  while(len--) *buf++ = SPI_transfer(0);
  W25_deselect();
}

// Write len bytes from buffer to address (area must be erased)
void W25_write(uint32_t addr, const uint8_t* buf, uint16_t len) {
  while(len) {
    W25_command(W25_CMD_WRITE_EN);
    W25_commandAddr(W25_CMD_PAGE_PROG, addr);
    do {                                          // until end of data or page
      SPI_transfer(*buf++);
      addr++; len--;
    } while(len && (addr & 0xFF));
    W25_deselect();
    W25_wait();
  }
}

// Erase 4KB sector containing address
void W25_eraseSector(uint32_t addr) {
  W25_command(W25_CMD_WRITE_EN);
  W25_commandAddr(W25_CMD_SECTOR_ERASE, addr);
  W25_deselect();
  W25_wait();
}

// Erase whole chip
void W25_eraseChip(void) {
  W25_command(W25_CMD_WRITE_EN);
  W25_command(W25_CMD_CHIP_ERASE);
  W25_wait();
}
