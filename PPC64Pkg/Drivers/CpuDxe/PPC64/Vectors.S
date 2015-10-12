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

#define EXC_START       0x0
#define EXC_RESET       0x100
#define EXC_MC          0x200
#define EXC_DSI         0x300
#define EXC_DSEG        0x380
#define EXC_ISI         0x400
#define EXC_ISEG        0x480
#define EXC_EXT         0x500
#define EXC_ALIGN       0x600
#define EXC_PROG        0x700
#define EXC_FPU         0x800
#define EXC_DEC         0x900
#define EXC_HDEC        0x980
#define EXC_DOOR        0xA00
#define EXC_RESV        0xB00
#define EXC_SC          0xC00
#define EXC_TRACE       0xD00
#define EXC_HDSI        0xE00
#define EXC_HISI        0xE20
#define EXC_HEA         0xE40
#define EXC_HMAINT      0xE60
#define EXC_HDOOR       0xE80
#define EXC_RESV1       0xEA0
#define EXC_RESV2       0xEC0
#define EXC_IMPL        0xEE0
#define EXC_PMU         0xF00
#define EXC_VECUN       0xF20
#define EXC_VSX         0xF40
#define EXC_FAC         0xF60
#define EXC_HFAC        0xF80
#define EXC_LAST        0xFFF

#define OFF_R0    (0 * 8)
#define OFF_R1    (1 * 8)
#define OFF_R2    (2 * 8)
#define OFF_R3    (3 * 8)
#define OFF_R4    (4 * 8)
#define OFF_R5    (5 * 8)
#define OFF_R6    (6 * 8)
#define OFF_R7    (7 * 8)
#define OFF_R8    (8 * 8)
#define OFF_R9    (9 * 8)
#define OFF_R10   (10 * 8)
#define OFF_R11   (11 * 8)
#define OFF_R12   (12 * 8)
#define OFF_R13   (13 * 8)
#define OFF_R14   (14 * 8)
#define OFF_R15   (15 * 8)
#define OFF_R16   (16 * 8)
#define OFF_R17   (17 * 8)
#define OFF_R18   (18 * 8)
#define OFF_R19   (19 * 8)
#define OFF_R20   (20 * 8)
#define OFF_R21   (21 * 8)
#define OFF_R22   (22 * 8)
#define OFF_R23   (23 * 8)
#define OFF_R24   (24 * 8)
#define OFF_R25   (25 * 8)
#define OFF_R26   (26 * 8)
#define OFF_R27   (27 * 8)
#define OFF_R28   (28 * 8)
#define OFF_R29   (29 * 8)
#define OFF_R30   (30 * 8)
#define OFF_R31   (31 * 8)
#define OFF_LR    (32 * 8)
#define OFF_CTR   (33 * 8)
#define OFF_XER   (34 * 8)
#define OFF_CR    (35 * 8)
#define OFF_HSRR0 (36 * 8)
#define OFF_HSRR1 (37 * 8)
#define OFF_VEC   (38 * 8)
#define SSIZE     (39 * 8)
/*
 * We enter here in HV mode. PCR is in HSPRG0, while
 * HSPRG1 is scratch.
 */
#define DEF_EXC(name, where)			\
	.org CpuDxeVectorsStart + EXC_##name;	\
	ASM_FUNC(CpuDxeVector##name);		\
	mtspr	SPRN_HSPRG1, r1;		\
	mfspr	r1, SPRN_HSPRG0;		\
	ld	r1, PCR_R(CpuDxeUnrecSP, r1);	\
	std	r0, OFF_R0(r1);			\
	li	r0, EXC_##name;			\
	std	r0, OFF_VEC(r1);		\
	std	r2, OFF_R2(r1);			\
	/*					\
	 * Enter with vec#, r0 and r2 saved to	\
	 * unrec_sp. r1 in SPRG1. r2 and r0	\
	 * are scratch.				\
	 */					\
	b where;				\

.p2align 8
.global CpuDxeVectorsStart
CpuDxeVectorsStart:
DEF_EXC(RESET, exc_sys)
DEF_EXC(MC, exc_sys)
DEF_EXC(DSI, exc_sys)
DEF_EXC(DSEG, exc_sys)
DEF_EXC(ISI, exc_sys)
DEF_EXC(ISEG, exc_sys)
DEF_EXC(EXT, exc_sys)
DEF_EXC(ALIGN, exc_sys)
DEF_EXC(PROG, exc_sys)
DEF_EXC(FPU, exc_sys)
DEF_EXC(DEC, exc_sys)
DEF_EXC(HDEC, exc_hv)
DEF_EXC(DOOR, exc_sys)
DEF_EXC(RESV, exc_sys)
DEF_EXC(SC, exc_sys)
DEF_EXC(TRACE, exc_sys)
DEF_EXC(HDSI, exc_hv)
DEF_EXC(HISI, exc_hv)
DEF_EXC(HEA, exc_hv)
DEF_EXC(HMAINT, exc_hv)
DEF_EXC(HDOOR, exc_hv)
DEF_EXC(RESV1, exc_sys)
DEF_EXC(RESV2, exc_sys)
DEF_EXC(IMPL, exc_sys)
DEF_EXC(PMU, exc_sys)
DEF_EXC(VECUN, exc_sys)
DEF_EXC(VSX, exc_sys)
DEF_EXC(FAC, exc_sys)
DEF_EXC(HFAC, exc_hv)
.org CpuDxeVectorsStart + EXC_LAST + 1
exc_hv:
	mfspr	r0, SPRN_HSRR0
	std	r0, OFF_HSRR0(r1)
	mfspr	r0, SPRN_HSRR1
	std	r0, OFF_HSRR1(r1)
	b	exc_cont
exc_sys:
	/*
	 * Saved MSR/PC get stored in
	 * in different registers depending
	 * on the exception type. But we'll
	 * always use hrfid to return, and thus
	 * will always use HSRR0/HSRR1.
	 */
	mfspr	r0, SPRN_SRR0
	std	r0, OFF_HSRR0(r1)
	mfspr	r0, SPRN_SRR1
	std	r0, OFF_HSRR1(r1)
exc_cont:
	/*
	 * If this is a non-recoverable (nested?)
	 * exception, continue saving to top of the
	 * unrecoverable stack r1. Otherwise, use
	 * interrupted r1 stack unless we're coming
	 * from unpriviledged mode. If we switch
	 * away from unrecoverable stack, we need to
	 * copy stashed vec, r0, r2, hsrr0
	 * and hsrr1 values to it.
	 *
	 * r0 - hsrr1.
	 * r1 - unrecoverable exception stack
	 * r2 - scratch.
	 */
	andi.	r2, r0, MSR_RI
	beq-	exc_more_save
	/*
	 * From any mode, continue using whatever
	 * stack we had before the exception, paying
	 * attention the protected zone.
	 */
	mfspr	r2, SPRN_HSPRG1
	subi	r2, r2, STACK_PROTECTED_ZONE
copy_stashed:
	/*
	 * Copy stashed vec, r0, r2, hsrr0 and hsrr1 values.
	 *
	 * r0 - scratch
	 * r1 - unrecoverable stack
	 * r2 - chosen kernel stack
	 * r0 is scratch.
	 */
	subi	r2, r2, SSIZE
	ld	r0, OFF_VEC(r1)
	std	r0, OFF_VEC(r2)
	ld	r0, OFF_R0(r1)
	std	r0, OFF_R0(r2)
	ld	r0, OFF_R2(r1)
	std	r0, OFF_R2(r2)
	ld	r0, OFF_HSRR0(r1)
	std	r0, OFF_HSRR0(r2)
	ld	r0,  OFF_HSRR1(r1)
	std	r0, OFF_HSRR1(r2)
	mr	r1, r2
	/*
	 * End switch to recoverable exception stack,
	 * now common register save code.
	 */
exc_more_save:
	/*
	 * Save more stuff. r0 and r2
	 * are scratch at this point.
	 */
	mfspr	r0, SPRN_HSPRG1
	std	r0, OFF_R1(r1)
	std	r3, OFF_R3(r1)
	std	r4, OFF_R4(r1)
	std	r5, OFF_R5(r1)
	std	r6, OFF_R6(r1)
	std	r7, OFF_R7(r1)
	std	r8, OFF_R8(r1)
	std	r9, OFF_R9(r1)
	std	r10, OFF_R10(r1)
	std	r11, OFF_R11(r1)
	std	r12, OFF_R12(r1)
	std	r13, OFF_R13(r1)
	std	r14, OFF_R14(r1)
	std	r15, OFF_R15(r1)
	std	r16, OFF_R16(r1)
	std	r17, OFF_R17(r1)
	std	r18, OFF_R18(r1)
	std	r19, OFF_R19(r1)
	std	r20, OFF_R20(r1)
	std	r21, OFF_R21(r1)
	std	r22, OFF_R22(r1)
	std	r23, OFF_R23(r1)
	std	r24, OFF_R24(r1)
	std	r25, OFF_R25(r1)
	std	r26, OFF_R26(r1)
	std	r27, OFF_R27(r1)
	std	r28, OFF_R28(r1)
	std	r29, OFF_R29(r1)
	std	r30, OFF_R30(r1)
	std	r31, OFF_R31(r1)
	mflr	r0
	std	r0, OFF_LR(r1)
	mfctr	r0
	std	r0, OFF_CTR(r1)
	mfxer	r0
	std	r0, OFF_XER(r1)
	mfcr	r0
	std	r0, OFF_CR(r1)
	mfspr	r13, SPRN_HSPRG0
	ld	r2, PCR(CpuDxeTOC)
	mr	r3, r1
	subi	r1, r1, STACK_FRAME_MIN
	/*
	 * Jump to handler. This is an indirect jump
	 * because our example is linked at some address
	 * other than 0, while the vectors have to be at 0.
	 */
	LOAD_ADDR(r12, CpuDxeVectorHandler)
	mtlr	r12
	blr
.globl CpuDxeVectorsEnd
CpuDxeVectorsEnd:

ASM_FUNC(CpuDxeRFI)
	/*
	 * Now to restore everything...
	 */
	ld	r0, OFF_HSRR1(r3)
	mtspr	SPRN_HSRR1, r0
	ld	r0, OFF_HSRR0(r3)
	mtspr	SPRN_HSRR0, r0
	ld	r0, OFF_CR(r3)
	mtcr	r0
	ld	r0, OFF_XER(r3)
	mtxer	r0
	ld	r0, OFF_CTR(r3)
	mtctr	r0
	ld	r0, OFF_LR(r3)
	mtlr	r0
	ld	r31, OFF_R31(r3)
	ld	r30, OFF_R30(r3)
	ld	r29, OFF_R29(r3)
	ld	r28, OFF_R28(r3)
	ld	r27, OFF_R27(r3)
	ld	r26, OFF_R26(r3)
	ld	r25, OFF_R25(r3)
	ld	r24, OFF_R24(r3)
	ld	r23, OFF_R23(r3)
	ld	r22, OFF_R22(r3)
	ld	r21, OFF_R21(r3)
	ld	r20, OFF_R20(r3)
	ld	r19, OFF_R19(r3)
	ld	r18, OFF_R18(r3)
	ld	r17, OFF_R17(r3)
	ld	r16, OFF_R16(r3)
	ld	r15, OFF_R15(r3)
	ld	r14, OFF_R14(r3)
	ld	r13, OFF_R13(r3)
	ld	r12, OFF_R12(r3)
	ld	r11, OFF_R11(r3)
	ld	r10, OFF_R10(r3)
	ld	r9, OFF_R9(r3)
	ld	r8, OFF_R8(r3)
	ld	r7, OFF_R7(r3)
	ld	r6, OFF_R6(r3)
	ld	r5, OFF_R5(r3)
	ld	r4, OFF_R4(r3)
	ld	r2, OFF_R2(r3)
	ld	r1, OFF_R1(r3)
	ld	r0, OFF_R0(r3)
	ld	r3, OFF_R3(r3)
	hrfid
