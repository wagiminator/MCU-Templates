// ===================================================================================
// Simple Command Line Shell                                                  * v1.0 *
// ===================================================================================
//
// A small, interface-independent command interpreter, which is used by the USB shell
// and the RTC clock examples. Characters are fed in with SHELL_input(), the
// answers are sent via the output function given to SHELL_init().
//
// Functions available:
// --------------------
// SHELL_init(out,flush,echo,p)   init shell with output function, flush function,
//                                echo (1: echo input characters, e.g. for terminal
//                                programs) and prompt string (e.g. "usb> ")
// SHELL_input(c)                 process received character
// SHELL_prompt()                 print prompt
// SHELL_busy()                   check if a command is being typed (line not empty)
// SHELL_command(cmd, arg)        optional user function for additional commands,
//                                returns 1 if command was handled (weak)
//
// Built-in commands:
// ------------------
// help                 list commands
// info                 show board information
// led [on|off|toggle]  switch LED or show LED state
// blink [n]            blink LED n times (default 3, max 20)
// temp                 read internal temperature sensor
// vdd                  read supply voltage VDD
// keys                 read state of the keys (BOOT and KEY)
// uptime               show time since start
// adc [pin]            read analog input (pin = a4, a5, a12..a15, default a4)
// touch [pin]          read TouchKey input (same pins as adc)
// random               get random number
// reset                restart the microcontroller (software reset)
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define SHELL_LINE_LEN    64            // max length of command line

void    SHELL_init(void (*out)(char), void (*flush)(void), uint8_t echo, const char* prompt);
void    SHELL_input(char c);
void    SHELL_prompt(void);
uint8_t SHELL_busy(void);
uint8_t SHELL_command(const char* cmd, const char* arg);  // optional user function

#ifdef __cplusplus
};
#endif
