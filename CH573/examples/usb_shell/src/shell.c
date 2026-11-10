// ===================================================================================
// Simple Command Line Shell                                                  * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "shell.h"
#include "board.h"

static void (*SHELL_out)(char);         // output function
static void (*SHELL_flush)(void);       // flush function
static const char* SHELL_promptStr;     // prompt string
static uint8_t SHELL_echo;              // 1: echo input
static char    SHELL_line[SHELL_LINE_LEN + 1];
static uint8_t SHELL_len;
static char    SHELL_lastEOL;           // last end-of-line character

// Optional user function for additional commands (weak default)
__attribute__((weak)) uint8_t SHELL_command(const char* cmd, const char* arg) {
  (void)cmd; (void)arg;
  return 0;
}

// Print formatted string via output function and flush
#define SHELL_printf(f, ...)  (printF(SHELL_out, f, ##__VA_ARGS__), SHELL_flush())

// Init shell
void SHELL_init(void (*out)(char), void (*flush)(void), uint8_t echo, const char* prompt) {
  SHELL_out       = out;
  SHELL_flush     = flush;
  SHELL_echo      = echo;
  SHELL_promptStr = prompt;
  SHELL_len       = 0;
  SHELL_lastEOL   = 0;
}

// Print prompt
void SHELL_prompt(void) {
  SHELL_printf("%s", SHELL_promptStr);
}

// Check if a command is being typed
uint8_t SHELL_busy(void) {
  return SHELL_len > 0;
}

// Convert decimal string to number
static uint32_t strtoul_dec(const char* s) {
  uint32_t n = 0;
  while((*s >= '0') && (*s <= '9')) n = n * 10 + (*s++ - '0');
  return n;
}

// Print temperature in 1/10 degrees
static void SHELL_printTemp(int16_t t) {
  if(t < 0) {
    SHELL_out('-');
    t = -t;
  }
  SHELL_printf("%d.%d C\n", t / 10, t % 10);
}

// Execute command line
static void SHELL_execute(char* line) {
  char* cmd = line;
  char* arg;

  // Split command and argument, convert command to lower case
  while(*cmd == ' ') cmd++;
  arg = cmd;
  while(*arg && (*arg != ' ')) {
    if((*arg >= 'A') && (*arg <= 'Z')) *arg += 'a' - 'A';
    arg++;
  }
  if(*arg) *arg++ = 0;
  while(*arg == ' ') arg++;
  if(!*cmd) return;

  // Built-in commands
  if(!strcmp(cmd, "help") || !strcmp(cmd, "?")) {
    SHELL_printf(
      "Commands:\n"
      "  help                 list commands\n"
      "  info                 show board information\n"
      "  led [on|off|toggle]  switch LED or show state\n"
      "  blink [n]            blink LED n times\n"
      "  temp                 read chip temperature\n"
      "  vdd                  read supply voltage VDD\n"
      "  keys                 read keys (BOOT, KEY)\n"
      "  uptime               time since start\n"
      "  adc [pin]            read analog input (a4, a5, a12..a15)\n"
      "  touch [pin]          read TouchKey input (a4, a5, a12..a15)\n"
      "  random               random number\n"
      "  reset                restart\n");
    SHELL_command("help", arg);           // let application list its commands
  }
  else if(!strcmp(cmd, "info")) {
    BOARD_printInfo(SHELL_out);
    SHELL_flush();
    SHELL_command("info", arg);           // let application add information
  }
  else if(!strcmp(cmd, "led")) {
    for(char* p = arg; *p; p++) if((*p >= 'A') && (*p <= 'Z')) *p += 'a' - 'A';
    if     (!strcmp(arg, "on"))     LED_on();
    else if(!strcmp(arg, "off"))    LED_off();
    else if(!strcmp(arg, "toggle")) LED_toggle();
    else if(*arg) {
      SHELL_printf("Usage: led [on|off|toggle]\n");
      return;
    }
    SHELL_printf("LED is %s\n", LED_state() ? "on" : "off");
  }
  else if(!strcmp(cmd, "blink")) {
    uint8_t n = 3;
    if(*arg) n = (uint8_t)strtoul_dec(arg);
    if(n > 20) n = 20;
    SHELL_printf("Blinking %d times ...\n", n);
    while(n--) {
      LED_on();  DLY_ms(150);
      LED_off(); DLY_ms(150);
    }
    SHELL_printf("Done\n");
  }
  else if(!strcmp(cmd, "temp")) {
    SHELL_printf("Chip temperature: ");
    SHELL_printTemp(BOARD_temp());
  }
  else if(!strcmp(cmd, "vdd")) {
    uint16_t mv = BOARD_vdd();
    SHELL_printf("VDD: %d.%02d V\n", mv / 1000, (mv % 1000) / 10);
  }
  else if(!strcmp(cmd, "keys")) {
    SHELL_printf("BOOT key (PB22) is %s, KEY (PB23) is %s\n",
                 BTN_read(BTN_BOOT) ? "pressed" : "released",
                 BTN_read(BTN_KEY)  ? "pressed" : "released");
  }
  else if(!strcmp(cmd, "uptime")) {
    uint32_t s = BOARD_millis() / 1000;
    SHELL_printf("Uptime: %dd %02d:%02d:%02d\n", s / 86400, (s / 3600) % 24, (s / 60) % 60, s % 60);
  }
  else if(!strcmp(cmd, "adc") || !strcmp(cmd, "touch")) {
    uint8_t n = 4, pin;
    if((*arg == 'a') || (*arg == 'A')) arg++;
    if(*arg) n = (uint8_t)strtoul_dec(arg);
    switch(n) {
      case 4:  pin = PA4;  break;
      case 5:  pin = PA5;  break;
      case 12: pin = PA12; break;
      case 13: pin = PA13; break;
      case 14: pin = PA14; break;
      case 15: pin = PA15; break;
      default:
        SHELL_printf("Only PA4, PA5 and PA12..PA15 can be used\n");
        return;
    }
    if(cmd[0] == 'a') {
      ADC_init();
      ADC_input(pin);
      ADC_read();
      int32_t mv = ADC_read_mV();
      SHELL_printf("PA%d (AIN%d): %d mV\n", n, ADC_CHANNEL(pin), (mv < 0) ? 0 : mv);
    }
    else {
      TOUCH_init();
      uint16_t val = TOUCH_read(pin);
      ADC_init();
      SHELL_printf("TouchKey PA%d: %d (lower value when touched)\n", n, val);
    }
  }
  else if(!strcmp(cmd, "random")) {
    SHELL_printf("Random number: 0x%08x\n", BOARD_random());
  }
  else if(!strcmp(cmd, "reset")) {
    SHELL_printf("Restarting ...\n");
    DLY_ms(100);
    RST_now();
  }
  else if(!SHELL_command(cmd, arg)) {
    SHELL_printf("Unknown command '%s', type 'help'\n", cmd);
  }
}

// Process received character
void SHELL_input(char c) {
  // End of line: execute command (CR+LF or LF+CR count as one end of line)
  if((c == '\r') || (c == '\n')) {
    if(SHELL_lastEOL && (c != SHELL_lastEOL) && !SHELL_len) {
      SHELL_lastEOL = 0;
      return;
    }
    SHELL_lastEOL = c;
    if(SHELL_echo) SHELL_out('\n');
    SHELL_line[SHELL_len] = 0;
    SHELL_execute(SHELL_line);
    SHELL_len = 0;
    SHELL_prompt();
    return;
  }
  SHELL_lastEOL = 0;

  // Backspace / delete
  if((c == 0x08) || (c == 0x7F)) {
    if(SHELL_len) {
      SHELL_len--;
      if(SHELL_echo) {
        SHELL_out(0x08); SHELL_out(' '); SHELL_out(0x08);
        SHELL_flush();
      }
    }
    return;
  }

  // Normal character: store in line buffer
  if((c >= ' ') && (SHELL_len < SHELL_LINE_LEN)) {
    SHELL_line[SHELL_len++] = c;
    if(SHELL_echo) {
      SHELL_out(c);
      SHELL_flush();
    }
  }
}
