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

	mr	r24, r4
	mr	r25, r5

	//
	// The FD is followed by a block of memory used by
	// Ipl allocations. The top of that is the PCR
	// page, pointed to by r13, and right below that
	// is the stack.
	//
	LOAD_IMM64(r5, FixedPcdGet32(PcdFdBaseAddress))
	LOAD_IMM64(r6, FixedPcdGet32(PcdFdSize))
	add	r5, r5, r6
	LOAD_IMM64(r1, FixedPcdGet32(PcdIplRAMRegionSize) - PCR_Size)
	add	r13, r5, r1
	LOAD_IMM64(r4, FixedPcdGet32(PcdCPUCorePrimaryStackSize))
	sub	r4, r13, r4
	subi	r1, r13, STACK_FRAME_MIN

	std	r24, PCR(OPALBase)
	std	r25, PCR(OPALEntry)

	//
	// Go C: r3 = FDT, r4 = stack base, r5 = Ipl RAM.
	//
	bl	CEntryPoint
	ori	0, 0, 0
	b	.
