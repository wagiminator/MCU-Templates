// ===================================================================================
// Basic DHT11/DHT22 (AM2302) Temperature and Humidity Sensor Functions       * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "dht.h"

// DHT variables
int16_t  DHT_temp;                    // temperature in 0.1°C
uint16_t DHT_hum;                     // humidity in 0.1%RH

// Wait while pin has the given level, returns duration in ~2us steps, 0 on timeout
static uint8_t DHT_wait(uint8_t level) {
  uint8_t cnt = 1;
  while((PIN_read(DHT_PIN) ? 1 : 0) == level) {
    if(++cnt == 0) return 0;                      // timeout (~500us)
    DLY_us(2);
  }
  return cnt;
}

// Init data pin
void DHT_init(void) {
  PIN_input(DHT_PIN);                             // release line (pull-up -> high)
}

// Read sensor, returns 0 on success
uint8_t DHT_read(void) {
  uint8_t i, low, high, buf[5] = {0, 0, 0, 0, 0};
  uint8_t err = 1;

  PIN_low(DHT_PIN);                               // start signal: pull line low
  PIN_output(DHT_PIN);
  DLY_ms(20);
  #if DHT_INT_LOCK > 0
  INT_disable();
  #endif
  PIN_input(DHT_PIN);                             // release line

  if(DHT_wait(1) && DHT_wait(0) && DHT_wait(1)) { // response: low 80us, high 80us
    for(i = 0; i < 40; i++) {                     // read 40 bits
      low  = DHT_wait(0);                         // 50us low (reference)
      high = DHT_wait(1);                         // 26-28us high: 0, 70us high: 1
      if(!low || !high) break;
      buf[i >> 3] <<= 1;
      if(high > low) buf[i >> 3] |= 1;            // high longer than low -> 1
    }
    if(i == 40 && (uint8_t)(buf[0] + buf[1] + buf[2] + buf[3]) == buf[4]) err = 0;
  }

  #if DHT_INT_LOCK > 0
  INT_enable();
  #endif
  if(err) return 1;

  #if DHT_TYPE == 11
  DHT_hum  = (uint16_t)buf[0] * 10 + (buf[1] % 10);
  DHT_temp = (int16_t)(buf[2] & 0x7F) * 10 + (buf[3] % 10);
  if(buf[2] & 0x80) DHT_temp = -DHT_temp;
  #else
  DHT_hum  = ((uint16_t)buf[0] << 8) | buf[1];
  DHT_temp = (int16_t)((((uint16_t)buf[2] & 0x7F) << 8) | buf[3]);
  if(buf[2] & 0x80) DHT_temp = -DHT_temp;
  #endif
  return 0;
}
