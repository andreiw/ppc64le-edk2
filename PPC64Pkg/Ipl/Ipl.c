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

#include <PiPei.h>
#include <Library/OpalLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PrePiLib.h>
#include <Library/HobLib.h>
#include <Library/PrePiHobListPointerLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Ppi/GuidedSectionExtraction.h>
#include <Guid/LzmaDecompress.h>
#include <Guid/FdtHob.h>
#include <libfdt.h>
#include <Endian.h>
#include <Pcr.h>
#include "LzmaDecompress.h"

EFI_STATUS
EFIAPI
ExtractGuidedSectionLibConstructor (
  VOID
  );

EFI_STATUS
EFIAPI
LzmaDecompressLibConstructor (
  VOID
  );

PCR PrimaryPCR;

VOID
Log (
	IN CONST CHAR16 *FormatString,
	...
	)
{
	VA_LIST Marker;
	UINTN   Count;
	CHAR8 Buffer[100];

	VA_START (Marker, FormatString);
	Count = AsciiVSPrintUnicodeFormat (Buffer, sizeof(Buffer),
					  FormatString, Marker);
	VA_END (Marker);

	SerialPortWrite ((UINT8 *) Buffer, Count);
}


UINT64
InitializeSystemMemory (
	IN EFI_PHYSICAL_ADDRESS FDTBase
	)
{
	INT32        Node, Prev;
	UINT64       NewBase;
	UINT64       NewSize;
	CONST CHAR8  *Type;
	INT32        Len;
	CONST UINT64 *RegProp;

	NewBase = 0;
	NewSize = 0;

	VOID *DeviceTreeBase = (VOID *) FDTBase;

	ASSERT (fdt_check_header (DeviceTreeBase) == 0);

	//
	// Look for a memory node
	//
	for (Prev = 0;; Prev = Node) {
		Node = fdt_next_node (DeviceTreeBase, Prev, NULL);
		if (Node < 0) {
			break;
		}

		//
		// Check for memory node
		//
		Type = fdt_getprop (DeviceTreeBase, Node, "device_type", &Len);
		if (Type && AsciiStrnCmp (Type, "memory", Len) == 0) {
			//
			// Get the 'reg' property of this node. For now, we will assume
			// two 8 byte quantities for base and size, respectively.
			//
			RegProp = fdt_getprop (DeviceTreeBase, Node, "reg", &Len);
			if (RegProp != 0 && Len == (2 * sizeof (UINT64))) {
				NewBase = fdt64_to_cpu (ReadUnaligned64 (RegProp));
				NewSize = fdt64_to_cpu (ReadUnaligned64 (RegProp + 1));

				//
				// Make sure the start of DRAM matches our expectation
				//
				ASSERT (FixedPcdGet64 (PcdSystemMemoryBase) == NewBase);
				PatchPcdSet64 (PcdSystemMemorySize, NewSize);
			} else {
				DEBUG ((EFI_D_ERROR, "%a: Failed to parse FDT memory node\n",
					__FUNCTION__));
			}
			break;
		}
	}

	return NewSize;
}


VOID
BuildMemoryHobs (
	IN VOID *FDT,
	IN EFI_PHYSICAL_ADDRESS UefiMemoryBase,
	IN UINT64 UefiMemorySize
	)
{
	UINTN Index;
	UINTN NumResv;
	EFI_PHYSICAL_ADDRESS StartReserved = -1;
	EFI_PHYSICAL_ADDRESS EndReserved = 0;
	EFI_RESOURCE_ATTRIBUTE_TYPE ResourceAttributes;

	ResourceAttributes = (
		EFI_RESOURCE_ATTRIBUTE_PRESENT |
		EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
		EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
		EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
		EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
		EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE |
		EFI_RESOURCE_ATTRIBUTE_TESTED
		);

	NumResv = fdt_num_mem_rsv(FDT);

	/*
	 * These ranges don't have to be sorted and aren't.
	 * For now do the saddest thing ever and just skip
	 * the entire block. Will have to revisit this again...
	 */
	for (Index = 0; Index < NumResv; Index++) {
		UINT64 Size;
		EFI_PHYSICAL_ADDRESS Addr;
		if (fdt_get_mem_rsv(FDT, Index, &Addr, &Size) != 0) {
			break;
		}

		DEBUG((EFI_D_INFO, "/memreserve/ 0x%lx 0x%lx;\n",
		       Addr, Size));

		if (Addr < StartReserved) {
			StartReserved = Addr;
		}

		if (Addr + Size > EndReserved) {
			EndReserved = Addr + Size;
		}
	}

	if (StartReserved < EndReserved) {
		DEBUG((EFI_D_INFO, "FIXME: Going to skip all 0x%lx-0x%lx\n",
		       StartReserved, EndReserved));
		BuildResourceDescriptorHob (
			EFI_RESOURCE_SYSTEM_MEMORY,
			ResourceAttributes,
			UefiMemoryBase,
			StartReserved - UefiMemoryBase
			);
		BuildResourceDescriptorHob (
			EFI_RESOURCE_SYSTEM_MEMORY,
			ResourceAttributes,
			EndReserved,
			(UefiMemoryBase + UefiMemorySize) - EndReserved
			);
	} else {
		BuildResourceDescriptorHob (
			EFI_RESOURCE_SYSTEM_MEMORY,
			ResourceAttributes,
			UefiMemoryBase,
			UefiMemorySize
			);
	}
}


VOID
CEntryPoint (
	IN EFI_PHYSICAL_ADDRESS FDTBase,
	IN EFI_PHYSICAL_ADDRESS StackBase,
	IN EFI_PHYSICAL_ADDRESS PHITBase
	)
{
	EFI_STATUS Status;
	UINTN FDTSize;
	UINT64 *FDTHobData;
	EFI_HOB_HANDOFF_INFO_TABLE *HobList;

	SerialPortInitialize ();

	Log (L"PPC64LE UEFI firmware (version %s built at %a on %a)\n",
	    (CHAR16*) PcdGetPtr (PcdFirmwareVersionString), __TIME__, __DATE__);

	ASSERT (fdt_check_header ((VOID *) FDTBase) == 0);
	FDTSize = fdt_totalsize ((VOID *) FDTBase);

	DEBUG((EFI_D_INFO, "FDT   @ 0x%lx-0x%lx\n",
	       FDTBase, FDTBase + FDTSize));
	DEBUG((EFI_D_INFO, "Stack @ 0x%lx-0x%lx\n",
	       StackBase, StackBase +
	       PcdGet32 (PcdCPUCorePrimaryStackSize)));
	DEBUG((EFI_D_INFO, "Hobs  @ 0x%lx-0x%lx\n",
	       PHITBase, PHITBase +
	       PcdGet32 (PcdPHITRegionSize)));

	InitializeSystemMemory (FDTBase);

	DEBUG ((EFI_D_INFO, "System RAM @ 0x%lx - 0x%lx\n",
		PcdGet64 (PcdSystemMemoryBase),
		PcdGet64 (PcdSystemMemoryBase) +
		PcdGet64 (PcdSystemMemorySize) - 1));

	HobList = HobConstructor (
		(VOID *) PHITBase,
		PcdGet32 (PcdPHITRegionSize),
		(VOID *) PHITBase,
		(VOID *) StackBase);
	PrePeiSetHobList (HobList);

	BuildMemoryHobs ((VOID *) FDTBase, PcdGet64 (PcdSystemMemoryBase),
			 PcdGet64 (PcdSystemMemorySize));

	BuildStackHob (StackBase, PcdGet32 (PcdCPUCorePrimaryStackSize));
	BuildCpuHob (PcdGet8 (PcdPrePiCpuMemorySize), PcdGet8 (PcdPrePiCpuIoSize));
	SetBootMode (BOOT_WITH_FULL_CONFIGURATION);
	BuildFvHob (PcdGet64 (PcdFvBaseAddress), PcdGet32 (PcdFvSize));
	FDTHobData = BuildGuidHob (&gFdtHobGuid, sizeof *FDTHobData);
	*FDTHobData = FDTBase;

	// SEC phase needs to run library constructors by hand.
	ExtractGuidedSectionLibConstructor ();

	// Build HOBs to pass up our version of stuff the DXE Core needs to save space
	BuildPeCoffLoaderHob ();
	BuildExtractSectionHob (
		&gLzmaCustomDecompressGuid,
		LzmaGuidedSectionGetInfo,
		LzmaGuidedSectionExtraction
		);

	// Load the DXE Core and transfer control to it
	Status = LoadDxeCoreFromFv (NULL, 0);
	ASSERT_EFI_ERROR (Status);
}

