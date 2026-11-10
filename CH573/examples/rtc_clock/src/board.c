// ===================================================================================
// Board Support for CH573F Boards                                            * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "board.h"

static uint8_t  LED_on_state = 0;   // current LED state
static uint32_t LED_off_time = 0;   // time to switch off LED after flash (0: none)

typedef struct {
  uint8_t  last;                    // last debounced key state
  uint8_t  state;                   // state of the event state machine
  uint32_t time;                    // time of last reading
  uint32_t start;                   // time of the last press / release
} BTN_t;
static BTN_t BTN[2];

// ===================================================================================
// Board Functions
// ===================================================================================

// Init on-board hardware
void BOARD_init(void) {
  PIN_output(PIN_LED);              // LED pin as output
  LED_off();                        // LED off
  PIN_input_PU(PIN_BOOT);           // keys with pull-up
  PIN_input_PU(PIN_KEY);
  MIL_init();                       // millis counter (SysTick)
  ADC_init();                       // ADC for temperature sensor and VDD
}

// Get reason of last reset as string
const char* BOARD_resetReason(void) {
  switch(RST_getFlags()) {
    case RST_FLAG_SW:    return "software";
    case RST_FLAG_RPOR:  return "power on";
    case RST_FLAG_WTR:   return "watchdog";
    case RST_FLAG_MR:    return "RST pin";
    case RST_FLAG_GPWSM: return "wake-up from shutdown";
    default:             return "power on during sleep";
  }
}

// Print board information
void BOARD_printInfo(void (*out)(char)) {
  uint8_t buf[8], i;
  printF(out, "Board:       %s\n", BOARD_NAME);
  printF(out, "Chip:        CH5%02x (chip ID 0x%02x)\n", ESIG_CHIPID & 0xFF, ESIG_CHIPID);
  printF(out, "CPU clock:   %d MHz\n", F_CPU / 1000000);
  ESIG_getMAC(buf);
  printF(out, "MAC address: ");
  for(i = 6; i; i--) printF(out, (i > 1) ? "%02x:" : "%02x\n", buf[i - 1]);
  ESIG_getUID(buf);
  printF(out, "Unique ID:   ");
  for(i = 0; i < 8; i++) printF(out, "%02x", buf[i]);
  printF(out, "\nLast reset:  %s\n", BOARD_resetReason());
}

// Read chip temperature in 1/10 degrees Celsius (factory calibration, 1 C resolution)
int16_t BOARD_temp(void) {
  int32_t sum = 0;
  for(uint8_t i = 0; i < 8; i++) sum += ADC_read_TEMP_raw();
  uint32_t c25  = ESIG_TEMP25;
  int16_t  tref = (c25 >> 16) ? (int16_t)(c25 >> 16) : 25;
  return tref * 10 + (sum - 8 * (int32_t)(c25 & 0xFFFF)) * 100 / (27 * 8);
}

// Read supply voltage VDD in mV
uint16_t BOARD_vdd(void) {
  uint32_t sum = 0;
  for(uint8_t i = 0; i < 4; i++) sum += ADC_read_VDD();
  return sum / 4;
}

// Get 32-bit random number (noise of the temperature sensor and SysTick)
uint32_t BOARD_random(void) {
  uint32_t r = 0;
  for(uint8_t i = 0; i < 32; i++) r = (r << 1) ^ (r >> 31) ^ (ADC_read_TEMP_raw() & 1);
  return r ^ STK->CNTL;
}

// ===================================================================================
// LED Functions
// ===================================================================================

// Set LED state
void LED_set(uint8_t on) {
  LED_on_state = on ? 1 : 0;
  LED_off_time = 0;
  PIN_write(PIN_LED, LED_on_state ^ LED_ACTIVE_LOW);
}

// Get LED state
uint8_t LED_state(void) {
  return LED_on_state;
}

// Switch LED on for n milliseconds (switched off by LED_update)
void LED_flash(uint16_t ms) {
  LED_on();
  LED_off_time = BOARD_millis() + ms;
  if(!LED_off_time) LED_off_time = 1;
}

// Switch LED off after flash time
void LED_update(void) {
  if(LED_off_time && ((int32_t)(BOARD_millis() - LED_off_time) >= 0)) LED_off();
}

// ===================================================================================
// Key Functions
// ===================================================================================

// Read key (pressed = low)
uint8_t BTN_read(uint8_t k) {
  return k ? !PIN_read(PIN_KEY) : !PIN_read(PIN_BOOT);
}

// Check if key was pressed (debounced, returns 1 once per press)
uint8_t BTN_pressed(uint8_t k) {
  BTN_t*   b   = &BTN[k & 1];
  uint32_t now = BOARD_millis();
  uint8_t  state;
  if(now - b->time < 20) return 0;  // debounce, also limits the reading rate
  state = BTN_read(k);
  if(state != b->last) {
    b->last = state;
    b->time = now;
    return state;
  }
  return 0;
}

// Get key event (short, long or double press)
uint8_t BTN_event(uint8_t k, uint8_t dbl) {
  BTN_t*   b   = &BTN[k & 1];
  uint32_t now = BOARD_millis();
  uint8_t  state;
  if(now - b->time < 20) return BTN_NONE;
  b->time = now;
  state = BTN_read(k);
  switch(b->state) {
    case 0:                         // idle: wait for press
      if(state) { b->state = 1; b->start = now; }
      break;
    case 1:                         // pressed: wait for release or long press
      if(!state) {
        if(!dbl) { b->state = 0; return BTN_SHORT; }
        b->state = 2; b->start = now;
      }
      else if(now - b->start >= BTN_LONG_MS) { b->state = 3; return BTN_LONG; }
      break;
    case 2:                         // released: wait for second press
      if(state) { b->state = 3; return BTN_DOUBLE; }
      if(now - b->start >= BTN_DOUBLE_MS) { b->state = 0; return BTN_SHORT; }
      break;
    default:                        // wait for release after long/double press
      if(!state) b->state = 0;
      break;
  }
  return BTN_NONE;
}

// ===================================================================================
// Minimal C Library Functions (weak, the templates are linked without libc)
// ===================================================================================
#define NOLIB __attribute__((weak, used, optimize("no-tree-loop-distribute-patterns")))

NOLIB int strcmp(const char* a, const char* b) {
  while(*a && (*a == *b)) {a++; b++;}
  return (uint8_t)*a - (uint8_t)*b;
}

NOLIB int strncmp(const char* a, const char* b, size_t n) {
  for(; n; n--, a++, b++) {
    if(*a != *b) return (uint8_t)*a - (uint8_t)*b;
    if(!*a) break;
  }
  return 0;
}

NOLIB size_t strlen(const char* s) {
  size_t n = 0;
  while(*s++) n++;
  return n;
}

NOLIB void* memcpy(void* dst, const void* src, size_t n) {
  uint8_t* d = (uint8_t*)dst; const uint8_t* s = (const uint8_t*)src;
  while(n--) *d++ = *s++;
  return dst;
}

NOLIB void* memset(void* dst, int c, size_t n) {
  uint8_t* d = (uint8_t*)dst;
  while(n--) *d++ = (uint8_t)c;
  return dst;
}

NOLIB int memcmp(const void* a, const void* b, size_t n) {
  const uint8_t* x = (const uint8_t*)a; const uint8_t* y = (const uint8_t*)b;
  for(; n; n--, x++, y++) if(*x != *y) return *x - *y;
  return 0;
}
