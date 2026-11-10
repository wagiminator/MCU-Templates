// ===================================================================================
// Basic CAN Functions (CAN1, bxCAN) for CH32V203                             * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// CAN_init()               Init CAN with defined bit rate (see below), accept all frames
// CAN_write(id, buf, len)  Send frame with identifier (id), data (*buf) and length (len,
//                          0..8), waits for a free mailbox, returns 0 on success
// CAN_ready()              Check if a transmit mailbox is free
// CAN_completed()          Check if all frames have been transmitted
// CAN_available()          Check if a received frame is in the RX FIFO
// CAN_read(&id, buf)       Read received frame (waits), stores identifier in id and data
//                          in buf (8 bytes), returns data length (0..8)
// CAN_error()              Read error status register (TEC, REC, bus-off, ...)
//
// Identifier (id):
// ----------------
// Standard frames use 11-bit identifiers (0x000 .. 0x7FF). For extended frames (29-bit
// identifier) set the CAN_EXT flag, e.g. CAN_write(CAN_EXT | 0x1ABCDEF, buf, 8). For
// remote frames set the CAN_RTR flag. Received identifiers carry the same flags.
//
// CAN pin mapping (set below in CAN parameters):
// ----------------------------------------------
// CAN_MAP    0      1
// RX-pin    PA11   PB8
// TX-pin    PA12   PB9
//
// Notes:
// ------
// - PA11/PA12 are also the USBD pins. CAN and USBD share their SRAM on the CH32V203
//   and cannot be used at the same time.
// - The CAN module runs with PCLK1 (= F_CPU in this template). F_CPU must be a
//   multiple of 8 * CAN_BAUD.
// - An external CAN transceiver (e.g. TJA1050, SN65HVD230) is required. For a test
//   without transceiver, set CAN_LOOPBACK to 1 (internal loopback, silent mode).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// CAN parameters
#define CAN_BAUD          500000        // CAN bit rate (e.g. 125000, 250000, 500000, 1000000)
#define CAN_MAP           0             // CAN pin mapping (see above)
#define CAN_LOOPBACK      0             // 1: internal loopback and silent mode (test)

// Identifier flags
#define CAN_EXT           0x80000000    // extended identifier (29-bit)
#define CAN_RTR           0x40000000    // remote transmission request

// CAN functions
void CAN_init(void);
uint8_t CAN_write(uint32_t id, const uint8_t* buf, uint8_t len);
uint8_t CAN_read(uint32_t* id, uint8_t* buf);

#define CAN_ready()       (CAN1->TSTATR & (CAN_TSTATR_TME0 | CAN_TSTATR_TME1 | CAN_TSTATR_TME2))
#define CAN_completed()   ((CAN1->TSTATR & (CAN_TSTATR_TME0 | CAN_TSTATR_TME1 | CAN_TSTATR_TME2)) \
                          == (CAN_TSTATR_TME0 | CAN_TSTATR_TME1 | CAN_TSTATR_TME2))
#define CAN_available()   (CAN1->RFIFO0 & CAN_RFIFO0_FMP0)
#define CAN_error()       (CAN1->ERRSR)

#ifdef __cplusplus
};
#endif
