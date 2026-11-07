// ===================================================================================
// Basic SX1276/SX1278 (RFM95/RFM96/RFM98) LoRa Transceiver Functions         * v1.0 *
// ===================================================================================
//
// Point-to-point LoRa communication (no LoRaWAN) with explicit header and CRC.
//
// Functions available:
// --------------------
// LORA_init()              init module, returns 1 if module was found
// LORA_send(*buf,l)        send packet of length l (1 - 255), blocking, returns 1 on
//                          success, then switches back to receive mode
// LORA_available()         check if a packet was received
// LORA_read(*buf)          read received packet into buffer (max. 255 bytes), returns
//                          length (0 if CRC error)
// LORA_getRSSI()           get RSSI of last received packet in dBm
// LORA_getSNR()            get SNR of last received packet in dB
// LORA_sleep()             switch to sleep mode (LORA_receive() to wake up)
// LORA_receive()           switch to continuous receive mode
//
// Notes:
// ------
// - All modules of a network need the same frequency, spreading factor, bandwidth,
//   coding rate and sync word.
// - The transmit power uses the PA_BOOST pin (all RFM9x modules).
// - Observe the duty cycle limits of your frequency band.
// - SPI: mode 0, max. 10MHz.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"
#include "spi.h"                      // choose your SPI library
#include "system.h"
#ifdef __SDCC
#include "delay.h"
#endif

// LoRa pins
#define LORA_PIN_NSS          PA4     // pin connected to NSS (chip select)
#define LORA_PIN_RST          PA3     // pin connected to RESET (comment out if unused)

// LoRa parameters
#define LORA_INIT_SPI         1       // 1: init SPI with LORA_init()
#define LORA_FREQ             868100  // frequency in kHz (e.g. 433175, 868100, 915000)
#define LORA_SF               7       // spreading factor (7 - 12)
#define LORA_BW               7       // bandwidth: 6: 62.5kHz, 7: 125kHz, 8: 250kHz,
                                      //            9: 500kHz
#define LORA_CR               1       // coding rate: 1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8
#define LORA_POWER            14      // transmit power in dBm (2 - 17)
#define LORA_SYNC             0x12    // sync word (0x12: private, 0x34: LoRaWAN)

// LoRa functions
uint8_t LORA_init(void);              // init module
uint8_t LORA_send(const uint8_t* buf, uint8_t len); // send packet
uint8_t LORA_available(void);         // check if a packet was received
uint8_t LORA_read(uint8_t* buf);      // read received packet
int16_t LORA_getRSSI(void);           // RSSI of last packet in dBm
int8_t  LORA_getSNR(void);            // SNR of last packet in dB
void LORA_sleep(void);                // switch to sleep mode
void LORA_receive(void);              // switch to continuous receive mode

#ifdef __cplusplus
}
#endif
