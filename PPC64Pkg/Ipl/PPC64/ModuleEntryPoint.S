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
#include <Library/PcdLib.h>
#include <AsmMacroLib.h>
#include <Chipset/PPC64.h>
#include <Pcr.h>

	TOC_ANCHOR

ASM_FUNC(_ModuleEntryPoint)
	LOAD_TOC(r2)

	//
	// Enter here with:
	// r3 = FDT
	// r8 = OPAL base
	// r9 = OPAL entry
	//

	// Turn off MSR_EE and MSR_RI.
	mtmsrd 0, 1

	//
	// The FD is followed by the stack, the PCR
	// page and PcdIplRAMRegionSize more bytes for Ipl
	// allocations.
	//
	LOAD_IMM64(r4, FixedPcdGet32(PcdFdBaseAddress))
	LOAD_IMM64(r5, FixedPcdGet32(PcdFdSize))
	add	r5, r4, r5
	LOAD_IMM64(r13, FixedPcdGet32(PcdCPUCorePrimaryStackSize))
	add	r13, r5, r13
	subi	r1, r13, STACK_FRAME_MIN
	addi	r6, r13, PCR_Size
	LOAD_IMM64(r7, FixedPcdGet32(PcdIplRAMRegionSize))
	add	r7, r6, r7

	std	r8, PCR(OPALBase)
	std	r9, PCR(OPALEntry)

	//
	// Go C: r3 = FDT
	//       r4 = IplReservedBottom
	//       r5 = Stack base
	//       r6 = IplFreeMemoryBottom
	//       r7 = IplMemoryTop
	bl	CEntryPoint
	ori	0, 0, 0
	b	.
