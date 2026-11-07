// ===================================================================================
// Basic SX1276/SX1278 (RFM95/RFM96/RFM98) LoRa Transceiver Functions         * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "sx1276.h"

// LoRa registers
#define LORA_REG_FIFO         0x00    // FIFO read/write access
#define LORA_REG_OPMODE       0x01    // operating mode
#define LORA_REG_FRF_MSB      0x06    // carrier frequency (3 bytes)
#define LORA_REG_PA_CONFIG    0x09    // power amplifier
#define LORA_REG_LNA          0x0C    // low noise amplifier
#define LORA_REG_FIFO_PTR     0x0D    // FIFO address pointer
#define LORA_REG_FIFO_TX      0x0E    // FIFO TX base address
#define LORA_REG_FIFO_RX      0x0F    // FIFO RX base address
#define LORA_REG_FIFO_RX_CUR  0x10    // start address of last packet received
#define LORA_REG_IRQ_FLAGS    0x12    // interrupt flags
#define LORA_REG_RX_BYTES     0x13    // number of received bytes
#define LORA_REG_PKT_SNR      0x19    // SNR of last packet
#define LORA_REG_PKT_RSSI     0x1A    // RSSI of last packet
#define LORA_REG_MODEM1       0x1D    // modem config 1 (BW, CR, header)
#define LORA_REG_MODEM2       0x1E    // modem config 2 (SF, CRC)
#define LORA_REG_PAYLOAD_LEN  0x22    // payload length
#define LORA_REG_MODEM3       0x26    // modem config 3 (LDRO, AGC)
#define LORA_REG_SYNC         0x39    // sync word
#define LORA_REG_VERSION      0x42    // chip version (0x12)
#define LORA_REG_PA_DAC       0x4D    // high power settings

// Operating modes (LoRa)
#define LORA_MODE_SLEEP       0x80
#define LORA_MODE_STDBY       0x81
#define LORA_MODE_TX          0x83
#define LORA_MODE_RXCONT      0x85

// IRQ flags
#define LORA_IRQ_TX_DONE      0x08
#define LORA_IRQ_CRC_ERR      0x20
#define LORA_IRQ_RX_DONE      0x40

// Carrier frequency register value: FRF = f * 2^19 / 32MHz = f[kHz] * 2048 / 125
#define LORA_FRF              ((uint32_t)LORA_FREQ * 2048UL / 125UL)

// Low data rate optimization for symbol times > 16ms
#if (LORA_SF == 11 && LORA_BW <= 7) || (LORA_SF == 12 && LORA_BW <= 8)
  #define LORA_LDRO           0x08
#else
  #define LORA_LDRO           0x00
#endif

// Write register (internal)
static void LORA_writeReg(uint8_t reg, uint8_t value) {
  PIN_low(LORA_PIN_NSS);
  SPI_transfer(reg | 0x80);
  SPI_transfer(value);
  PIN_high(LORA_PIN_NSS);
}

// Read register (internal)
static uint8_t LORA_readReg(uint8_t reg) {
  uint8_t value;
  PIN_low(LORA_PIN_NSS);
  SPI_transfer(reg & 0x7F);
  value = SPI_transfer(0);
  PIN_high(LORA_PIN_NSS);
  return value;
}

// Init module, returns 1 if module was found
uint8_t LORA_init(void) {
  PIN_high(LORA_PIN_NSS);
  PIN_output(LORA_PIN_NSS);
  #if LORA_INIT_SPI > 0
  SPI_init();
  #endif
  #ifdef LORA_PIN_RST
  PIN_low(LORA_PIN_RST);                          // hardware reset
  PIN_output(LORA_PIN_RST);
  DLY_ms(1);
  PIN_input(LORA_PIN_RST);
  #endif
  DLY_ms(10);
  if(LORA_readReg(LORA_REG_VERSION) != 0x12) return 0;

  LORA_writeReg(LORA_REG_OPMODE, 0x00);           // FSK sleep mode
  LORA_writeReg(LORA_REG_OPMODE, LORA_MODE_SLEEP);// LoRa sleep mode
  LORA_writeReg(LORA_REG_FRF_MSB,     (uint8_t)(LORA_FRF >> 16)); // frequency
  LORA_writeReg(LORA_REG_FRF_MSB + 1, (uint8_t)(LORA_FRF >> 8));
  LORA_writeReg(LORA_REG_FRF_MSB + 2, (uint8_t)(LORA_FRF));
  LORA_writeReg(LORA_REG_FIFO_TX, 0x00);          // use whole FIFO for TX and RX
  LORA_writeReg(LORA_REG_FIFO_RX, 0x00);
  LORA_writeReg(LORA_REG_LNA, LORA_readReg(LORA_REG_LNA) | 0x03); // LNA boost
  LORA_writeReg(LORA_REG_MODEM1, (LORA_BW << 4) | (LORA_CR << 1)); // explicit header
  LORA_writeReg(LORA_REG_MODEM2, (LORA_SF << 4) | 0x04);           // CRC on
  LORA_writeReg(LORA_REG_MODEM3, LORA_LDRO | 0x04);                // AGC auto
  LORA_writeReg(LORA_REG_SYNC, LORA_SYNC);
  #if LORA_POWER > 17
  LORA_writeReg(LORA_REG_PA_CONFIG, 0x8F);        // PA_BOOST, +17dBm
  #elif LORA_POWER < 2
  LORA_writeReg(LORA_REG_PA_CONFIG, 0x80);        // PA_BOOST, +2dBm
  #else
  LORA_writeReg(LORA_REG_PA_CONFIG, 0x80 | (LORA_POWER - 2)); // PA_BOOST
  #endif
  LORA_receive();
  return 1;
}

// Switch to continuous receive mode
void LORA_receive(void) {
  LORA_writeReg(LORA_REG_OPMODE, LORA_MODE_RXCONT);
}

// Switch to sleep mode
void LORA_sleep(void) {
  LORA_writeReg(LORA_REG_OPMODE, LORA_MODE_SLEEP);
}

// Send packet (blocking), returns 1 on success
uint8_t LORA_send(const uint8_t* buf, uint8_t len) {
  uint16_t timeout = 5000;                        // max. 5s
  uint8_t  result;
  if(!len) return 0;
  LORA_writeReg(LORA_REG_OPMODE, LORA_MODE_STDBY);
  LORA_writeReg(LORA_REG_FIFO_PTR, 0x00);
  PIN_low(LORA_PIN_NSS);                          // write payload into FIFO
  SPI_transfer(LORA_REG_FIFO | 0x80);
  for(result = len; result; result--) SPI_transfer(*buf++);
  PIN_high(LORA_PIN_NSS);
  LORA_writeReg(LORA_REG_PAYLOAD_LEN, len);
  LORA_writeReg(LORA_REG_OPMODE, LORA_MODE_TX);   // start transmission
  while(!(LORA_readReg(LORA_REG_IRQ_FLAGS) & LORA_IRQ_TX_DONE) && --timeout) DLY_ms(1);
  result = timeout ? 1 : 0;
  LORA_writeReg(LORA_REG_IRQ_FLAGS, 0xFF);        // clear flags
  LORA_receive();
  return result;
}

// Check if a packet was received
uint8_t LORA_available(void) {
  return (LORA_readReg(LORA_REG_IRQ_FLAGS) & LORA_IRQ_RX_DONE) ? 1 : 0;
}

// Read received packet, returns length (0 if CRC error or nothing received)
uint8_t LORA_read(uint8_t* buf) {
  uint8_t i, len = 0;
  uint8_t flags = LORA_readReg(LORA_REG_IRQ_FLAGS);
  if((flags & LORA_IRQ_RX_DONE) && !(flags & LORA_IRQ_CRC_ERR)) {
    len = LORA_readReg(LORA_REG_RX_BYTES);
    LORA_writeReg(LORA_REG_FIFO_PTR, LORA_readReg(LORA_REG_FIFO_RX_CUR));
    PIN_low(LORA_PIN_NSS);                        // read payload from FIFO
    SPI_transfer(LORA_REG_FIFO);
    for(i = len; i; i--) *buf++ = SPI_transfer(0);
    PIN_high(LORA_PIN_NSS);
  }
  LORA_writeReg(LORA_REG_IRQ_FLAGS, 0xFF);        // clear flags
  return len;
}

// Get RSSI of last received packet in dBm
int16_t LORA_getRSSI(void) {
  #if LORA_FREQ > 779000
  return (int16_t)LORA_readReg(LORA_REG_PKT_RSSI) - 157;  // high frequency port
  #else
  return (int16_t)LORA_readReg(LORA_REG_PKT_RSSI) - 164;  // low frequency port
  #endif
}

// Get SNR of last received packet in dB
int8_t LORA_getSNR(void) {
  return (int8_t)LORA_readReg(LORA_REG_PKT_SNR) / 4;
}
