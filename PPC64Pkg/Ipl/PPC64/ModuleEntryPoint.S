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
	// Stack is right above the FD, because the FD
	// is not at the very top.
	//
	LOAD_IMM64(r10, FixedPcdGet32(PcdFdBaseAddress))
	LOAD_IMM64(r11, FixedPcdGet32(PcdFdSize))
	add	r10, r10, r11
	LOAD_IMM64(r11, FixedPcdGet32(PcdCPUCorePrimaryStackSize))
	add	r10, r10, r11
	subi	r10, r10, STACK_FRAME_MIN

	LOAD_ADDR(r13, PrimaryPCR)
	std	r2, PCR(Toc)
	std	r4, PCR(OPALBase)
	std	r5, PCR(OPALEntry)

	// go C
	bl	CEntryPoint
	ori	0, 0, 0
	b	.
