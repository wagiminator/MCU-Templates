// ===================================================================================
// Basic PIOC Functions for CH32X035/X034/X033 (programmable I/O controller)  * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "pioc.h"
#include "gpio.h"

// Pins
#if PIOC_MAP == 0
  #define PIOC_PIN_IO0    PC18
#elif PIOC_MAP == 1
  #define PIOC_PIN_IO0    PC7
#else
  #error Wrong PIOC_MAP!
#endif
#define PIOC_PIN_IO1      PC19

// Host bits of SYS_CFG
#define PIOC_IO_MASK      (PIOC_IO0 | PIOC_IO1)
#define PIOC_HOST_MASK    (PIOC_MST_CFG_B4 | PIOC_IO_MASK | PIOC_MST_RESET | PIOC_MST_CLK_GATE)

// Init PIOC and connect pins io (PIOC_IO0 and/or PIOC_IO1)
void PIOC_init(uint8_t io) {
  io &= PIOC_IO_MASK;
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN;    // enable AFIO and GPIOC
  PIOC->SYS_CFG = PIOC_MST_RESET;               // stop and reset PIOC
  #if PIOC_MAP == 1
    AFIO->PCFR1 |=  AFIO_PCFR1_PIOC_REMAP;      // IO0 on PC7
  #else
    AFIO->PCFR1 &= ~AFIO_PCFR1_PIOC_REMAP;      // IO0 on PC18
  #endif
  if((io & PIOC_IO1) || ((io & PIOC_IO0) && (PIOC_MAP == 0)))
    AFIO->PCFR1 = (AFIO->PCFR1 & ~AFIO_PCFR1_SWJ_CFG) | AFIO_PCFR1_SWJ_CFG_2; // SWD off
  if(io & PIOC_IO0) {
    PIN_high(PIOC_PIN_IO0);
    PIN_alternate(PIOC_PIN_IO0);                // alternate function push-pull
  }
  if(io & PIOC_IO1) {
    PIN_high(PIOC_PIN_IO1);
    PIN_alternate(PIOC_PIN_IO1);                // alternate function push-pull
  }
  PIOC->SYS_CFG = PIOC_MST_RESET | io;          // connect pins to PIOC
}

// Stop and reset PIOC, load program (len bytes)
void PIOC_load(const uint8_t* prog, uint16_t len) {
  uint16_t i;
  PIOC_reset();                                 // stop and reset PIOC
  if(len > PIOC_RAM_SIZE) len = PIOC_RAM_SIZE;
  for(i=0; i<len; i++) PIOC_RAM[i] = prog[i];   // copy program to code RAM
}

// Copy buffer to PIOC code RAM at byte offset (pauses PIOC)
void PIOC_writeRAM(uint16_t offs, const uint8_t* buf, uint16_t len) {
  uint8_t cfg = PIOC->SYS_CFG & PIOC_HOST_MASK;
  PIOC->SYS_CFG = cfg & ~PIOC_MST_CLK_GATE;     // pause PIOC
  while(len-- && (offs < PIOC_RAM_SIZE)) PIOC_RAM[offs++] = *buf++;
  PIOC->SYS_CFG = cfg;                          // continue PIOC
}

// Copy PIOC code RAM at byte offset to buffer (pauses PIOC)
void PIOC_readRAM(uint16_t offs, uint8_t* buf, uint16_t len) {
  uint8_t cfg = PIOC->SYS_CFG & PIOC_HOST_MASK;
  PIOC->SYS_CFG = cfg & ~PIOC_MST_CLK_GATE;     // pause PIOC
  while(len-- && (offs < PIOC_RAM_SIZE)) *buf++ = PIOC_RAM[offs++];
  PIOC->SYS_CFG = cfg;                          // continue PIOC
}

// Write byte to PIOC (CTRL_WR), wait until PIOC has read the previous one
void PIOC_write(uint8_t c) {
  while(!PIOC_ready());                         // wait for PIOC to read last byte
  PIOC->CTRL_WR = c;                            // write byte, sets DATA_MW_SR
}

// Read byte from PIOC (CTRL_RD), wait if necessary
uint8_t PIOC_read(void) {
  while(!PIOC_available());                     // wait for PIOC to write a byte
  return PIOC->CTRL_RD;                         // read byte, clears DATA_SW_MR
}

// Write command byte and wait for the answer byte
uint8_t PIOC_command(uint8_t c) {
  if(PIOC_available()) (void)PIOC->CTRL_RD;     // discard old answer
  PIOC_write(c);                                // write command
  return PIOC_read();                           // wait for answer and return it
}

// Interrupt service routine
__attribute__((weak)) void PIOC_callback(void) {}
void PIOC_IRQHandler(void) __attribute__((interrupt));
void PIOC_IRQHandler(void) {
  PIOC_INT_clear();                             // clear interrupt request
  PIOC_callback();                              // call user function (PIOC_ISR)
}
