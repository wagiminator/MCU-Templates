// ===================================================================================
// Project:   Raspberry Pi Pico Examples - RTC Clock with Alarm
// Version:   v1.0
// Year:      2026
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// A clock with alarm based on the real-time clock (RTC peripheral of the RP2040). The time
// is set and shown via the terminal (USB serial port). The LED flashes briefly
// every second (heartbeat). When the alarm time is reached, the LED fades in and
// out (PWM) and a message is sent every 10 seconds, until the BOOTSEL button is
// pressed or "alarm off" is typed. Between the clock updates, the CPU sleeps.
//
// Commands (in addition to the commands of the USB shell, type "help"):
//
//   time [hh:mm:ss]          show or set time
//   date [yyyy-mm-dd]        show or set date (the weekday is calculated)
//   alarm [hh:mm[:ss]|off]   show, set or switch off the daily alarm
//   clock [on|off]           show the time every second in the terminal (in front
//                            of the prompt, paused while a command is typed)
//
// Tested template functions: RTC (time, date, weekday, alarm interrupt), sleep
// (SLEEP_ms), PWM, USB device (CDC), BOOTSEL button, shell.
//
// Settings in config.h: CLOCK_TIME, CLOCK_DATE (start time if the RTC is not
// running), ALARM_REPEAT, PIN_LED, USB descriptors.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <string.h>
#include "board.h"                        // board support (LED, button, ADC, info)
#include "term.h"                         // terminal via USB serial port
#include "shell.h"                        // command line interpreter
#include "pwm.h"                          // PWM (LED fading)

const char* WEEKDAYS[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

uint8_t  alarmSet  = 0;                   // alarm active?
uint8_t  alarmH, alarmM, alarmS;          // alarm time
volatile uint8_t alarmFlag = 0;           // set by alarm interrupt
uint8_t  ringing   = 0;                   // alarm is ringing
uint8_t  showClock = 1;                   // show time every second

// ===================================================================================
// RTC Functions
// ===================================================================================

// Alarm interrupt
void RTC_IRQ_IRQHandler(void) {
  RTC_clearAlarmFlag();                   // clear flag (stops the alarm)
  alarmFlag = 1;
}

// Print date and time
void printDateTime(void (*out)(char)) {
  uint32_t t = RTC_readTime();            // must be read before the date
  uint32_t d = RTC_readDate();
  printF(out, "%s %04d-%02d-%02d %02d:%02d:%02d", WEEKDAYS[RTC_getWeekday(t)],
         RTC_getYear(d), RTC_getMonth(d), RTC_getDay(d),
         RTC_getHours(t), RTC_getMinutes(t), RTC_getSeconds(t));
}

// Parse "a:b[:c]" or "a-b-c" into three numbers, returns number of values found
uint8_t parse3(const char* s, uint16_t* v) {
  uint8_t n = 0;
  v[0] = v[1] = v[2] = 0;
  while(*s && (n < 3)) {
    if((*s < '0') || (*s > '9')) return 0;
    while((*s >= '0') && (*s <= '9')) v[n] = v[n] * 10 + (*s++ - '0');
    n++;
    if((*s == ':') || (*s == '-')) s++;
    else break;
  }
  return n;
}

// Set alarm
void setAlarm(void) {
  RTC_setAlarm(alarmH, alarmM, alarmS);
  RTC_enableAlarmInt();
}

// ===================================================================================
// Shell Extension (additional commands)
// ===================================================================================
uint8_t SHELL_command(const char* cmd, const char* arg) {
  uint16_t v[3];
  if(!strcmp(cmd, "help")) {
    TERM_print("  time [hh:mm:ss]      show or set time\n"
               "  date [yyyy-mm-dd]    show or set date\n"
               "  alarm [hh:mm|off]    show, set or switch off the daily alarm\n"
               "  clock [on|off]       show the time every second\n");
    return 1;
  }
  if(!strcmp(cmd, "time") || !strcmp(cmd, "date")) {
    if(*arg) {
      if(cmd[0] == 't') {
        if((parse3(arg, v) < 2) || (v[0] > 23) || (v[1] > 59) || (v[2] > 59)) {
          TERM_print("Usage: time hh:mm:ss\n");
          return 1;
        }
        RTC_setTime(v[0], v[1], v[2]);
      }
      else {
        if((parse3(arg, v) != 3) || (v[0] < RTC_YEAR_REF) || (v[0] > RTC_YEAR_REF + 99)
           || (v[1] < 1) || (v[1] > 12) || (v[2] < 1) || (v[2] > 31)) {
          TERM_print("Usage: date yyyy-mm-dd\n");
          return 1;
        }
        RTC_setDate(v[0] - RTC_YEAR_REF, v[1], v[2]);
      }
      if(alarmSet) setAlarm();            // recalculate alarm
      DLY_ms(5);                          // wait until the RTC has taken the values
    }
    printDateTime(TERM_putchar);
    TERM_print("\n");
    return 1;
  }
  if(!strcmp(cmd, "alarm")) {
    if(!strcmp(arg, "off")) {
      alarmSet = 0;
      ringing  = 0;
      RTC_stopAlarm();
    }
    else if(*arg) {
      if((parse3(arg, v) < 2) || (v[0] > 23) || (v[1] > 59) || (v[2] > 59)) {
        TERM_print("Usage: alarm hh:mm[:ss] or alarm off\n");
        return 1;
      }
      alarmH = v[0]; alarmM = v[1]; alarmS = v[2];
      alarmSet = 1;
      setAlarm();
    }
    if(alarmSet) TERM_printf("Alarm at %02d:%02d:%02d\n", alarmH, alarmM, alarmS);
    else         TERM_print("Alarm is off\n");
    return 1;
  }
  if(!strcmp(cmd, "clock")) {
    if(!strcmp(arg, "on"))  showClock = 1;
    if(!strcmp(arg, "off")) showClock = 0;
    TERM_printf("Clock display is %s\n", showClock ? "on" : "off");
    return 1;
  }
  if(!strcmp(cmd, "info")) {
    TERM_print("Date/time:   ");
    printDateTime(TERM_putchar);
    TERM_print("\n");
    return 1;
  }
  return 0;
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  uint8_t  connected = 0;
  uint8_t  lastSec = 0xFF, sec;
  uint32_t ringTime = 0;
  uint16_t fade = 0;
  uint16_t v[3];

  // Setup
  BOARD_init();                           // init LED, millis, ADC
  TERM_init();                            // init USB serial port
  SHELL_init(TERM_putchar, TERM_flush, 1, "clock> ");
  PWM_init();                             // PWM for LED fading
  if(!RTC_isInit()) {                     // start RTC with default time
    RTC_init();
    parse3(CLOCK_DATE, v); RTC_setDate(v[0] - RTC_YEAR_REF, v[1], v[2]);
    parse3(CLOCK_TIME, v); RTC_setTime(v[0], v[1], v[2]);
  }

  // Loop
  while(1) {
    // Terminal opened: show welcome message
    if(TERM_connected() != connected) {
      connected = TERM_connected();
      if(connected) {
        DLY_ms(100);
        TERM_print("\n=== Raspberry Pi Pico - RTC Clock ===\n");
        TERM_print("Set the clock with 'time hh:mm:ss' and 'date yyyy-mm-dd',\n");
        TERM_print("the alarm with 'alarm hh:mm'. Type 'help' for all commands.\n");
        SHELL_prompt();
      }
    }

    // Characters received via USB: feed into shell
    while(TERM_available()) SHELL_input(TERM_read());

    // Alarm interrupt: start ringing
    if(alarmFlag) {
      alarmFlag = 0;
      ringing   = 1;
      ringTime  = BOARD_millis() - 10000;
      PWM_start(PIN_LED);
    }

    // Ringing: fade LED, message every 10 seconds, stop with BOOTSEL button
    if(ringing) {
      PWM_write(PIN_LED, (fade < 256) ? fade : 511 - fade);
      fade = (fade + 4) & 511;
      if(BOARD_millis() - ringTime >= 10000) {
        ringTime = BOARD_millis();
        TERM_print("\n*** ALARM *** ");
        printDateTime(TERM_putchar);
        TERM_print(" - press BOOTSEL or type 'alarm off'\n");
        SHELL_prompt();
      }
      if(BTN_pressed() || !alarmSet) {
        ringing = 0;
        TERM_print("\nAlarm stopped\n");
        SHELL_prompt();
      }
      if(!ringing) {
        PWM_stop(PIN_LED);
        LED_off();
        if(alarmSet) setAlarm();          // set alarm again for the next day
      }
      DLY_ms(5);
      continue;
    }

    // New second: LED heartbeat and clock display
    sec = RTC_getSeconds(RTC_readTime());
    (void)RTC_readDate();
    if(sec != lastSec) {
      lastSec = sec;
      LED_flash(20);
      if(showClock && connected && !SHELL_busy()) {
        TERM_print("\r");                 // overwrite the line with the prompt
        printDateTime(TERM_putchar);
        TERM_print("  ");
        SHELL_prompt();
      }
    }
    LED_update();
    if(BTN_pressed()) {
      TERM_print("\n");
      printDateTime(TERM_putchar);
      TERM_print("\n");
      SHELL_prompt();
    }
    SLEEP_ms(10);                         // sleep until the next check
  }
}
