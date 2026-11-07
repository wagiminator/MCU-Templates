// ===================================================================================
// Software USB Handler for PY32F0xx (Cortex-M0+ @ 48MHz)                     * v1.0 *
// ===================================================================================
//
// Low-speed USB (1.5Mbit/s) bit-banging for the Cortex-M0+ core of the PY32F0xx at
// 48MHz (32 clock cycles per bit). The code is executed from SRAM (section .RamFunc),
// so that the timing is deterministic (zero wait states, single-cycle IOPORT access
// to the GPIO registers). The structure and the interface to the C part follow
// rv003usb.S by CNLohr (https://github.com/cnlohr/rv003usb, MIT License).
//
// Receiving:
// - The interrupt is triggered by the falling edge of D- (start of SYNC).
// - The sampling phase is synchronized with the edges of the SYNC pattern, the
//   end of SYNC (two K states) is detected by the absence of an edge.
// - NRZI decoding and bit unstuffing are done on the fly, the CRC is not checked.
// - After the packet (SE0) the C handlers are called (setup, in, out, data, ack).
//
// Transmitting (usb_send_data):
// - SYNC, PID, data and CRC16 are sent with NRZI encoding and bit stuffing, the
//   CRC16 is calculated on the fly. Exactly 32 cycles per bit.
//
// Cycle counts in comments: [n] = cycle within the bit when the instruction starts.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "usb_handler.h"

.syntax unified
.cpu cortex-m0plus
.thumb

// Delay macro (n cycles)
.macro DELAY n
  .rept \n
    nop
  .endr
.endm

// Delay macro using a free register (n cycles, n >= 6: loop with 3 cycles each)
.macro DLY reg, n
  .if \n >= 6
    movs  \reg, #(\n / 3)
.Ldly\@:
    subs  \reg, #1
    bne   .Ldly\@
    DELAY (\n - 3 * (\n / 3))
  .else
    DELAY \n
  .endif
.endm

// One edge check of the SYNC window (4 cycles if no edge)
.macro SYNC_CHECK
    ldr   r0, [r3, #USB_GPIO_IDR]
    ands  r0, r2
    cmp   r0, r1
    bne   sync_edge
.endm

#define FIRST_EDGE_LOOPS    20                // wait up to 20 * 7 cycles for first edge
#define TX_POLY             0xA001            // CRC16 polynomial (reflected)

.global USB_IRQ_HANDLER
.global usb_send_data
.global usb_send_empty
.global always0

.section .RamFunc.usb_handler, "ax", %progbits

// ===================================================================================
// USB Interrupt Handler (falling edge on D-)
// ===================================================================================
.balign 4
.thumb_func
.type USB_IRQ_HANDLER, %function
USB_IRQ_HANDLER:
    push  {r4-r7, lr}
    ldr   r3, =USB_GPIO_BASE
    ldr   r2, =USB_DMASK
    ldr   r0, [r3, #USB_GPIO_IDR]             // MUST check SE0 immediately
    ands  r0, r2
    beq   handle_se0                          // SE0 -> keep-alive pulse or bus reset
    sub   sp, #28                             // stack frame: arg5 + receive buffer
    movs  r1, r0                              // r1: last line state

    // Clear EXTI pending flag, from now on new edges will trigger again
    ldr   r4, =USB_EXTI_PR
    ldr   r5, =(1<<USB_PIN_DM)
    str   r5, [r4]

    // Wait for the first edge of the SYNC pattern
    movs  r7, #FIRST_EDGE_LOOPS
wait_first:
    ldr   r0, [r3, #USB_GPIO_IDR]
    ands  r0, r2
    cmp   r0, r1
    bne   sync_edge
    subs  r7, #1
    bne   wait_first
    b     usb_exit_1                          // no edge -> no packet

    // Exit interrupt handler
usb_exit_1:
    add   sp, #28
usb_exit:
    pop   {r4-r7, pc}

    // SE0 at interrupt entry: keep-alive pulse (or bus reset)
handle_se0:
    ldr   r4, =USB_EXTI_PR
    ldr   r5, =(1<<USB_PIN_DM)
    str   r5, [r4]                            // clear EXTI pending flag
    ldr   r7, =usb_handle_se0
    blx   r7
    pop   {r4-r7, pc}

.ltorg

    // Edge detected (the ldr of the detecting check is the reference time T)
sync_edge:                                    // [T+5] (bne taken: T+3, T+4)
    movs  r1, r0                              // [T+5] new line state
    beq   usb_exit_1                          // [T+6] SE0 -> abort

    // SYNC window: 10 checks at T+7, T+11, ... T+43. The next SYNC edge is expected
    // at T+32 (+/-4). If there is no edge, the end of SYNC (KK) is reached.
sync_window:
    SYNC_CHECK
    SYNC_CHECK
    SYNC_CHECK
    SYNC_CHECK
    SYNC_CHECK
    SYNC_CHECK
    SYNC_CHECK
    SYNC_CHECK
    SYNC_CHECK
    SYNC_CHECK                                // last check: ldr at [T+43], bne [T+46]

    // No edge since T: we are in the second K of the SYNC pattern. The last edge
    // was at T-2 (average), the first bit of the PID starts at T-2+64. Its center
    // is at T+78. The first sample is taken 3 cycles earlier (at T+75), so that
    // the sampling point stays within the bit even with a clock deviation.
sync_timeout:                                 // [T+47]
    ldr   r0, =USB_KSTATE                     // [T+47] line must be K
    cmp   r1, r0                              // [T+49]
    bne   usb_exit_1                          // [T+50]
    movs  r4, #0                              // [T+51] received byte
    movs  r6, #5                              // [T+52] bit stuffing counter (SYNC ends with 1)
    movs  r7, #(USB_BUFFER_SIZE*8)            // [T+53] number of bits that can be received
    add   r5, sp, #8                          // [T+54] buffer pointer: data at sp+8 (aligned)
    subs  r5, #1                              // [T+55] PID at sp+7
    DLY   r0, 17                              // [T+56]
    b     rx_bit                              // [T+73]

// ===================================================================================
// Receive Bits
// ===================================================================================
// r0: temp, r1: last line state, r2: line mask, r3: GPIO base, r4: received byte,
// r5: buffer pointer, r6: bit stuffing counter, r7: remaining bits counter
.balign 4
rx_bit:
    ldr   r0, [r3, #USB_GPIO_IDR]             // [0] sample line
    ands  r0, r2                              // [1]
    eors  r1, r0                              // [2] r1 = transition?
    beq   rx_one                              // [3] no transition -> 1 bit
rx_zero:
    movs  r1, r0                              // [4] new line state, SE0?
    beq   rx_se0                              // [5]
    movs  r6, #6                              // [6] reset bit stuffing counter
    lsrs  r4, r4, #1                          // [7] shift in 0 bit
    b     rx_common                           // [8]
rx_one:
    movs  r1, r0                              // [5] new line state
    lsrs  r4, r4, #1                          // [6] shift in 1 bit
    adds  r4, #0x80                           // [7]
    subs  r6, #1                              // [8] six ones in a row?
    beq   rx_stuff                            // [9]
rx_common:
    subs  r7, #1                              // [10] buffer overflow?
    beq   rx_abort                            // [11]
    lsls  r0, r7, #29                         // [12] end of byte?
    beq   rx_eob                              // [13]
    DLY   r0, 16                              // [14]
    b     rx_bit                              // [30]

rx_eob:                                       // [15]
    strb  r4, [r5]                            // [15] store received byte
    adds  r5, #1                              // [17]
    DLY   r0, 12                              // [18]
    b     rx_bit                              // [30]

    // Sixth 1 bit: handle byte end, then skip the stuffed 0 bit
rx_stuff:                                     // [11]
    subs  r7, #1                              // [11]
    beq   rx_abort                            // [12]
    lsls  r0, r7, #29                         // [13]
    bne   rx_stuff_noeob                      // [14]
    strb  r4, [r5]                            // [15]
    adds  r5, #1                              // [17]
    b     rx_stuff_join                       // [18]
rx_stuff_noeob:                               // [16]
    DELAY 4                                   // [16]
rx_stuff_join:                                // [20]
    DLY   r0, 12                              // [20]
    ldr   r0, [r3, #USB_GPIO_IDR]             // [32] sample stuffed bit
    ands  r0, r2                              // [33]
    eors  r1, r0                              // [34]
    beq   rx_abort                            // [35] no transition -> bit stuffing error
    movs  r1, r0                              // [36]
    beq   rx_se0                              // [37]
    movs  r6, #6                              // [38]
    DLY   r0, 23                              // [39]
    b     rx_bit                              // [62]

rx_abort:
    b     usb_exit_1

// ===================================================================================
// End of Packet: Call the C Handlers
// ===================================================================================
rx_se0:
    lsls  r0, r7, #29                         // whole number of bytes?
    bne   rx_exit
    add   r1, sp, #8                          // r1: data pointer (after PID)
    subs  r6, r5, r1
    adds  r6, #1                              // r6: number of bytes including PID
    subs  r0, r1, #1
    ldrb  r5, [r0]                            // r5: PID
    ldr   r4, =rv003usb_internal_data
    str   r4, [sp, #0]                        // 5th argument: ist

    // ACK
    cmp   r5, #0xD2
    bne   1f
    movs  r0, #0
    movs  r2, #0
    movs  r3, #0
    ldr   r7, =usb_pid_handle_ack
    b     call_handler

    // DATA0, DATA1
1:  cmp   r6, #3
    blo   rx_exit
    movs  r2, #0
    cmp   r5, #0xC3
    beq   2f
    movs  r2, #1
    cmp   r5, #0x4B
    bne   3f
2:  movs  r0, r5                              // this_token
    movs  r3, r6                              // length (C handler subtracts 3)
    ldr   r7, =usb_pid_handle_data
    b     call_handler

    // Tokens (PID + 11 bits address/endpoint + CRC5)
3:  cmp   r6, #3
    bne   rx_exit
    ldrb  r3, [r1, #0]
    ldrb  r0, [r1, #1]
    lsls  r0, r0, #8
    orrs  r3, r0
    lsrs  r2, r3, #7
    movs  r0, #0x0F
    ands  r2, r0                              // r2: endpoint
    cmp   r2, #ENDPOINTS
    bhs   rx_exit
    movs  r0, #0x7F
    ands  r0, r3                              // r0: address
    beq   4f
    ldrb  r3, [r4, #MY_ADDRESS_OFFSET_BYTES]
    cmp   r3, r0
    bne   rx_exit                          // not our address
4:  movs  r3, #0
    ldr   r7, =usb_pid_handle_in
    cmp   r5, #0x69
    beq   call_handler
    ldr   r7, =usb_pid_handle_out
    cmp   r5, #0xE1
    beq   call_handler
    ldr   r7, =usb_pid_handle_setup
    cmp   r5, #0x2D
    bne   rx_exit

call_handler:
    blx   r7
rx_exit:
    b     usb_exit_1

.ltorg

// ===================================================================================
// Send Data
// ===================================================================================
// void usb_send_empty(uint32_t token);
// void usb_send_data(uint8_t * data, uint32_t length, uint32_t poly_function, uint32_t token);
//
// r0: CRC polynomial, r1: data pointer, r2: CRC, r3: bits to send (with stop marker),
// r4: GPIO base, r5: BSRR value, r6: BSRR toggle mask, r7: bit stuffing counter,
// r8: remaining bytes, r9: 0x10000 if CRC still has to be sent, else 0,
// r12: MODER value for output mode
.balign 4
.thumb_func
.type usb_send_empty, %function
usb_send_empty:
    movs  r3, r0
    ldr   r0, =always0
    movs  r1, #2
    movs  r2, #2

.thumb_func
.type usb_send_data, %function
usb_send_data:
    push  {r4-r7, lr}
    mov   r4, r8
    mov   r5, r9
    push  {r4, r5}
    sub   sp, #16                             // RAM buffer for data from flash

    // Copy data from flash to RAM (flash access has wait states)
    lsrs  r4, r0, #29                         // data in RAM (0x2xxxxxxx)?
    bne   2f
    cmp   r1, #8                              // max 8 bytes
    bhi   2f
    mov   r6, sp
    movs  r5, r1
    beq   3f
1:  subs  r5, #1
    ldrb  r4, [r0, r5]
    strb  r4, [r6, r5]
    bne   1b
3:  mov   r0, sp
2:

    // Prepare registers
    mov   r8, r1                              // remaining bytes
    movs  r4, #0
    cmp   r2, #0
    bne   1f
    movs  r4, #1                              // poly_function = 0 -> send CRC16
    lsls  r4, r4, #16
1:  mov   r9, r4
    movs  r1, r0                              // data pointer
    ldr   r2, =0xFFFF                         // CRC start value
    lsls  r3, r3, #7                          // SYNC (7 more bits after first K) + PID
    adds  r3, #0x40
    movs  r4, #1
    lsls  r4, r4, #15
    orrs  r3, r4                              // stop marker after 15 bits
    ldr   r4, =USB_GPIO_BASE
    ldr   r0, [r4, #USB_GPIO_MODER]
    ldr   r6, =USB_MODER_MASK
    bics  r0, r6
    ldr   r6, =USB_MODER_OUT
    orrs  r0, r6                              // r0: MODER value for output
    ldr   r5, =USB_BSRR_K
    ldr   r6, =USB_BSRR_TOGGLE
    movs  r7, #6                              // bit stuffing counter
    str   r5, [r4, #USB_GPIO_BSRR]            // preset K
    str   r0, [r4, #USB_GPIO_MODER]           // [-1] turn bus around: first K of SYNC

// -----------------------------------------------------------------------------------
// Header bits (SYNC + PID), no CRC calculation
// -----------------------------------------------------------------------------------
tx_hdr_bit:
    str   r5, [r4, #USB_GPIO_BSRR]            // [0] output bit
tx_hdr_prep:
    cmp   r3, #1                              // [1] all header bits sent?
    beq   tx_load                             // [2] -> continue with data at [4]
    lsrs  r3, r3, #1                          // [3] next bit -> carry
    bcs   tx_hdr_one                          // [4]
    eors  r5, r6                              // [5] 0 bit -> toggle
    movs  r7, #6                              // [6]
    DELAY 3                                   // [7]
    b     tx_hdr_wait                         // [10]
tx_hdr_one:                                   // [6]
    DELAY 3                                   // [6]
    subs  r7, #1                              // [9] (no stuffing possible in header)
    DELAY 2                                   // [10]
tx_hdr_wait:                                  // [12]
    DLY   r0, 16                              // [12]
    ldr   r0, =TX_POLY                        // [28] CRC polynomial for data part
    b     tx_hdr_bit                          // [30]

// -----------------------------------------------------------------------------------
// Data bits with CRC calculation and bit stuffing
// -----------------------------------------------------------------------------------
tx_bit:
    str   r5, [r4, #USB_GPIO_BSRR]            // [0] output bit
tx_prep:
    cmp   r3, #1                              // [1] byte done?
    beq   tx_load                             // [2]
    DELAY 14                                  // [3] same time as loading a byte
tx_next_bit:                                  // [17]
    lsrs  r3, r3, #1                          // [17] next bit -> carry
    bcs   tx_one                              // [18]
    eors  r5, r6                              // [19] 0 bit -> toggle
    movs  r7, #6                              // [20] reset bit stuffing counter
    lsrs  r2, r2, #1                          // [21] CRC
    bcc   1f                                  // [22]
    eors  r2, r0                              // [23]
1:  b     tx_wait                             // [24]
tx_one:                                       // [20]
    lsrs  r2, r2, #1                          // [20] CRC
    bcs   1f                                  // [21]
    eors  r2, r0                              // [22]
1:  subs  r7, #1                              // [23] six ones?
    beq   tx_stuff                            // [24]
    DELAY 1                                   // [25]
tx_wait:                                      // [26]
    DELAY 4                                   // [26]
    b     tx_bit                              // [30]

    // Load next byte, CRC or end of packet (entered at [4], continues at [17])
tx_load:                                      // [4]
    mov   r3, r8                              // [4]
    subs  r3, #1                              // [5]
    bmi   tx_nodata                           // [6]
    mov   r8, r3                              // [7]
    ldrb  r3, [r1]                            // [8]
    adds  r1, #1                              // [10]
    adds  r3, #128                            // [11]
    adds  r3, #128                            // [12] stop marker at bit 8
    DELAY 2                                   // [13]
    b     tx_next_bit                         // [15]
tx_nodata:                                    // [8]
    mov   r3, r9                              // [8] CRC to send?
    cmp   r3, #0                              // [9]
    beq   tx_eop                              // [10]
    mvns  r3, r2                              // [11] inverted CRC
    uxth  r3, r3                              // [12]
    add   r3, r9                              // [13] stop marker at bit 16
    mov   r9, r8                              // [14] r8 is 0 here -> no more CRC
    b     tx_next_bit                         // [15]

    // Six ones: send this bit, then the stuffed 0 bit
tx_stuff:                                     // [26]
    DELAY 6                                   // [26]
    str   r5, [r4, #USB_GPIO_BSRR]            // [32] sixth 1 bit
    eors  r5, r6                              // [33] stuffed 0 bit
    movs  r7, #6                              // [34]
    DLY   r0, 25                              // [35]
    ldr   r0, =TX_POLY                        // [60] restore CRC polynomial
    b     tx_bit                              // [62] -> str at [64]

    // End of packet: SE0 for 2 bits, J for 1 bit, then release the bus
tx_eop:                                       // [12]
    ldr   r5, =USB_BSRR_SE0                   // [12]
    DLY   r3, 18                              // [14]
    str   r5, [r4, #USB_GPIO_BSRR]            // [32] SE0
    ldr   r5, =USB_BSRR_J                     // [33]
    DLY   r3, 61                              // [35]
    str   r5, [r4, #USB_GPIO_BSRR]            // [96] J
    ldr   r5, [r4, #USB_GPIO_MODER]           // [97]
    ldr   r6, =USB_MODER_MASK                 // [98]
    bics  r5, r6                              // [100]
    DLY   r3, 27                              // [101]
    str   r5, [r4, #USB_GPIO_MODER]           // [128] release bus (input mode)

    // Clear pending interrupt caused by our own transmission
    ldr   r4, =USB_EXTI_PR
    ldr   r5, =(1<<USB_PIN_DM)
    str   r5, [r4]
    ldr   r4, =USB_NVIC_ICPR
    ldr   r5, =(1<<USB_IRQ_NUM)
    str   r5, [r4]

    add   sp, #16
    pop   {r4, r5}
    mov   r8, r4
    mov   r9, r5
    pop   {r4-r7, pc}

.ltorg

.balign 4
always0:
    .word 0x00000000
