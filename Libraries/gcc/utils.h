// ===================================================================================
// Some Useful Functions                                                      * v0.2 *
// ===================================================================================
//
// Functions available:
// --------------------
// abs(n)                   returns positive value of n
// min(x,y)                 returns minimum of x and y
// max(x,y)                 returns maximum of x and y
// constrain(x,lo,hi)       limits x to the range lo..hi
// swap(x,y)                swaps values of x and y
//
// random(max)              returns random value between 0 and max-1 (max > 0)
// random_seed(val)         sets start value of random function (val != 0)
//
// map(x,imin,imax,omin,omax)  maps x from input range to output range
//
// Notes:
// ------
// - The macros evaluate their arguments more than once, so don't use expressions with
//   side effects (e.g. min(i++, j)).
// - The random function is a 32-bit xorshift generator.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>

#ifndef abs
#define abs(n)                (((n)>=0)?(n):(-(n)))
#endif
#ifndef min
#define min(x,y)              (((x)>(y))?(y):(x))
#endif
#ifndef max
#define max(x,y)              (((x)>(y))?(x):(y))
#endif
#define constrain(x,lo,hi)    (((x)<(lo))?(lo):(((x)>(hi))?(hi):(x)))

#define swap(x,y) do {   \
  __typeof__(x) _t = x;  \
  x = y;                 \
  y = _t;                \
} while(0)

static uint32_t rnval = 0xACE1DFEE;

static inline void random_seed(uint32_t val) {
  if(val) rnval = val;
}

static inline uint32_t random(uint32_t max) {
  rnval ^= rnval << 13;
  rnval ^= rnval >> 17;
  rnval ^= rnval << 5;
  return max ? (rnval % max) : 0;
}

static inline int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) {
  if(in_max == in_min) return out_min;
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
