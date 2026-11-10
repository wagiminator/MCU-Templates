// ===================================================================================
// SHA-256 Hardware Accelerator Functions for RP2350                          * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// SHA256_init()            Init SHA-256 accelerator and start a new hash
// SHA256_update(buf,len)   Add (len) bytes from buffer (*buf) to the hash
// SHA256_final(hash)       Finish the hash, store result (32 bytes) in hash
// SHA256_calc(buf,len,hash) Calculate SHA-256 of buffer in one step
//
// Notes:
// ------
// - The padding of the message is done in software, the 64-byte blocks are
//   processed by the hardware. The result is in the usual big-endian byte order.
// - Only one hash can be calculated at a time.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

void SHA256_init(void);                                   // start new hash
void SHA256_update(const uint8_t* buf, uint32_t len);     // add data
void SHA256_final(uint8_t* hash);                         // finish, get result
void SHA256_calc(const uint8_t* buf, uint32_t len, uint8_t* hash); // one step

#ifdef __cplusplus
};
#endif
