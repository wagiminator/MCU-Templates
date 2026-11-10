// ===================================================================================
// SHA-256 Hardware Accelerator Functions for RP2350                          * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "sha256.h"

static uint32_t SHA256_word;                      // word being assembled (big-endian)
static uint64_t SHA256_count;                     // number of bytes added

// Write next byte to the accelerator
static void SHA256_putByte(uint8_t b) {
  SHA256_word = (SHA256_word << 8) | b;
  if((++SHA256_count & 3) == 0) {                 // 4 bytes complete?
    while(!(SHA256->CSR & SHA256_CSR_WDATA_RDY_Msk)); // wait until ready
    SHA256->WDATA = SHA256_word;                  // write word
  }
}

// Init SHA-256 accelerator and start a new hash
void SHA256_init(void) {
  RESET_release(RESETS_RESET_SHA256);             // release SHA256 from reset
  SHA256->CSR  = SHA256_CSR_START_Msk;            // no byte swap, start new hash
  SHA256_word  = 0;
  SHA256_count = 0;
}

// Add data to the hash
void SHA256_update(const uint8_t* buf, uint32_t len) {
  while(len--) SHA256_putByte(*buf++);
}

// Finish the hash and store the result (32 bytes)
void SHA256_final(uint8_t* hash) {
  uint64_t bits = SHA256_count << 3;              // message length in bits
  uint8_t  i;
  uint32_t sum;
  SHA256_putByte(0x80);                           // padding: 1-bit
  while((SHA256_count & 63) != 56) SHA256_putByte(0); // zeros up to 56 bytes in block
  for(i = 0; i < 8; i++) SHA256_putByte((uint8_t)(bits >> (56 - (i << 3)))); // length
  while(!(SHA256->CSR & SHA256_CSR_SUM_VLD_Msk)); // wait for result
  for(i = 0; i < 8; i++) {                        // read result (big-endian)
    sum = SHA256->SUM[i];
    *hash++ = sum >> 24; *hash++ = sum >> 16; *hash++ = sum >> 8; *hash++ = sum;
  }
}

// Calculate SHA-256 of buffer in one step
void SHA256_calc(const uint8_t* buf, uint32_t len, uint8_t* hash) {
  SHA256_init();
  SHA256_update(buf, len);
  SHA256_final(hash);
}
