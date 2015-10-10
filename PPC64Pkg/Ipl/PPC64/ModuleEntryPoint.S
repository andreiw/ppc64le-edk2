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

	LOAD_ADDR(r13, PrimaryPCR)
	std	r2, PCR(Toc)
	std	r4, PCR(OPALBase)
	std	r5, PCR(OPALEntry)

	LOAD_IMM64(r5, FixedPcdGet32(PcdFdBaseAddress))
	LOAD_IMM64(r6, FixedPcdGet32(PcdFdSize))
	add	r5, r5, r6
	LOAD_IMM64(r1, FixedPcdGet32(PcdPHITRegionSize))
	add	r1, r5, r1
	LOAD_IMM64(r4, FixedPcdGet32(PcdCPUCorePrimaryStackSize))
	sub	r4, r1, r4
	subi	r1, r1, STACK_FRAME_MIN


	//
	// Go C: r3 = FDT, r4 = stack base, r5 = PHIT memory
	//
	bl	CEntryPoint
	ori	0, 0, 0
	b	.
