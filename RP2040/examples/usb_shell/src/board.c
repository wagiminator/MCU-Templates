// ===================================================================================
// Board Support for Raspberry Pi Pico                                        * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "board.h"
#if __has_include("flash.h")
#include "flash.h"                  // unique ID of the flash chip (if flash lib is used)
#define BOARD_FLASH_UID   1
#endif

static uint8_t  LED_on_state = 0;   // current LED state
static uint32_t LED_off_time = 0;   // time to switch off LED after flash (0: none)
static uint8_t  BTN_last     = 0;   // last debounced button state
static uint32_t BTN_time     = 0;   // time of last button state change
static uint8_t  BTN_state    = 0;   // state of the event state machine
static uint32_t BTN_start    = 0;   // time of the last press / release

// ===================================================================================
// Board Functions
// ===================================================================================

// Init on-board hardware
void BOARD_init(void) {
  PIN_output(PIN_LED);              // LED pin as output
  LED_off();                        // LED off
  MIL_init();                       // millis counter (TIMER)
  ADC_init();                       // ADC for temperature sensor and VSYS
  PIN_input_AN(GP29);               // VSYS/3 on GP29
}

// Get reason of last reset as string
const char* BOARD_resetReason(void) {
  if(RST_wasIWDG())     return "watchdog";
  if(RST_wasSoftware()) return "software";
  if(RST_wasPin())      return "RUN pin";
  if(RST_wasPower())    return "power on";
  return "other (e.g. debugger or bootloader)";
}

// Print board information
void BOARD_printInfo(void (*out)(char)) {
  printF(out, "Board:       %s\n", BOARD_NAME);
  printF(out, "Chip:        RP2040-B%d, boot ROM v%d\n", ESIG_REVISION, ROM_VERSION);
  printF(out, "CPU clock:   %d MHz\n", F_CPU / 1000000);
  #ifdef BOARD_FLASH_UID
  {
    uint8_t uid[8], i;
    FLASH_getUID(uid);
    printF(out, "Unique ID:   ");
    for(i = 0; i < 8; i++) printF(out, "%02x", uid[i]);
    printF(out, " (flash chip)\nFlash:       JEDEC ID 0x%06x\n", FLASH_getJEDEC());
  }
  #endif
  printF(out, "Last reset:  %s\n", BOARD_resetReason());
}

// Read chip temperature in 1/10 degrees Celsius (27 C at 0.706V, -1.721 mV/C)
int16_t BOARD_temp(void) {
  int32_t uv;
  ADC_input_TEMP();
  ADC_read();                       // first conversion after switching is discarded
  uv = (int32_t)(((uint32_t)ADC_read() * (ADC_VREF * 250)) >> 10);  // voltage in uV
  return (int16_t)(270 - ((uv - 706000) * 10 + (uv < 706000 ? -860 : 860)) / 1721);
}

// Read supply voltage VSYS in mV (GP29 = VSYS/3)
uint16_t BOARD_vsys(void) {
  REG_CLR(ADC->CS, ADC_CS_TS_EN);   // temperature sensor off
  ADC_input(GP29);
  ADC_read();
  return ((uint32_t)ADC_read() * ADC_VREF * 3 + 2048) >> 12;
}

// Get 32-bit random number from the random bit of the ring oscillator
uint32_t BOARD_random(void) {
  uint32_t r = 0;
  uint8_t  i;
  volatile uint8_t d;
  for(i = 0; i < 32; i++) {
    r = (r << 1) | (ROSC->RANDOMBIT & 1);
    for(d = 0; d < 10; d++);        // let the oscillator drift between two samples
  }
  return r ^ (uint32_t)MIC_read();
}

// ===================================================================================
// LED Functions
// ===================================================================================

// Set LED state
void LED_set(uint8_t on) {
  LED_on_state = on ? 1 : 0;
  LED_off_time = 0;
  PIN_write(PIN_LED, LED_on_state);
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
// Button Functions (BOOTSEL button)
// ===================================================================================

// Read BOOTSEL button (runs from SRAM, flash is not accessible during reading)
RAM_FUNC uint8_t BTN_read(void) {
  volatile uint32_t i;
  uint8_t pressed;
  INT_ATOMIC_BLOCK {
    // Disable chip select output of the flash, the button pulls the line low
    IO_QSPI->GPIO_QSPI_SS_CTRL = (IO_QSPI->GPIO_QSPI_SS_CTRL & ~IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_Msk)
                               | IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_DISABLE;
    for(i = 0; i < 1000; i++);      // wait for the line to settle
    pressed = !(SIO->GPIO_HI_IN & (1 << 1));  // bit 1: QSPI_SS
    // Restore normal chip select output
    IO_QSPI->GPIO_QSPI_SS_CTRL = (IO_QSPI->GPIO_QSPI_SS_CTRL & ~IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_Msk)
                               | IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_NORMAL;
  }
  return pressed;
}

// Check if button was pressed (debounced, returns 1 once per press)
uint8_t BTN_pressed(void) {
  uint32_t now = BOARD_millis();
  uint8_t  state;
  if(now - BTN_time < 20) return 0;  // debounce, also limits the reading rate
  state = BTN_read();
  if(state != BTN_last) {
    BTN_last = state;
    BTN_time = now;
    return state;
  }
  return 0;
}

// Get button event (short, long or double press)
uint8_t BTN_event(uint8_t dbl) {
  uint32_t now = BOARD_millis();
  uint8_t  state;
  if(now - BTN_time < 20) return BTN_NONE;
  BTN_time = now;
  state = BTN_read();
  switch(BTN_state) {
    case 0:                         // idle: wait for press
      if(state) { BTN_state = 1; BTN_start = now; }
      break;
    case 1:                         // pressed: wait for release or long press
      if(!state) {
        if(!dbl) { BTN_state = 0; return BTN_SHORT; }
        BTN_state = 2; BTN_start = now;
      }
      else if(now - BTN_start >= BTN_LONG_MS) { BTN_state = 3; return BTN_LONG; }
      break;
    case 2:                         // released: wait for second press
      if(state) { BTN_state = 3; return BTN_DOUBLE; }
      if(now - BTN_start >= BTN_DOUBLE_MS) { BTN_state = 0; return BTN_SHORT; }
      break;
    default:                        // wait for release after long/double press
      if(!state) BTN_state = 0;
      break;
  }
  return BTN_NONE;
}
