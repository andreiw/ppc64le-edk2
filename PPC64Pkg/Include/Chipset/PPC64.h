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

#ifndef __PPC64_H__
#define __PPC64_H__

#ifdef PPC64_ABI_elfv1
// 48 + 64 parameter save area
#define STACK_FRAME_MIN 112

//
// The 288 bytes below the stack pointer is
// available as volatile storage which is not
// preserved across function calls. Interrupt
// handlers and any other functions that might
// run without an explicit call must take care
// to preserve this region. If a function does
// not need more stack space than is available
// in this area, it does not need to have a
// stack frame.
//
#define STACK_PROTECTED_ZONE 288
#define STACK_TOC            40
#define STACK_LR             16
#define STACK_CR             8
#endif /* PPC64_ABI_elfv1 */

#ifdef PPC64_ABI_elfv2
//
// Power Architecture 64-Bit ELF V2 ABI: The minimum stack frame
// size shall be 32 bytes. A minimum stack frame consists of the
// first 4 double-words (back-chain doubleword, CR save word and
// reserved word, LR save doubleword, and TOC pointer doubleword),
// with padding to meet the 16-byte alignment requirement.
//
#define STACK_FRAME_MIN 32
//
// ELF v2 ABI, p46, 2.2.2.4 Protected Zone.
//
// The 288 bytes below the stack pointer are available as volatile
// program storage that is not preserved across function calls.
// Interrupt handlers and any other functions that might run
// without an explicit call must take care to preserve a
// protected zone, also referred to as the red zone, of 512 bytes
// that consists of:
// - The 288-byte volatile program storage region that is used
//   to hold saved registers and local variables.
// - An additional 224 bytes below the volatile program storage
//   region that is set aside as a volatile system storage region for
//   system functions
//
// If a function does not call other functions and does not need
// more stack space than is available in the volatile program
// storage region (that is, 288 bytes), it does not need to have
// a stack frame. The 224-byte volatile system storage region is
// not available to compilers for allocation to saved registers
// and local variables.
//
#define STACK_PROTECTED_ZONE 512

#define STACK_TOC            24
#define STACK_LR             16
#define STACK_CR             8
#endif /* PPC64_ABI_elfv2 */

#ifndef __ASSEMBLY__
#define __MASK(X) (1UL << (X))
#else
#define __MASK(X) (1 << (X))
#endif

#define MSR_SF_LG       63              /* Enable 64 bit mode */
#define MSR_HV_LG       60              /* Hypervisor state */
#define MSR_SLE_LG      58              /* Split Little Endian */
#define MSR_TS1_LG      34              /* Transaction State 0 */
#define MSR_TS0_LG      33              /* Transaction State 1 */
#define MSR_TSA_LG      32              /* Transactional Memory Available */
#define MSR_VEC_LG      25              /* Enable AltiVec */
#define MSR_VSX_LG      23              /* Enable VSX */
#define MSR_EE_LG       15              /* External Interrupt Enable */
#define MSR_PR_LG       14              /* Problem State / Privilege Level */
#define MSR_FP_LG       13              /* Floating Point enable */
#define MSR_ME_LG       12              /* Machine Check Enable */
#define MSR_FE0_LG      11              /* Floating Exception mode 0 */
#define MSR_SE_LG       10              /* Single Step */
#define MSR_BE_LG       9               /* Branch Trace */
#define MSR_FE1_LG      8               /* Floating Exception mode 1 */
#define MSR_IR_LG       5               /* Instruction Relocate */
#define MSR_DR_LG       4               /* Data Relocate */
#define MSR_PMM_LG      2               /* Performance monitor */
#define MSR_RI_LG       1               /* Recoverable Exception */
#define MSR_LE_LG       0               /* Little Endian */

#define MSR_SF          __MASK(MSR_SF_LG)       /* Enable 64 bit mode */
#define MSR_HV          __MASK(MSR_HV_LG)       /* Hypervisor state */
#define MSR_SLE         __MASK(MSR_SLE_LG)      /* Split Little Endian */
#define MSR_TS1         __MASK(MSR_TS1_LG)      /* Transaction State 0 */
#define MSR_TS0         __MASK(MSR_TS0_LG)      /* Transcation State 1 */
#define MSR_TSA         __MASK(MSR_TSA_LG)      /* Transactional Memory */
#define MSR_VEC         __MASK(MSR_VEC_LG)      /* Enable AltiVec */
#define MSR_VSX         __MASK(MSR_VSX_LG)      /* Enable VSX */
#define MSR_EE          __MASK(MSR_EE_LG)       /* External Interrupt Enable */
#define MSR_PR          __MASK(MSR_PR_LG)       /* Problem State / Privilege Level */
#define MSR_FP          __MASK(MSR_FP_LG)       /* Floating Point enable */
#define MSR_ME          __MASK(MSR_ME_LG)       /* Machine Check Enable */
#define MSR_FE0         __MASK(MSR_FE0_LG)      /* Floating Exception mode 0 */
#define MSR_SE          __MASK(MSR_SE_LG)       /* Single Step */
#define MSR_BE          __MASK(MSR_BE_LG)       /* Branch Trace */
#define MSR_FE1         __MASK(MSR_FE1_LG)      /* Floating Exception mode 1 */
#define MSR_IR          __MASK(MSR_IR_LG)       /* Instruction Relocate */
#define MSR_DR          __MASK(MSR_DR_LG)       /* Data Relocate */
#define MSR_PMM         __MASK(MSR_PMM_LG)      /* Performance monitor */
#define MSR_RI          __MASK(MSR_RI_LG)       /* Recoverable Exception */
#define MSR_LE          __MASK(MSR_LE_LG)       /* Little Endian */

#define SPRN_HID0       0x3F0           /* Hardware Implementation Register 0 */
#define HID0_HILE       __MASK(63 - 19) /* Hypervisor interrupt LE on Power8 */
#define SPRN_LPCR       0x13E           /* LPAR Control Register */
#define SPRN_HRMOR      0x139           /* Real mode offset register */
#define SPRN_SRR0       0x01A           /* Save/Restore Register 0 */
#define SPRN_SRR1       0x01B           /* Save/Restore Register 1 */
#define SPRN_HSRR0      0x13A           /* Save/Restore Register 0 */
#define SPRN_HSRR1      0x13B           /* Save/Restore Register 1 */
#define SPRN_HSPRG0     0x130           /* Hypervisor Scratch 0 */
#define SPRN_HSPRG1     0x131           /* Hypervisor Scratch 1 */
#define SPRN_HDEC       0x136           /* Hypervisor decrementer */
#define SPRN_DEC        0x16            /* Decrementer. */
#define SPRN_SDR1       0x019           /* HTAB base. */
#define SPRN_DAR        0x013           /* Data Adress Register. */
#define SPRN_DSISR      0x012           /* Data Storage Interrupt Status */

#define LPCR_ILE_LG     (63 - 38)             /* Interrupt Little Endian */
#define LPCR_ILE        __MASK(LPCR_ILE_LG)
#define LPCR_AIL1_LG    (63 - 39)             /* Alternate Interrupt Location 1 */
#define LPCR_AIL1        __MASK(LPCR_AIL1_LG)
#define LPCR_AIL0_LG    (63 - 40)             /* Alternate Interrupt Location 0 */
#define LPCR_AIL0       __MASK(LPCR_AIL0_LG)
#define LPCR_LPES_LG    (63 - 60)             /* Logical Part. Env. Selector */
#define LPCR_LPES       __MASK(LPCR_LPES_LG)
#define LPCR_HDICE_LG   (63 - 63)             /* Hypervisor Decrementer Enable */
#define LPCR_HDICE      __MASK(LPCR_HDICE_LG)

/*
 * We can't really disable the regular decrementer, but we can set it
 * to the maximum value that doesn't cause an interrupt, which is
 * the value with MSB bit set to 0.
 */
#define DEC_MAX_USABLE 0x7fffffff

#endif // __PPC64_H__
