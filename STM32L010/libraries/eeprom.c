// ===================================================================================
// Basic EEPROM Functions for STM32L0xx                                       * v1.1 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "eeprom.h"

// Flash keys
#define FLASH_PEKEY1      ((uint32_t)0x89ABCDEF)
#define FLASH_PEKEY2      ((uint32_t)0x02030405)

// Error flags of FLASH_SR
#define FLASH_SR_ERRORS   (FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_SIZERR \
                         | FLASH_SR_OPTVERR | FLASH_SR_RDERR | FLASH_SR_NOTZEROERR \
                         | FLASH_SR_FWWERR)

// Unlock data EEPROM and FLASH_PECR register
void EEPROM_unlock(void) {
  while(FLASH->SR & FLASH_SR_BSY);        // wait till no operation is on going
  if(FLASH->PECR & FLASH_PECR_PELOCK) {   // check if the PELOCK is unlocked
    FLASH->PEKEYR = FLASH_PEKEY1;         // perform unlock sequence
    FLASH->PEKEYR = FLASH_PEKEY2;
  }
}

// Lock data EEPROM and FLASH_PECR
void EEPROM_lock(void) {
  while(FLASH->SR & FLASH_SR_BSY);        // wait till no operation is on going
  FLASH->PECR |= FLASH_PECR_PELOCK;       // lock the NVM by setting PELOCK
}

// Start write/erase operation and wait until it is completed, returns 1 on success
static uint8_t EEPROM_wait(void) {
  uint32_t sr;
  while((sr = FLASH->SR) & FLASH_SR_BSY);       // wait till completed
  FLASH->SR = sr & (FLASH_SR_EOP | FLASH_SR_ERRORS); // clear flags
  return !(sr & FLASH_SR_ERRORS);               // success if no error flag is set
}

// Check if address range is inside the EEPROM
static inline uint8_t EEPROM_valid(uint16_t addr, uint8_t len) {
  return((uint32_t)addr + len <= EEPROM_size());
}

// Prepare write/erase: unlock EEPROM, clear old flags
static void EEPROM_begin(void) {
  EEPROM_unlock();                                // unlock EEPROM
  FLASH->SR = FLASH_SR_EOP | FLASH_SR_ERRORS;     // clear EOP and error flags
}

// Erase data EEPROM word (set to 0x00000000)
uint8_t EEPROM_erase(uint16_t addr) {
  uint8_t ok;
  addr &= 0xfffc;                                 // addr must be 4-byte-aligned
  if(!EEPROM_valid(addr, 4)) return 0;            // valid address?
  EEPROM_begin();                                 // unlock EEPROM
  FLASH->PECR |= FLASH_PECR_ERASE | FLASH_PECR_DATA;          // enable word erase
  *(__IO uint32_t *)(DATA_EEPROM_BASE + addr) = (uint32_t)0;  // start erase
  ok = EEPROM_wait();                             // wait till completed
  FLASH->PECR &= ~(FLASH_PECR_ERASE | FLASH_PECR_DATA);       // disable word erase
  EEPROM_lock();                                  // lock EEPROM
  return ok;
}

// Write 32-bit word to EEPROM
uint8_t EEPROM_write32(uint16_t addr, uint32_t value) {
  uint8_t ok = 1;
  addr &= 0xfffc;                                 // addr must be 4-byte-aligned
  if(!EEPROM_valid(addr, 4)) return 0;            // valid address?
  if(value != EEPROM_read32(addr)) {              // value update needed?
    EEPROM_begin();                               // unlock EEPROM
    *(__IO uint32_t *)(DATA_EEPROM_BASE + addr) = value;      // write new value
    ok = EEPROM_wait();                           // wait till completed
    EEPROM_lock();                                // lock EEPROM
  }
  return ok;
}

// Write 16-bit half-word to EEPROM (direct half-word write, RM0451 chapter 3.3.3)
uint8_t EEPROM_write16(uint16_t addr, uint16_t value) {
  uint8_t ok = 1;
  addr &= 0xfffe;                                 // addr must be 2-byte-aligned
  if(!EEPROM_valid(addr, 2)) return 0;            // valid address?
  if(value != EEPROM_read16(addr)) {              // value update needed?
    EEPROM_begin();                               // unlock EEPROM
    *(__IO uint16_t *)(DATA_EEPROM_BASE + addr) = value;      // write new value
    ok = EEPROM_wait();                           // wait till completed
    EEPROM_lock();                                // lock EEPROM
  }
  return ok;
}

// Write 8-bit byte to EEPROM (direct byte write, RM0451 chapter 3.3.3)
uint8_t EEPROM_write8(uint16_t addr, uint8_t value) {
  uint8_t ok = 1;
  if(!EEPROM_valid(addr, 1)) return 0;            // valid address?
  if(value != EEPROM_read8(addr)) {               // value update needed?
    EEPROM_begin();                               // unlock EEPROM
    *(__IO uint8_t *)(DATA_EEPROM_BASE + addr) = value;       // write new value
    ok = EEPROM_wait();                           // wait till completed
    EEPROM_lock();                                // lock EEPROM
  }
  return ok;
}
