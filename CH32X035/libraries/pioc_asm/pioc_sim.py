#!/usr/bin/env python3
# ===================================================================================
# Project:   pioc_sim - Simple Simulator for the PIOC (RISC8B eMCU) of the CH32X035
# Version:   v1.0
# Year:      2026
# Author:    Stefan Wagner
# Github:    https://github.com/wagiminator
# License:   MIT License
# ===================================================================================
#
# Description:
# ------------
# Cycle-counting instruction simulator for checking PIOC programs (logic, timing,
# handshake with the host) without hardware. The pins, the host interface, the bit
# encoder (PWM modulation, transmit) and timer 0 (PWM mode) are modeled according to
# the PIOC manual. The peripheral models are approximations, not a verified model of
# the silicon!
#
# Usage (Python):
# ---------------
# from pioc_sim import PIOC, pulses
# p = PIOC(open('blink.bin', 'rb').read())   # binary from: pioc_asm.py blink.asm -b
# p.sfr[0x20] = 10                           # host writes DATA_REG0
# p.host_write_cmd(1)                        # host writes CTRL_WR
# p.run_cycles(1000000)                      # run 1M clock cycles
# print(pulses(p.trace[0]))                  # (level, duration) of IO0

SFR_INDIR_PORT, SFR_INDIR_PORT2, SFR_PRG_COUNT, SFR_STATUS = 0x00, 0x01, 0x02, 0x03
SFR_INDIR_ADDR, SFR_TMR0_COUNT, SFR_TIMER_CTRL, SFR_TMR0_INIT = 0x04, 0x05, 0x06, 0x07
SFR_BIT_CYCLE, SFR_INDIR_ADDR2, SFR_PORT_DIR, SFR_PORT_IO = 0x08, 0x09, 0x0A, 0x0B
SFR_BIT_CONFIG, SFR_SYS_CFG, SFR_CTRL_RD, SFR_CTRL_WR = 0x0C, 0x1C, 0x1D, 0x1E
SFR_DATA_EXCH = 0x1F
Z, C = 2, 0


class PIOC:
  def __init__(self, code_bytes):
    self.mem = bytearray(4096)
    self.mem[:len(code_bytes)] = code_bytes
    self.sfr = [0] * 0x40
    self.sfr[SFR_BIT_CONFIG] = 0x10
    self.pc = 1
    self.a = 0
    self.stack = []
    self.cycle = 0
    self.ext = [1, 1]                 # external levels (pull-ups)
    self.trace = [[], []]             # (cycle, level) changes per pin
    self.level = [None, None]
    self.rise = self.fall = False
    self.enc_cnt = 0
    self.enc_bit = 0
    self.enc_on = False
    self.tmr_div = 0
    self.max_stack = 0
    self.update_pins()

  # --- memory / SFR -------------------------------------------------------------
  def word(self, a):
    return self.mem[2 * a] | self.mem[2 * a + 1] << 8

  def rd(self, f):
    f &= 0xFF
    if f == SFR_INDIR_PORT:
      return self.rd(self.sfr[SFR_INDIR_ADDR])
    if f == SFR_INDIR_PORT2:
      return self.rd(self.sfr[SFR_INDIR_ADDR2])
    if f == SFR_STATUS:
      return self.sfr[SFR_STATUS] | (0x10 if self.stack else 0)
    if f == SFR_PRG_COUNT:
      return self.pc & 0xFF
    if f == SFR_CTRL_WR:
      self.sfr[SFR_SYS_CFG] &= ~0x20      # DATA_MW_SR cleared by eMCU read
    if f == SFR_PORT_IO:
      v = self.sfr[SFR_PORT_IO] & 0x03
      i0, i1 = self.level
      v |= i0 << 4 | i1 << 5 | (i0 ^ i1) << 7
      v |= ((i0 ^ (v & 1)) << 2) | ((i1 ^ (v >> 1 & 1)) << 3)
      return v
    if f < 0x40:
      return self.sfr[f]
    raise RuntimeError('read of unimplemented register 0x%02X at pc 0x%03X' % (f, self.pc))

  def wr(self, f, v, indirect2=False):
    f &= 0xFF
    v &= 0xFF
    if f == SFR_INDIR_PORT:
      return self.wr(self.sfr[SFR_INDIR_ADDR], v)
    if f == SFR_INDIR_PORT2:
      return self.wr(self.sfr[SFR_INDIR_ADDR2], v)
    if f == SFR_PRG_COUNT:
      self.pc = (self.pc & 0x700) | v
      self.jumped = True
      return
    if f == SFR_STATUS:
      self.sfr[f] = v & 0x2F
      return
    if f == SFR_SYS_CFG:
      self.sfr[f] = (self.sfr[f] & 0x7F) | (v & 0x80)
      return
    if f == SFR_CTRL_RD:
      self.sfr[f] = v
      self.sfr[SFR_SYS_CFG] |= 0x40       # DATA_SW_MR
      return
    if f == SFR_CTRL_WR:
      return                              # read only for eMCU
    if f == SFR_BIT_CYCLE:
      was_on = self.enc_active()
      self.sfr[f] = v
      self.rise = self.fall = False       # writing clears the edge records
      if self.enc_active() and not was_on:
        self.enc_cnt = 0
        self.enc_start_bit()
      if not self.enc_active():
        self.enc_on = False
      return
    if f == SFR_PORT_IO:
      self.sfr[f] = v & 0x03
      self.update_pins()
      return
    if f == SFR_PORT_DIR:
      self.sfr[f] = v
      self.update_pins()
      return
    if f < 0x40:
      self.sfr[f] = v
      return
    raise RuntimeError('write of unimplemented register 0x%02X' % f)

  # --- peripherals --------------------------------------------------------------
  def enc_active(self):
    return (self.sfr[SFR_BIT_CYCLE] & 0x7C) != 0 and (self.sfr[SFR_BIT_CONFIG] & 0x80)

  def enc_start_bit(self):
    self.enc_bit = self.sfr[SFR_BIT_CYCLE] >> 7
    self.enc_on = True

  def update_pins(self):
    for i in (0, 1):
      if self.sfr[SFR_PORT_DIR] >> i & 1:
        lvl = self.sfr[SFR_PORT_IO] >> i & 1
        if i == 0 and self.sfr[SFR_TIMER_CTRL] & 0x10:
          lvl = self.tmr_out
      else:
        lvl = self.ext[i]
      if lvl != self.level[i]:
        if i == 0 and self.level[0] is not None:
          if lvl:
            self.rise = True
          else:
            self.fall = True
        self.level[i] = lvl
        self.trace[i].append((self.cycle, lvl))

  tmr_out = 0

  def tick(self):
    # bit encoder (PWM modulation, TX)
    if self.enc_active() and self.enc_on:
      period = (self.sfr[SFR_BIT_CYCLE] & 0x7F) + 1
      high = period * (3 if self.enc_bit else 1) // 4
      out = 1 if self.enc_cnt < high else 0
      self.sfr[SFR_PORT_IO] = (self.sfr[SFR_PORT_IO] & ~1) | out
      tail = self.enc_cnt >= period * 3 // 4
      self.sfr[SFR_BIT_CONFIG] = (self.sfr[SFR_BIT_CONFIG] & 0xEF) | (0x10 if tail else 0)
      self.update_pins()
      self.enc_cnt += 1
      if self.enc_cnt >= period:
        self.enc_cnt = 0
        self.enc_start_bit()
    # timer 0 (PWM mode only)
    tc = self.sfr[SFR_TIMER_CTRL]
    if tc & 0x20:
      div = (1024, 256, 64, 16, 8, 4, 2, 1)[tc & 7]
      self.tmr_div += 1
      if self.tmr_div >= div:
        self.tmr_div = 0
        cnt = (self.sfr[SFR_TMR0_COUNT] + 1) & 0xFF
        self.sfr[SFR_TMR0_COUNT] = cnt
        if tc & 0x08:
          self.tmr_out = 1 if cnt > self.sfr[SFR_TMR0_INIT] else 0
        if tc & 0x10:
          self.update_pins()
    self.cycle += 1

  # --- host side ---------------------------------------------------------------
  def host_write_cmd(self, v):
    self.sfr[SFR_CTRL_WR] = v & 0xFF
    self.sfr[SFR_SYS_CFG] |= 0x20

  def host_read_result(self):
    self.sfr[SFR_SYS_CFG] &= ~0x40
    return self.sfr[SFR_CTRL_RD]

  # --- core ---------------------------------------------------------------------
  def setz(self, r):
    self.sfr[SFR_STATUS] = (self.sfr[SFR_STATUS] & ~(1 << Z)) | ((r & 0xFF) == 0) << Z

  def setc(self, c):
    self.sfr[SFR_STATUS] = (self.sfr[SFR_STATUS] & ~(1 << C)) | (1 if c else 0)

  def flag(self, b):
    return self.sfr[SFR_STATUS] >> b & 1

  def step(self):
    """execute one instruction, return number of cycles"""
    w = self.word(self.pc)
    pc = self.pc
    self.pc = (self.pc + 1) & 0x7FF
    self.jumped = False
    cyc = 1
    hi = w >> 8
    if w & 0x8000:                                    # CMPZ
      if self.a == (w >> 8 & 0x7F):
        self.pc = (pc & 0x700) | (w & 0xFF)
        cyc = 2
    elif (w & 0xF000) == 0x6000:                        # JMP
      self.pc = w & 0xFFF & 0x7FF
      cyc = 2
    elif (w & 0xF000) == 0x7000:                        # CALL
      self.stack.append(self.pc)
      self.max_stack = max(self.max_stack, len(self.stack))
      if len(self.stack) > 6:
        raise RuntimeError('stack overflow at 0x%03X' % pc)
      self.pc = w & 0x7FF
      cyc = 2
    elif (w & 0xF000) in (0x4000, 0x5000):              # BC BS BTSC BTSS
      f, b = w & 0xFF, w >> 8 & 7
      op = w >> 11 & 3
      v = self.rd(f)
      if op == 0:
        self.wr(f, v & ~(1 << b))
      elif op == 1:
        self.wr(f, v | (1 << b))
      elif op == 2:
        if not v >> b & 1:
          self.pc += 1
          cyc = 2
      else:
        if v >> b & 1:
          self.pc += 1
          cyc = 2
    elif (w & 0xF000) == 0x3000:                        # JNZ JZ JNC JC
      t = (pc & 0x400) | (w & 0x3FF)
      c = w >> 10 & 3
      cond = [not self.flag(Z), self.flag(Z), not self.flag(C), self.flag(C)][c]
      if cond:
        self.pc = t
        cyc = 2
    elif (w & 0xF000) == 0x2000:                        # constants
      k = w & 0xFF
      op = hi & 0x0F
      if op == 0x0:                                   # RETL
        self.a = k
        self.pc = self.stack.pop()
        cyc = 2
      elif op == 0x1:
        self.a = k
        self.setz(1)
        self.pc = self.stack.pop()
        cyc = 2
      elif op in (0x2, 0x3):                          # MOVIP / MOVA1F
        if op == 0x2:
          self.sfr[SFR_INDIR_ADDR] = k
        else:
          self.wr(SFR_PORT_DIR, k)
      elif op in (0x4, 0x5, 0x6, 0x7):
        if op == 0x4 or op == 0x5:
          if op == 0x4:
            self.sfr[SFR_INDIR_ADDR2] = k
          else:
            self.wr(SFR_PORT_IO, k)
        elif op == 0x6:
          self.wr(self.sfr[SFR_INDIR_ADDR2], k)
        else:
          self.wr(self.sfr[SFR_INDIR_ADDR], k)
      elif op == 0x8:
        self.a = k
      elif op == 0x9:
        self.a &= k
        self.setz(self.a)
      elif op == 0xA:
        self.a |= k
        self.setz(self.a)
      elif op == 0xB:
        self.a ^= k
        self.setz(self.a)
      elif op == 0xC:
        r = self.a + k
        self.setc(r > 0xFF)
        self.a = r & 0xFF
        self.setz(self.a)
      elif op == 0xD:                                 # SUBL k-A, C = borrow
        r = k - self.a
        self.setc(r < 0)
        self.a = r & 0xFF
        self.setz(self.a)
      elif op == 0xE:
        r = k + self.a
        self.setc(r > 0xFF)
        self.setz(r)
      elif op == 0xF:
        r = k - self.a
        self.setc(r < 0)
        self.setz(r)
    elif (w & 0xE000) == 0x0000 and (w & 0x0F00) != 0:  # byte ops
      d = w >> 12 & 1
      op = w >> 8 & 0xF
      if op in (2, 3):                                # MOV F,d (9 bit)
        f = w & 0x1FF
        v = self.rd(f)
        self.setz(v)
        if d:
          self.wr(f, v)
        else:
          self.a = v
      elif op == 1:
        if d:
          raise RuntimeError('unknown opcode %04X' % w)
        self.wr(w & 0xFF, 0)
        self.setz(0)
      else:
        f = w & 0xFF
        v = self.rd(f)
        skip = False
        if op == 4:
          r = (v + 1) & 0xFF; self.setz(r)
        elif op == 5:
          r = (v - 1) & 0xFF; self.setz(r)
        elif op == 6:
          r = (v + 1) & 0xFF; skip = r == 0
        elif op == 7:
          r = (v - 1) & 0xFF; skip = r == 0
        elif op == 8:
          r = (v >> 4 | v << 4) & 0xFF
        elif op == 9:
          r = v & self.a; self.setz(r)
        elif op == 10:
          r = v | self.a; self.setz(r)
        elif op == 11:
          r = v ^ self.a; self.setz(r)
        elif op == 12:
          r = v + self.a; self.setc(r > 0xFF); r &= 0xFF; self.setz(r)
        elif op == 13:
          r = v - self.a; self.setc(r < 0); r &= 0xFF; self.setz(r)
        elif op == 14:
          r = (v << 1 | self.flag(C)) & 0xFF; self.setc(v >> 7)
        elif op == 15:
          r = v >> 1 | self.flag(C) << 7; self.setc(v & 1)
        if d:
          self.wr(f, r)
        else:
          self.a = r
        if skip:
          self.pc += 1
          cyc = 2
    elif (w & 0xF000) == 0x1000:                        # MOVA F
      self.wr(w & 0x1FF, self.a)
    elif (w & 0xFF00) == 0x0000:                        # misc
      lo = w & 0xFF
      if lo == 0:
        pass
      elif (lo & 0xFC) == 0x04:                         # CLRA
        self.a = 0
        self.setz(0)
      elif (lo & 0xF8) == 0x10:                         # WAITB
        b = lo & 7
        sc = self.sfr[SFR_SYS_CFG]
        io = self.rd(SFR_PORT_IO)
        ok = [not sc & 0x40, self.sfr[SFR_BIT_CONFIG] & 0x10, self.fall, self.rise,
              sc & 0x20, io & 0x08, not io & 0x04, io & 0x04][b]
        if not ok:
          self.pc = pc                                # stay
        else:
          if b == 2:
            self.fall = False
          if b == 3:
            self.rise = False
      elif lo == 0x18:                                # RDCODE
        addr = (self.a & 7) << 8 | self.sfr[SFR_INDIR_ADDR]
        wv = self.word(addr)
        self.a = wv & 0xFF
        self.sfr[SFR_INDIR_ADDR] = wv >> 8
        cyc = 2
      elif (lo & 0xFC) == 0x1C:                         # BCTC
        a = lo & 3
        io = self.rd(SFR_PORT_IO)
        if a == 0:
          self.setc(self.flag(C) ^ (io >> 4 & 1))
        elif a == 1:
          self.setc(io >> 6 & 1)
        elif a == 2:
          self.setc(io >> 4 & 1)
        else:
          self.setc(io >> 5 & 1)
      elif (lo & 0xFC) == 0x30:                         # RET
        self.pc = self.stack.pop()
        cyc = 2
      elif (lo & 0xFC) == 0x34:                         # RETZ
        self.pc = self.stack.pop()
        self.setz(0)
        cyc = 2
      elif (lo & 0xE0) == 0x80 or (lo & 0xE0) == 0xA0:    # BP1F / BP2F
        reg = SFR_INDIR_ADDR if (lo & 0xE0) == 0x80 else SFR_DATA_EXCH
        a, b = lo >> 3 & 3, lo & 7
        bit = self.sfr[reg] >> b & 1
        if a == 0:
          self.setc(bit)
        elif a == 1:
          self.sfr[SFR_BIT_CYCLE] = (self.sfr[SFR_BIT_CYCLE] & 0x7F) | bit << 7
        elif a == 2:
          self.wr(SFR_PORT_IO, (self.sfr[SFR_PORT_IO] & ~1) | bit)
        else:
          self.wr(SFR_PORT_IO, (self.sfr[SFR_PORT_IO] & ~2) | bit << 1)
      elif (lo & 0xC0) == 0xC0:                         # BG1F / BG2F
        reg = SFR_INDIR_ADDR if (lo & 0xE0) == 0xC0 else SFR_DATA_EXCH
        a, b = lo >> 3 & 3, lo & 7
        io = self.rd(SFR_PORT_IO)
        bit = [self.flag(C), io >> 6 & 1, io >> 4 & 1, io >> 5 & 1][a]
        self.sfr[reg] = (self.sfr[reg] & ~(1 << b)) | bit << b
      else:
        raise RuntimeError('unsupported instruction %04X at 0x%03X' % (w, pc))
    else:
      raise RuntimeError('unsupported instruction %04X at 0x%03X' % (w, pc))
    if self.jumped:
      cyc = 2
    for _ in range(cyc):
      self.tick()
    return cyc

  def run_until(self, cond, max_cycles=10_000_000):
    start = self.cycle
    while not cond():
      self.step()
      if self.cycle - start > max_cycles:
        raise RuntimeError('timeout at pc 0x%03X' % self.pc)

  def run_cycles(self, n):
    end = self.cycle + n
    while self.cycle < end:
      self.step()


def pulses(trace, start=0):
  """return list of (level, duration) from a pin trace"""
  out = []
  t = [x for x in trace if x[0] >= start]
  for (c0, l0), (c1, _) in zip(t, t[1:]):
    out.append((l0, c1 - c0))
  return out
