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

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PcdLib.h>
#include <PiDxe.h>
#include <Library/DxeServicesLib.h>
#include <Library/HobLib.h>
#include <libfdt.h>

#include <Protocol/BlockMmio.h>

#include <Guid/Fdt.h>
#include <Guid/FdtHob.h>

typedef struct {
  MEMMAP_DEVICE_PATH MemMapDevPath;
  EFI_DEVICE_PATH_PROTOCOL EndDevPath;
} INITRD_DEVICE_PATH;

INITRD_DEVICE_PATH InitrdDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
        HW_MEMMAP_DP,
      {
        sizeof (MEMMAP_DEVICE_PATH),
        0
      }
    },
    EfiMemoryMappedIO,
    0,
    0,
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      sizeof (EFI_DEVICE_PATH_PROTOCOL),
      0
    }
  }
};


/**
  Main entry point of the FDT bus driver. At some point this is going
  to do something a bit more intelligent.

  @param[in]  ImageHandle   The firmware allocated handle for the present driver
                            UEFI image.
  @param[in]  *SystemTable  A pointer to the EFI System table.

  @retval  EFI_SUCCESS           The driver was initialized.
  @retval  EFI_OUT_OF_RESOURCES  The "End of DXE" event could not be allocated or
                                 there was not enough memory in pool to install
                                 the Shell Dynamic Command protocol.
  @retval  EFI_LOAD_ERROR        Unable to add the HII package.

**/
EFI_STATUS
FdtBusDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  VOID *Hob;
  VOID *DeviceTreeBase;
  BLOCK_MMIO_PROTOCOL *BlockMmio;
  EFI_STATUS Status;
  EFI_HANDLE Handle;
  INT32 Node;
  INT32 PropertyLen;
  UINT64 InitrdStart;
  UINT64 InitrdEnd;
  const struct fdt_property *Property;

  Hob = GetFirstGuidHob(&gFdtHobGuid);
  if (Hob == NULL || GET_GUID_HOB_DATA_SIZE (Hob) != sizeof (UINT64)) {
    return EFI_NOT_FOUND;
  }

  DeviceTreeBase = (VOID *)(UINTN)*(UINT64 *)GET_GUID_HOB_DATA (Hob);
  if (fdt_check_header (DeviceTreeBase) != 0) {
    DEBUG ((EFI_D_ERROR, "%a: No DTB found @ 0x%p\n", __FUNCTION__,
            DeviceTreeBase));
    return EFI_NOT_FOUND;
  }

  //
  // Install the FDT into the Configuration Table
  //
  Status = gBS->InstallConfigurationTable (
                  &gFdtTableGuid,
                  DeviceTreeBase
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "couldn't install DTB into configuration table: %r\n",
            Status));
    return Status;
  }

  Node = fdt_path_offset (DeviceTreeBase, "/chosen");
  if (Node < 0) {
    DEBUG ((EFI_D_ERROR, "/chosen not found?\n"));
    return EFI_NOT_FOUND;
  }

  Property = fdt_get_property (DeviceTreeBase,
                               Node,
                               "linux,initrd-start",
                               &PropertyLen);
  if (Property == NULL ||
      PropertyLen < sizeof(UINT32) ||
      PropertyLen > sizeof(UINT64)) {
    DEBUG ((EFI_D_ERROR, "no or wrong linux,initrd-start"));
    return EFI_NOT_FOUND;
  }
  if (PropertyLen == sizeof(UINT32)) {
    InitrdStart = fdt32_to_cpu (*(UINT32 *) Property->data);
  } else {
    InitrdStart = fdt64_to_cpu (*(UINT64 *) Property->data);
  }

  Property = fdt_get_property (DeviceTreeBase,
                               Node,
                               "linux,initrd-end",
                               &PropertyLen);
  if (Property == NULL ||
      PropertyLen < sizeof(UINT32) ||
      PropertyLen > sizeof(UINT64)) {
    DEBUG ((EFI_D_ERROR, "no or wrong linux,initrd-end"));
    return EFI_NOT_FOUND;
  }
  if (PropertyLen == sizeof(UINT32)) {
    InitrdEnd = fdt32_to_cpu (*(UINT32 *) Property->data);
  } else {
    InitrdEnd = fdt64_to_cpu (*(UINT64 *) Property->data);
  }

  if ((InitrdEnd - InitrdStart) == 0) {
    return EFI_NOT_FOUND;
  }
  
  BlockMmio = AllocateZeroPool (sizeof (*BlockMmio));
  ASSERT (BlockMmio != NULL);

  BlockMmio->Revision = 0;
  BlockMmio->Media = AllocateZeroPool (sizeof (*BlockMmio->Media));
  ASSERT (BlockMmio->Media != NULL);

  BlockMmio->Media->MediaId = 0;
  BlockMmio->Media->RemovableMedia = FALSE;
  BlockMmio->Media->MediaPresent = TRUE;
  BlockMmio->Media->LogicalPartition = FALSE;
  BlockMmio->Media->ReadOnly = FALSE;
  BlockMmio->Media->WriteCaching = FALSE;
  BlockMmio->Media->BlockSize = 512;
  BlockMmio->Media->IoAlign = 0;
  BlockMmio->Media->LastBlock = (InitrdEnd - InitrdStart) >> 9;
  BlockMmio->BaseAddress = InitrdStart;

  InitrdDevicePath.MemMapDevPath.StartingAddress = InitrdStart;
  InitrdDevicePath.MemMapDevPath.EndingAddress = InitrdEnd;
  
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (&Handle,
                                                   &gBlockMmioProtocolGuid,
                                                   BlockMmio,
                                                   &gEfiDevicePathProtocolGuid,
                                                   &InitrdDevicePath,
                                                   NULL);
  ASSERT (!EFI_ERROR (Status));
  
  return EFI_SUCCESS;
}
