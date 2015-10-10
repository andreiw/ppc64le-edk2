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

#include <AutoGen.h>
#include <AsmMacroLib.h>
#include <Chipset/PPC64.h>
#include <Pcr.h>

#define OPAL_SUCCESS            0
#define OPAL_PARAMETER          -1
#define OPAL_BUSY               -2
#define OPAL_PARTIAL            -3
#define OPAL_CONSTRAINED        -4
#define OPAL_CLOSED             -5
#define OPAL_HARDWARE           -6
#define OPAL_UNSUPPORTED        -7
#define OPAL_PERMISSION         -8
#define OPAL_NO_MEM             -9
#define OPAL_RESOURCE           -10
#define OPAL_INTERNAL_ERROR     -11
#define OPAL_BUSY_EVENT         -12
#define OPAL_HARDWARE_FROZEN    -13

#define OPAL_CONSOLE_WRITE       1
#define OPAL_CONSOLE_READ        2
#define OPAL_TERMINAL_0          0

#define OPAL_POLL_EVENTS         10

#define OPAL_REINIT_CPUS         70
#define OPAL_REINIT_CPUS_HILE_LE (1 << 1)

/*
 * Perform a call to the OpenPower Abstraction Layer
 * from little-endian mode (virt or real).
 */
#define OPAL_CALL(name, token)				\
	ASM_FUNC(name);					\
	li	r0, token;				\
	b	opal_call_common;
opal_call_common:
	std	r2, STACK_TOC(r1)
	mflr	r12
	std	r12,STACK_LR(r1)
	mfcr	r12;
	std	r12,STACK_CR(r1)
	/*
	 * We'll use r14 to pass current MSR to opal_return,
	 * so save clobbered r14 on stack.
	 */
	std	r14,-8(r1)
	LOAD_ADDR(r12, opal_return)
	mtlr	r12
	/*
	 * OPAL calls done in big-endian call, so we
	 * will return from interrupt into big-endian
	 * mode running the OPAL entry.
	 */
	mfmsr	r14;
	li	r11, MSR_LE;
	/*
	 * OPAL expects MMU and external interrupts off.
	 */
	ori	r11, r11, MSR_EE;
	ori	r11, r11, MSR_IR;
	ori	r11, r11, MSR_DR;
	andc	r12, r14, r11;
	/*
	 * EE must be off before we touch HSRR1/HSRR0.
	 */
	li	r11, 0
	mtmsrd	r11, 1
	mtspr	SPRN_HSRR1, r12;
	ld	r2, PCR(OPALBase);
	ld	r12, PCR(OPALEntry);
	mtspr	SPRN_HSRR0, r12;
	/*
	 * OPAL preserves r1, r13-r31. r3-r11 are
	 * arguments.
	 */
	hrfid
ASM_FUNC(opal_return)
	/*
	 * Enter here in BE with r13 = PCR, r14 = old MSR.
	 * Stack contains CR/LR in ABI slots, and clobbered r14
	 * value at -8.
	 *
	 * We must restore MSR before touching any memory, because
	 * virt-mode might need to be on.
	 */
	FIXUP_ENDIAN
	mtmsr	r14
	ld	r2, STACK_TOC(r1)
	ld	r12,STACK_CR(r1)
	mtcr	r12
	ld	r12,STACK_LR(r1)
	ld	r14,-8(r1)
	mtlr	r12
	blr

OPAL_CALL(OpalConsoleWrite, OPAL_CONSOLE_WRITE)
OPAL_CALL(OpalConsoleRead, OPAL_CONSOLE_READ)
OPAL_CALL(OpalReinitCPUs, OPAL_REINIT_CPUS)
OPAL_CALL(OpalPollEvents, OPAL_POLL_EVENTS)
