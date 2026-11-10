// ===================================================================================
// Basic FLASH Functions (DataFlash and CodeFlash) for CH573/CH571            * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "flash.h"

#define FLASH_WE  (RB_ROM_CTRL_EN | RB_ROM_DATA_WE | RB_ROM_CODE_WE)

// Read n bytes from DataFlash offset a into buffer b
RAM_FUNC uint8_t FLASH_DATA_read(uint32_t addr, void* buf, uint32_t len) {
  uint8_t* ptr = (uint8_t*)buf;
  if((addr + len) > FLASH_DATA_SIZE) return 1;
  FLASH_ROM_open(RB_ROM_CTRL_EN);
  FLASH_ROM_addr(0x0B, FLASH_DATA_START + addr);  // fast read
  while(len--) *ptr++ = FLASH_ROM_in();
  FLASH_ROM_close();
  return 0;
}

// Read single byte from DataFlash offset a
uint8_t FLASH_DATA_readByte(uint32_t addr) {
  uint8_t data = 0xFF;
  FLASH_DATA_read(addr, &data, 1);
  return data;
}

// Write n bytes from buffer b to DataFlash offset a (page by page)
RAM_FUNC uint8_t FLASH_DATA_write(uint32_t addr, const void* buf, uint32_t len) {
  const uint8_t* ptr = (const uint8_t*)buf;
  uint8_t result = 0;
  if((addr + len) > FLASH_DATA_SIZE) return 1;
  addr += FLASH_DATA_START;
  FLASH_ROM_open(FLASH_WE);
  while(len) {
    FLASH_ROM_addr(0x02, addr);                   // page program
    do {
      FLASH_ROM_out(*ptr++);
      addr++; len--;
    } while(len && (addr & 0xFF));                // until end of data or page
    if(!FLASH_ROM_wait()) {result = 1; break;}    // wait until written
  }
  FLASH_ROM_close();
  return result;
}

// Erase flash area at physical address (step sizes 64K/4K/256 bytes)
RAM_FUNC static uint8_t FLASH_erase(uint32_t addr, uint32_t len, uint32_t maxstep) {
  uint32_t step;
  uint8_t  result = 0;
  len  = (len + (addr & 0xFF) + 0xFF) & ~(uint32_t)0xFF;  // align to pages
  addr = addr & ~(uint32_t)0xFF;
  FLASH_ROM_open(FLASH_WE);
  while(len) {
    step = maxstep;
    while((step > 256) && ((addr & (step - 1)) || (len < step))) step >>= 4;
    FLASH_ROM_addr((step == 65536) ? 0xD8 : (step == 4096) ? 0x20 : 0x81, addr);
    if(!FLASH_ROM_wait()) {result = 1; break;}
    addr += step;
    len  -= step;
  }
  FLASH_ROM_close();
  return result;
}

// Erase DataFlash area (pages of 256 bytes) at offset a
uint8_t FLASH_DATA_erase(uint32_t addr, uint32_t len) {
  if((addr + len) > FLASH_DATA_SIZE) return 1;
  return FLASH_erase(FLASH_DATA_START + addr, len, 4096);
}

// Erase CodeFlash area (blocks of 4096 bytes) at address a
uint8_t FLASH_CODE_erase(uint32_t addr, uint32_t len) {
  len  = (len + (addr & 0xFFF) + 0xFFF) & ~(uint32_t)0xFFF; // align to blocks
  addr = addr & ~(uint32_t)0xFFF;
  if((addr + len) > FLASH_DATA_START) return 1;
  return FLASH_erase(addr, len, 65536);
}

// Write n bytes (multiple of 4, buffer 32-bit aligned) to CodeFlash address a
RAM_FUNC uint8_t FLASH_CODE_write(uint32_t addr, const void* buf, uint32_t len) {
  const uint32_t* ptr = (const uint32_t*)buf;
  uint8_t result = 0;
  if((addr & 3) || ((addr + len) > FLASH_DATA_START)) return 1;
  len >>= 2;
  FLASH_ROM_open(FLASH_WE);
  while(len) {
    FLASH_ROM_addr(0x02, addr);                   // page program
    do {
      R32_FLASH_DATA = *ptr++;                    // write 32-bit word
      for(uint8_t i=4; i; i--) {
        while((int8_t)R8_FLASH_CTRL < 0);
        R8_FLASH_CTRL = 0x15;                     // shift out byte
      }
      addr += 4; len--;
    } while(len && (addr & 0xFF));                // until end of data or page
    if(!FLASH_ROM_wait()) {result = 1; break;}
  }
  FLASH_ROM_close();
  return result;
}
