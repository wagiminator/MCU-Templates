// ===================================================================================
// nRF24L01+ Functions                                                        * v1.0 *
// ===================================================================================
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "spi.h"
#include "gpio.h"
#include "config.h"

// NRF variables
extern __xdata uint8_t NRF_channel;             // channel (0x00 - 0x7F)
extern __xdata uint8_t NRF_speed;               // 0:250kbps, 1:1Mbps, 2:2Mbps
extern __xdata uint8_t NRF_tx_addr[];           // transmit address
extern __xdata uint8_t NRF_rx_addr[];           // receive address
extern __code uint8_t* NRF_STR[];               // speed strings

// NRF functions
void NRF_init(void);                            // init NRF
void NRF_configure(void);                       // configure NRF
uint8_t NRF_available(void);                    // check if data is available for reading
uint8_t NRF_readPayload(__xdata uint8_t *buf); // read payload into buffer, return length
void NRF_writePayload(__xdata uint8_t *buf, uint8_t len);  // send a data package (max length 32)
