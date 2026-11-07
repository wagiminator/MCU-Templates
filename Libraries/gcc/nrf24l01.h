// ===================================================================================
// nRF24L01+ Functions                                                        * v1.1 *
// ===================================================================================
//
// Collection of the most necessary functions for controlling an nRF24L01+.
//
// Functions available:
// --------------------
// NRF_init()               init NRF
// NRF_configure()          configure NRF
// NRF_available()          check if data is available for reading
// NRF_readPayload(*buf)    read payload into buffer, return length
// NRF_writePayload(*buf,l) send a data package of length l (1-32),
//                          returns 1 if acknowledged by the receiver, 0 if not
// NRF_powerDown()          switch to power down mode (NRF_configure() to wake up)
//
// Variables:
// ----------
// NRF_channel              channel (0x00 - 0x7F)
// NRF_speed                speed - 0:250kbps, 1:1Mbps, 2:2Mbps
// NRF_tx_addr[]            transmit address (5 bytes)
// NRF_rx_addr[]            receive address (5 bytes)
//
// Notes:
// ------
// - Change the variables before calling NRF_init() or call NRF_configure() afterwards.
// - Auto-acknowledge and dynamic payload length are enabled. The receiver listens on
//   pipe 1 (NRF_rx_addr), the transmitter's TX address must match it.
// - SPI: mode 0, max. 10MHz.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "spi.h"                                  // choose your SPI library
#include "gpio.h"
#include "system.h"
#ifdef __SDCC
#include "delay.h"
#endif

// NRF parameters
#define PIN_CSN               PA0                 // NRF SPI slave select pin
#define PIN_CE                PA1                 // NRF cable enable pin
#define NRF_INIT_SPI          1                   // 1: init SPI with NRF_init()
#define NRF_CONFIG            0x0C                // CRC scheme, 0x08:8bit, 0x0C:16bit

// NRF registers
#define NRF_REG_CONFIG        0x00                // configuration register
#define NRF_REG_SETUP_RETR    0x04                // automatic retransmission
#define NRF_REG_RF_CH         0x05                // RF frequency channel
#define NRF_REG_RF_SETUP      0x06                // RF setup register
#define NRF_REG_STATUS        0x07                // status register
#define NRF_REG_RX_ADDR_P0    0x0A                // RX address pipe 0
#define NRF_REG_RX_ADDR_P1    0x0B                // RX address pipe 1
#define NRF_REG_TX_ADDR       0x10                // TX address
#define NRF_REG_FIFO_STATUS   0x17                // FIFO status register
#define NRF_REG_DYNPD         0x1C                // enable dynamic payload length
#define NRF_REG_FEATURE       0x1D                // feature

// NRF commands
#define NRF_CMD_R_RX_PL_WID   0x60                // read RX payload length
#define NRF_CMD_R_RX_PAYLOAD  0x61                // read RX payload
#define NRF_CMD_W_TX_PAYLOAD  0xA0                // write TX payload
#define NRF_CMD_FLUSH_TX      0xE1                // flush TX FIFO
#define NRF_CMD_FLUSH_RX      0xE2                // flush RX FIFO

// NRF variables
extern uint8_t NRF_channel;                       // channel (0x00 - 0x7F)
extern uint8_t NRF_speed;                         // 0:250kbps, 1:1Mbps, 2:2Mbps
extern uint8_t NRF_tx_addr[];                     // transmit address
extern uint8_t NRF_rx_addr[];                     // receive address
extern const char* const NRF_STR[];               // speed strings

// NRF functions
void NRF_init(void);                              // init NRF
void NRF_configure(void);                         // configure NRF
uint8_t NRF_available(void);                      // check if data is available for reading
uint8_t NRF_readPayload(uint8_t *buf);            // read payload into buffer, return length
uint8_t NRF_writePayload(const uint8_t *buf, uint8_t len); // send a data package (max 32)
void NRF_powerDown(void);                         // switch to power down mode

#ifdef __cplusplus
}
#endif
