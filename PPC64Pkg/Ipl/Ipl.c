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
#include <Chipset/PPC64Intrinsics.h>
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

VOID
CpuInit (
	VOID
	)
{
	smt_medium ();
	hdec_disable ();

	PcrGet()->PhysVectorsBase = 0;
	PcrGet()->PhysVectorsSize = 0x2000;
}

VOID
ParseFDT (
	IN VOID *DeviceTreeBase
	)
{
	INT32        Node, Prev;
	UINT64       NewBase;
	UINT64       NewSize;
	UINT64       MemBase;
	UINT64       MemSize;
	CONST CHAR8  *Type;
	INT32        Len;
	CONST UINT64 *RegProp;
	CONST UINT32 *RegProp32;

	NewBase = 0;
	NewSize = 0;
	MemBase = 0;
	MemSize = 0;

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
				// We expect the memory nodes to be sorted. I
				// hope that's a normal assumption to make :(.
				//
				// This also assumes that no discontinuities occur.
				//
				ASSERT (NewBase == MemBase + MemSize);
				MemSize += NewSize;
			} else {
				DEBUG ((EFI_D_ERROR, "%a: Failed to parse FDT memory node\n",
					__FUNCTION__));
			}
		} else if (Type && AsciiStrnCmp (Type, "cpu", Len) == 0) {
			RegProp32 = fdt_getprop (DeviceTreeBase, Node,
						 "timebase-frequency", NULL);
			PcrGet()->TBFreq = fdt32_to_cpu (ReadUnaligned32 (RegProp32));
			DEBUG ((EFI_D_INFO, "TB %lu Hz\n", PcrGet()->TBFreq));
		}
	}

	ASSERT (FixedPcdGet64 (PcdSystemMemoryBase) == MemBase);
	PatchPcdSet64 (PcdSystemMemorySize, MemSize);
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
	INT32 Node;
	INT32 PropertyLen;
	UINT64 InitrdStart;
	UINT64 InitrdEnd;
	const struct fdt_property *Property;

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

	BuildResourceDescriptorHob (
		EFI_RESOURCE_SYSTEM_MEMORY,
		ResourceAttributes,
		UefiMemoryBase,
		UefiMemorySize
		);

	NumResv = fdt_num_mem_rsv(FDT);

	for (Index = 0; Index < NumResv; Index++) {
		UINT64 Size;
		EFI_PHYSICAL_ADDRESS Addr;
		if (fdt_get_mem_rsv(FDT, Index, &Addr, &Size) != 0) {
			break;
		}

		Size = ALIGN_VALUE(Size, EFI_PAGE_SIZE);
		DEBUG((EFI_D_INFO, "/memreserve/ [0x%lx-0x%lx) -> RT\n",
		       Addr, Addr + Size));
		BuildMemoryAllocationHob (Addr,Size,
					  EfiRuntimeServicesData);
	}

	Node = fdt_path_offset (FDT, "/chosen");
	ASSERT(Node >= 0);
	if (Node < 0) {
		DEBUG ((EFI_D_ERROR, "/chosen not found?\n"));
		return;
	}

	Property = fdt_get_property (FDT, Node,
				     "linux,initrd-start",
				     &PropertyLen);
	if (Property == NULL ||
	    PropertyLen < sizeof(UINT32) ||
	    PropertyLen > sizeof(UINT64)) {
		DEBUG ((EFI_D_ERROR, "no or wrong linux,initrd-start"));
		return;
	}

	if (PropertyLen == sizeof(UINT32)) {
		InitrdStart = fdt32_to_cpu (*(UINT32 *) Property->data);
	} else {
		InitrdStart = fdt64_to_cpu (*(UINT64 *) Property->data);
	}

	Property = fdt_get_property (FDT, Node,
				     "linux,initrd-end",
				     &PropertyLen);
	if (Property == NULL ||
	    PropertyLen < sizeof(UINT32) ||
	    PropertyLen > sizeof(UINT64)) {
		DEBUG ((EFI_D_ERROR, "no or wrong linux,initrd-end"));
		return;
	}

	if (PropertyLen == sizeof(UINT32)) {
		InitrdEnd = fdt32_to_cpu (*(UINT32 *) Property->data);
	} else {
		InitrdEnd = fdt64_to_cpu (*(UINT64 *) Property->data);
	}

	if (InitrdEnd - InitrdStart > 0) {
		BuildMemoryAllocationHob (InitrdStart,
					  InitrdEnd - InitrdStart,
					  EfiBootServicesData);
	}
}


VOID
BuildFdtHob(
	IN VOID *FDTBase
	)
{
	UINTN FDTSize;
	UINTN FDTPages;
	VOID *SafeFDT;
	UINT64 *FDTHobData;

	FDTSize = fdt_totalsize (FDTBase);

	FDTPages = EFI_SIZE_TO_PAGES (FDTSize);
	SafeFDT = AllocatePages (FDTPages);
	ASSERT (SafeFDT != NULL);
	fdt_open_into (FDTBase, SafeFDT, FDTSize);

	DEBUG((EFI_D_INFO, "FDT @ 0x%lx-0x%lx\n",
	       (UINTN) SafeFDT, (UINTN) SafeFDT + FDTSize));

	FDTHobData = BuildGuidHob (&gFdtHobGuid, sizeof *FDTHobData);
	*FDTHobData = (UINT64) SafeFDT;
}

VOID
CEntryPoint (
	IN EFI_PHYSICAL_ADDRESS FDTBase,
	IN EFI_PHYSICAL_ADDRESS IplReservedBottom,
	IN EFI_PHYSICAL_ADDRESS StackBase,
	IN EFI_PHYSICAL_ADDRESS IplFreeMemoryBottom,
	IN EFI_PHYSICAL_ADDRESS IplMemoryTop
	)
{
	EFI_STATUS Status;
	EFI_HOB_HANDOFF_INFO_TABLE *HobList;

	SerialPortInitialize ();

	Log (L"PPC64LE UEFI firmware (%s built at %a on %a)\n",
	    (CHAR16*) PcdGetPtr (PcdFirmwareVersionString), __TIME__, __DATE__);

	CpuInit ();

	ParseFDT ((VOID *) FDTBase);
	DEBUG ((EFI_D_INFO, "System RAM @ 0x%lx - 0x%lx\n",
		PcdGet64 (PcdSystemMemoryBase),
		PcdGet64 (PcdSystemMemoryBase) +
		PcdGet64 (PcdSystemMemorySize) - 1));

	DEBUG((EFI_D_INFO, "Ipl total @ 0x%lx-0x%lx\n",
	       IplReservedBottom, IplMemoryTop));
	DEBUG((EFI_D_INFO, "Ipl used  @ 0x%lx-0x%lx\n",
	       IplReservedBottom, IplFreeMemoryBottom));
	DEBUG((EFI_D_INFO, "Stack     @ 0x%lx-0x%lx\n",
	       StackBase, StackBase +
	       PcdGet32 (PcdCPUCorePrimaryStackSize)));
	DEBUG((EFI_D_INFO, "Ipl free  @ 0x%lx-0x%lx\n",
	       IplFreeMemoryBottom, IplMemoryTop));

	/*
	 * Claim PHIT memory entire system RAM range:
	 * this lets us use BuildMemoryAllocationHob
	 * to cover reserved allocations.
	 */
	HobList = HobConstructor (
		(VOID *) 0,
		PcdGet64 (PcdSystemMemoryBase) +
		PcdGet64 (PcdSystemMemorySize),
		(VOID *) IplFreeMemoryBottom,
		(VOID *) IplMemoryTop);
	PrePeiSetHobList (HobList);

	/*
	 * [IplReservedBottom, IplFreeMemoryBottom) is
	 * RAM reserved for FD, stack and PCR.
	 */
	BuildMemoryAllocationHob (IplReservedBottom,
				  IplFreeMemoryBottom - IplReservedBottom,
				  EfiBootServicesData);

	/*
	 * Reserve CPU vectors space.
	 */
	BuildMemoryAllocationHob (PcrGet()->PhysVectorsBase,
				  PcrGet()->PhysVectorsSize,
				  EfiBootServicesCode);

	/*
	 * Copy and reserve FDT.
	 */
	BuildFdtHob((VOID *) FDTBase);

	/*
	 * Describe system RAM and reserved OPAL ranges from FDT.
	 */
	BuildMemoryHobs ((VOID *) FDTBase, PcdGet64 (PcdSystemMemoryBase),
			 PcdGet64 (PcdSystemMemorySize));

	/*
	 * DXE stack (which is our stack as well before we transfer control).
	 */
	BuildStackHob (StackBase, PcdGet32 (PcdCPUCorePrimaryStackSize));

	BuildCpuHob (PcdGet8 (PcdPrePiCpuMemorySize), PcdGet8 (PcdPrePiCpuIoSize));
	SetBootMode (BOOT_WITH_FULL_CONFIGURATION);
	BuildFvHob (PcdGet64 (PcdFvBaseAddress), PcdGet32 (PcdFvSize));

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

