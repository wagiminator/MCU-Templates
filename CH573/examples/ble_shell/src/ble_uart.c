// ===================================================================================
// Bluetooth LE UART (Nordic UART Service)                                    * v1.0 *
// ===================================================================================
// 2025 by Stefan Wagner:   https://github.com/wagiminator

#include "ble_uart.h"

// Nordic UART Service UUIDs (LSB first)
static const uint8_t BUART_UUID_SERVICE[16] = {
  0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E };
static const uint8_t BUART_UUID_RX[16] = {
  0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x02, 0x00, 0x40, 0x6E };
static const uint8_t BUART_UUID_TX[16] = {
  0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x03, 0x00, 0x40, 0x6E };

static uint8_t  BUART_rxChar[BLE_ATT_MTU];          // value buffers of characteristics
static uint8_t  BUART_txChar[BLE_ATT_MTU];
static uint8_t  BUART_rxBuf[BUART_RXBUF];           // receive ring buffer
static volatile uint16_t BUART_rxHead, BUART_rxCount;
static uint8_t  BUART_txBuf[BLE_ATT_MTU];           // transmit buffer
static uint16_t BUART_txLen;
static uint16_t BUART_txHandle;

// Called when the client writes to the RX characteristic
static void BUART_received(uint16_t handle, const uint8_t* data, uint16_t len) {
  (void)handle;
  while(len-- && (BUART_rxCount < BUART_RXBUF)) {
    BUART_rxBuf[(BUART_rxHead + BUART_rxCount) % BUART_RXBUF] = *data++;
    BUART_rxCount++;
  }
}

// Init BLE and start advertising
uint8_t BUART_init(void) {
  if(!BLE_init()) return 0;
  BLE_addService128(BUART_UUID_SERVICE);
  BLE_addChar128(BUART_UUID_RX, BLE_WRITE | BLE_WRITE_NORESP, BUART_rxChar, 0,
                 sizeof(BUART_rxChar), BUART_received);
  BUART_txHandle = BLE_addChar128(BUART_UUID_TX, BLE_NOTIFY, BUART_txChar, 0,
                                  sizeof(BUART_txChar), 0);
  return BLE_advertiseName(BLE_NAME, BUART_UUID_SERVICE);
}

// Check if a device is connected and listening
uint8_t BUART_connected(void) {
  BLE_poll();
  return BLE_notifyEnabled(BUART_txHandle);
}

// Check number of bytes in receive buffer
uint16_t BUART_available(void) {
  BLE_poll();
  return BUART_rxCount;
}

// Read single character from receive buffer (waits until available)
char BUART_read(void) {
  while(!BUART_available());
  char c = BUART_rxBuf[BUART_rxHead];
  BUART_rxHead = (BUART_rxHead + 1) % BUART_RXBUF;
  BUART_rxCount--;
  return c;
}

// Flush transmit buffer (send notification)
void BUART_flush(void) {
  if(BUART_txLen) BLE_notify(BUART_txHandle, BUART_txBuf, BUART_txLen);
  BUART_txLen = 0;
}

// Write single character to transmit buffer
void BUART_write(char c) {
  BUART_txBuf[BUART_txLen++] = c;
  if(BUART_txLen >= BLE_getMTU() - 3) BUART_flush();
}
