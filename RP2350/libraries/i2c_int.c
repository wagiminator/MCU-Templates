// ===================================================================================
// I2C Master Functions with Interrupt for RP2350 (I2C0/I2C1)                 * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "i2c_int.h"
#include "gpio.h"

// Transfer state
static uint16_t I2C_pending;      // held back data byte (bit 15: valid)
static uint8_t  I2C_addr;         // current target address (with R/W bit)
static uint8_t  I2C_restart;      // next command needs a repeated start
static uint8_t  I2C_sent;         // something was sent in this transaction
static uint8_t  I2C_abort;        // transfer was aborted (NACK)

#define I2C_PENDING     0x8000

// Interrupt driven buffer transfer
volatile uint8_t I2C_INT_busy = 0;                // buffer transfer ongoing flag
static const uint8_t* volatile I2C_INT_ptr;       // pointer to next byte
static volatile uint16_t I2C_INT_len;             // remaining bytes

// Check for abort (NACK), clear abort flag
static uint8_t I2C_checkAbort(void) {
  if(I2C_DEV->IC_RAW_INTR_STAT & I2C_IC_RAW_INTR_STAT_TX_ABRT) {
    (void)I2C_DEV->IC_CLR_TX_ABRT;                // clear abort
    I2C_abort = 1;
    I2C_pending = 0;
  }
  return I2C_abort;
}

// Push command into TX FIFO
static void I2C_command(uint32_t cmd) {
  if(I2C_restart) {
    cmd |= I2C_IC_DATA_CMD_RESTART;               // repeated start before this byte
    I2C_restart = 0;
  }
  while(!(I2C_DEV->IC_STATUS & I2C_IC_STATUS_TFNF)) {  // wait for space in TX FIFO
    if(I2C_checkAbort()) return;
  }
  I2C_DEV->IC_DATA_CMD = cmd;
  I2C_sent = 1;
}

// Send held back data byte (with or without STOP)
static void I2C_flush(uint32_t stop) {
  if(I2C_pending & I2C_PENDING) {
    I2C_pending &= 0xff;
    I2C_command(I2C_pending | stop);
    I2C_pending = 0;
  }
}

// Wait for end of transmission (STOP detected or abort)
static void I2C_waitStop(void) {
  while(!(I2C_DEV->IC_RAW_INTR_STAT & I2C_IC_RAW_INTR_STAT_STOP_DET)) {
    if(I2C_checkAbort()) break;
  }
  (void)I2C_DEV->IC_CLR_STOP_DET;                 // clear stop detection
  I2C_checkAbort();
}

// I2C init function
void I2C_init(void) {
  uint32_t period, lcnt, hcnt, hold;

  // Reset and setup I2C module (master, fast-mode, repeated start)
  RESET_cycle(I2C_RESET);
  I2C_DEV->IC_ENABLE = 0;
  I2C_DEV->IC_CON    = I2C_IC_CON_MASTER_MODE | I2C_IC_CON_IC_SLAVE_DISABLE
                     | I2C_IC_CON_IC_RESTART_EN | I2C_IC_CON_TX_EMPTY_CTRL
                     | ((uint32_t)2 << I2C_IC_CON_SPEED_Pos);
  I2C_DEV->IC_TX_TL  = 0;
  I2C_DEV->IC_INTR_MASK = 0;                      // interrupts disabled
  I2C_DEV->IC_RX_TL  = 0;

  // Set clock rate (SCL low: 60%, high: 40% of period)
  period = (F_PERI + I2C_CLKRATE / 2) / I2C_CLKRATE;
  lcnt   = period * 3 / 5;
  hcnt   = period - lcnt;
  I2C_DEV->IC_FS_SCL_HCNT = hcnt;
  I2C_DEV->IC_FS_SCL_LCNT = lcnt;
  I2C_DEV->IC_FS_SPKLEN   = (lcnt < 16) ? 1 : lcnt / 16;
  hold = (I2C_CLKRATE < 1000000) ? (F_PERI * 3) / 10000000 + 1  // SDA hold: 300ns
                                 : (F_PERI * 3) / 25000000 + 1; //           120ns
  I2C_DEV->IC_SDA_HOLD = (I2C_DEV->IC_SDA_HOLD & ~I2C_IC_SDA_HOLD_IC_SDA_TX_HOLD_Msk) | hold;
  I2C_DEV->IC_ENABLE = 1;

  // Setup pins (pull-ups enabled, external pull-ups are still required)
  PIN_alternate(I2C_SDA_PIN, GPIO_FUNC_I2C);
  PIN_alternate(I2C_SCL_PIN, GPIO_FUNC_I2C);
  PIN_pullup(I2C_SDA_PIN);
  PIN_pullup(I2C_SCL_PIN);
}

// Start I2C transmission (addr must contain R/W bit), or repeated start
void I2C_start(uint8_t addr) {
  while(I2C_INT_busy);                            // wait for buffer transfer
  if((I2C_sent || (I2C_pending & I2C_PENDING)) && !I2C_abort) { // repeated start
    I2C_flush(0);                                 // send held back byte
    I2C_restart = 1;
    I2C_addr = addr;
    return;
  }
  I2C_DEV->IC_ENABLE = 0;                         // target can only be set if disabled
  while(I2C_DEV->IC_ENABLE_STATUS & I2C_IC_ENABLE_STATUS_IC_EN);
  I2C_DEV->IC_TAR    = addr >> 1;                 // 7-bit target address
  I2C_DEV->IC_ENABLE = 1;
  (void)I2C_DEV->IC_CLR_TX_ABRT;                  // clear flags
  (void)I2C_DEV->IC_CLR_STOP_DET;
  I2C_addr    = addr;
  I2C_pending = 0;
  I2C_restart = 0;
  I2C_sent    = 0;
  I2C_abort   = 0;
}

// Stop I2C transmission
void I2C_stop(void) {
  while(I2C_INT_busy);                            // wait for buffer transfer
  if(!I2C_abort) {
    if(I2C_pending & I2C_PENDING) I2C_flush(I2C_IC_DATA_CMD_STOP);  // last byte + STOP
    else if(!I2C_sent) {                          // address only: read dummy byte
      I2C_command(I2C_IC_DATA_CMD_CMD | I2C_IC_DATA_CMD_STOP);
    }
    I2C_waitStop();
  }
  while(I2C_DEV->IC_RXFLR) (void)I2C_DEV->IC_DATA_CMD;  // discard remaining data
  I2C_sent = 0;
}

// Send data byte via I2C bus (held back until next write or stop)
void I2C_write(uint8_t data) {
  while(I2C_INT_busy);                            // wait for buffer transfer
  if(I2C_checkAbort()) return;
  I2C_flush(0);
  I2C_pending = I2C_PENDING | data;
}

// Read data byte from I2C bus (ack=0: last byte, STOP is sent)
uint8_t I2C_read(uint8_t ack) {
  while(I2C_INT_busy);                            // wait for buffer transfer
  if(I2C_checkAbort()) return 0xff;
  I2C_flush(0);
  I2C_command(I2C_IC_DATA_CMD_CMD | (ack ? 0 : I2C_IC_DATA_CMD_STOP));
  while(!I2C_DEV->IC_RXFLR) {                     // wait for data
    if(I2C_checkAbort()) return 0xff;
  }
  return I2C_DEV->IC_DATA_CMD;
}

// Check if the last address or data byte was not acknowledged
uint8_t I2C_nack(void) {
  while(I2C_INT_busy);                            // wait for buffer transfer
  if(I2C_pending & I2C_PENDING) return I2C_abort;
  return I2C_checkAbort();
}

// Send data buffer via I2C bus and stop (interrupt driven, returns immediately)
void I2C_writeBuffer(uint8_t* buf, uint16_t len) {
  while(I2C_INT_busy);                            // wait for previous transfer
  if(I2C_checkAbort()) return;                    // transfer aborted (NACK)?
  if(!len) {                                      // nothing to send?
    I2C_stop();                                   // -> just stop
    return;
  }
  I2C_flush(0);                                   // send held back byte
  if(I2C_abort) return;
  I2C_INT_ptr  = buf;                             // set buffer pointer
  I2C_INT_len  = len;                             // set number of bytes
  I2C_INT_busy = 1;                               // set busy flag
  I2C_sent     = 1;
  (void)I2C_DEV->IC_CLR_STOP_DET;                 // clear stop detection
  I2C_DEV->IC_INTR_MASK = I2C_IC_INTR_MASK_M_TX_EMPTY // TX FIFO below threshold
                        | I2C_IC_INTR_MASK_M_TX_ABRT  // abort (NACK)
                        | I2C_IC_INTR_MASK_M_STOP_DET;// STOP detected
  NVIC_EnableIRQ(I2C_IRQn);                       // enable interrupt
}

// I2C interrupt service routine (fill TX FIFO, end of transfer)
static void I2C_ISR(void) {
  uint32_t stat = I2C_DEV->IC_INTR_STAT;
  if(stat & I2C_IC_INTR_STAT_R_TX_ABRT) {         // NACK -> transfer aborted
    (void)I2C_DEV->IC_CLR_TX_ABRT;
    I2C_abort   = 1;
    I2C_INT_len = 0;
  }
  while(I2C_INT_len && (I2C_DEV->IC_STATUS & I2C_IC_STATUS_TFNF)) {
    uint32_t cmd = *I2C_INT_ptr++;                // next data byte
    if(I2C_restart) {                             // repeated start before this byte
      cmd |= I2C_IC_DATA_CMD_RESTART;
      I2C_restart = 0;
    }
    if(!--I2C_INT_len) cmd |= I2C_IC_DATA_CMD_STOP; // last byte with STOP
    I2C_DEV->IC_DATA_CMD = cmd;
  }
  if(!I2C_INT_len) I2C_DEV->IC_INTR_MASK &= ~I2C_IC_INTR_MASK_M_TX_EMPTY;
  if(stat & I2C_IC_INTR_STAT_R_STOP_DET) {        // STOP detected -> finished
    (void)I2C_DEV->IC_CLR_STOP_DET;
    if(!I2C_INT_len) {
      I2C_DEV->IC_INTR_MASK = 0;                  // disable interrupts
      I2C_sent     = 0;
      I2C_INT_busy = 0;                           // transfer finished
    }
  }
}
void I2C0_IRQ_IRQHandler(void) { if(I2C_NUM == 0) I2C_ISR(); }
void I2C1_IRQ_IRQHandler(void) { if(I2C_NUM == 1) I2C_ISR(); }

// Read data via I2C bus to buffer and stop
void I2C_readBuffer(uint8_t* buf, uint16_t len) {
  while(len--) *buf++ = I2C_read(len > 0);        // read buffer
  I2C_stop();                                     // stop transmission
}
