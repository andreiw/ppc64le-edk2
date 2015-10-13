//
// Copyright (c) 2015, Andrei Warkentin <andrey.warkentin@gmail.com>
//
// This program and the accompanying materials
// are licensed and made available under the terms and conditions of the BSD License
// which accompanies this distribution.  The full text of the license may be found at
// http://opensource.org/licenses/bsd-license.php
//
// THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
// WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
//

#ifndef __PPC64_INTRINSICS_H__
#define __PPC64_INTRINSICS_H__

#include <Chipset/PPC64.h>

#define _S(...) __S(__VA_ARGS__)
#define __S(...) #__VA_ARGS__

#define REG_READ_FN(reg)                                                \
  static inline UINT64                                                  \
  GET_##reg(void)                                                       \
  {                                                                     \
    UINT64 reg = 0;                                                     \
    asm volatile("mfspr %0, " _S(SPRN_##reg)                            \
                 : "=r" (reg) :: "memory");                             \
    return reg;                                                         \
  }                                                                     \

#define REG_WRITE_FN(reg)                                               \
  static inline void                                                    \
  SET_##reg(UINT64 reg)                                                 \
  {                                                                     \
    asm volatile("mtspr " _S(SPRN_##reg)", %0"                          \
                 :: "r" (reg) : "memory");                              \
  }                                                                     \

REG_READ_FN(LPCR)
REG_WRITE_FN(LPCR)
REG_READ_FN(HID0)
REG_WRITE_FN(HID0)
REG_WRITE_FN(HSPRG0)
REG_READ_FN(DEC)
REG_WRITE_FN(DEC)
REG_READ_FN(HDEC)
REG_WRITE_FN(HDEC)
REG_READ_FN(SDR1)
REG_WRITE_FN(SDR1)
REG_READ_FN(DAR)
REG_READ_FN(DSISR)

#undef _S
#undef __S

static inline UINT64
mftb(void)
{
  UINT64 tb;
  asm volatile("mftb %0" : "=r"(tb) : : "memory");
  return tb;
}

static inline UINT64
mfmsr(void)
{
  UINT64 val;

  asm volatile("mfmsr %0" : "=r"(val) : : "memory");
  return val;
}


static inline void
mtmsr(UINT64 val)
{
  asm volatile("mtmsr %0" : : "r"(val) : "memory");
}

static inline void  __attribute__((always_inline))
mtmsrd(UINT64 val, const UINTN l)
{
  asm volatile("mtmsrd %0,%1" : : "r"(val), "i"(l) : "memory");
}

static inline VOID
smt_medium(VOID)
{
  asm volatile("or 2, 2, 2");
}

static inline VOID
hdec_enable(VOID)
{
  SET_LPCR(GET_LPCR() | LPCR_HDICE);
}

static inline VOID
hdec_disable(VOID)
{
  SET_LPCR(GET_LPCR() & ~LPCR_HDICE);
}

#endif // __PPC64_INTRINSICSH__
