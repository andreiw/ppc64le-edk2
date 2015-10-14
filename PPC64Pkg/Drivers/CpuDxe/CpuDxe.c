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

#include "CpuDxe.h"
#include <Pcr.h>
#include <Library/OpalLib.h>
#include <Chipset/PPC64Intrinsics.h>

#define UNRECOVERABLE_EXCEPTION_STACK_PAGES 2

EFI_CPU_INTERRUPT_HANDLER mHDECHandler = NULL;


/**
  This function flushes the range of addresses from Start to Start+Length
  from the processor's data cache. If Start is not aligned to a cache line
  boundary, then the bytes before Start to the preceding cache line boundary
  are also flushed. If Start+Length is not aligned to a cache line boundary,
  then the bytes past Start+Length to the end of the next cache line boundary
  are also flushed. The FlushType of EfiCpuFlushTypeWriteBackInvalidate must be
  supported. If the data cache is fully coherent with all DMA operations, then
  this function can just return EFI_SUCCESS. If the processor does not support
  flushing a range of the data cache, then the entire data cache can be flushed.

  @param  This             The EFI_CPU_ARCH_PROTOCOL instance.
  @param  Start            The beginning physical address to flush from the processor's data
                           cache.
  @param  Length           The number of bytes to flush from the processor's data cache. This
                           function may flush more bytes than Length specifies depending upon
                           the granularity of the flush operation that the processor supports.
  @param  FlushType        Specifies the type of flush operation to perform.

  @retval EFI_SUCCESS           The address range from Start to Start+Length was flushed from
                                the processor's data cache.
  @retval EFI_UNSUPPORTEDT      The processor does not support the cache flush type specified
                                by FlushType.
  @retval EFI_DEVICE_ERROR      The address range from Start to Start+Length could not be flushed
                                from the processor's data cache.

**/
EFI_STATUS
EFIAPI
CpuFlushCpuDataCache (
  IN EFI_CPU_ARCH_PROTOCOL           *This,
  IN EFI_PHYSICAL_ADDRESS            Start,
  IN UINT64                          Length,
  IN EFI_CPU_FLUSH_TYPE              FlushType
  )
{

  switch (FlushType) {
    case EfiCpuFlushTypeWriteBack:
      break;
    case EfiCpuFlushTypeInvalidate:
      break;
    case EfiCpuFlushTypeWriteBackInvalidate:
      break;
    default:
      return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}


/**
  This function enables interrupt processing by the processor.

  @param  This             The EFI_CPU_ARCH_PROTOCOL instance.

  @retval EFI_SUCCESS           Interrupts are enabled on the processor.
  @retval EFI_DEVICE_ERROR      Interrupts could not be enabled on the processor.

**/
EFI_STATUS
EFIAPI
CpuEnableInterrupt (
  IN EFI_CPU_ARCH_PROTOCOL          *This
  )
{
  mtmsrd(mfmsr() | MSR_EE, 1);
  return EFI_SUCCESS;
}


/**
  This function disables interrupt processing by the processor.

  @param  This             The EFI_CPU_ARCH_PROTOCOL instance.

  @retval EFI_SUCCESS           Interrupts are disabled on the processor.
  @retval EFI_DEVICE_ERROR      Interrupts could not be disabled on the processor.

**/
EFI_STATUS
EFIAPI
CpuDisableInterrupt (
  IN EFI_CPU_ARCH_PROTOCOL          *This
  )
{
  mtmsrd(mfmsr() & ~MSR_EE, 1);
  return EFI_SUCCESS;
}


/**
  This function retrieves the processor's current interrupt state a returns it in
  State. If interrupts are currently enabled, then TRUE is returned. If interrupts
  are currently disabled, then FALSE is returned.

  @param  This             The EFI_CPU_ARCH_PROTOCOL instance.
  @param  State            A pointer to the processor's current interrupt state. Set to TRUE if
                           interrupts are enabled and FALSE if interrupts are disabled.

  @retval EFI_SUCCESS           The processor's current interrupt state was returned in State.
  @retval EFI_INVALID_PARAMETER State is NULL.

**/
EFI_STATUS
EFIAPI
CpuGetInterruptState (
  IN  EFI_CPU_ARCH_PROTOCOL         *This,
  OUT BOOLEAN                       *State
  )
{
  if (State == NULL) {
    return EFI_INVALID_PARAMETER;
  }


  *State = ((mfmsr() & MSR_EE) != 0);
  return EFI_SUCCESS;
}


/**
  This function generates an INIT on the processor. If this function succeeds, then the
  processor will be reset, and control will not be returned to the caller. If InitType is
  not supported by this processor, or the processor cannot programmatically generate an
  INIT without help from external hardware, then EFI_UNSUPPORTED is returned. If an error
  occurs attempting to generate an INIT, then EFI_DEVICE_ERROR is returned.

  @param  This             The EFI_CPU_ARCH_PROTOCOL instance.
  @param  InitType         The type of processor INIT to perform.

  @retval EFI_SUCCESS           The processor INIT was performed. This return code should never be seen.
  @retval EFI_UNSUPPORTED       The processor INIT operation specified by InitType is not supported
                                by this processor.
  @retval EFI_DEVICE_ERROR      The processor INIT failed.

**/
EFI_STATUS
EFIAPI
CpuInit (
  IN EFI_CPU_ARCH_PROTOCOL           *This,
  IN EFI_CPU_INIT_TYPE               InitType
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
CpuRegisterInterruptHandler (
  IN EFI_CPU_ARCH_PROTOCOL          *This,
  IN EFI_EXCEPTION_TYPE             InterruptType,
  IN EFI_CPU_INTERRUPT_HANDLER      InterruptHandler
  )
{
  if (InterruptType != EXCEPT_PPC64_HDEC) {
    return EFI_UNSUPPORTED;
  }

  if (InterruptHandler != NULL &&
      mHDECHandler != NULL) {
    return EFI_ALREADY_STARTED;
  }

  mHDECHandler = InterruptHandler;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
CpuGetTimerValue (
  IN  EFI_CPU_ARCH_PROTOCOL          *This,
  IN  UINT32                         TimerIndex,
  OUT UINT64                         *TimerValue,
  OUT UINT64                         *TimerPeriod   OPTIONAL
  )
{
  return EFI_UNSUPPORTED;
}

/**
  This function modifies the attributes for the memory region specified by BaseAddress and
  Length from their current attributes to the attributes specified by Attributes.

  @param  This             The EFI_CPU_ARCH_PROTOCOL instance.
  @param  BaseAddress      The physical address that is the start address of a memory region.
  @param  Length           The size in bytes of the memory region.
  @param  Attributes       The bit mask of attributes to set for the memory region.

  @retval EFI_SUCCESS           The attributes were set for the memory region.
  @retval EFI_ACCESS_DENIED     The attributes for the memory resource range specified by
                                BaseAddress and Length cannot be modified.
  @retval EFI_INVALID_PARAMETER Length is zero.
  @retval EFI_OUT_OF_RESOURCES  There are not enough system resources to modify the attributes of
                                the memory resource range.
  @retval EFI_UNSUPPORTED       The processor does not support one or more bytes of the memory
                                resource range specified by BaseAddress and Length.
                                The bit mask of attributes is not support for the memory resource
                                range specified by BaseAddress and Length.

**/
EFI_STATUS
EFIAPI
CpuSetMemoryAttributes (
  IN EFI_CPU_ARCH_PROTOCOL    *This,
  IN EFI_PHYSICAL_ADDRESS      BaseAddress,
  IN UINT64                    Length,
  IN UINT64                    EfiAttributes
  )
{
  return EFI_UNSUPPORTED;
}

//
// Globals used to initialize the protocol
//
EFI_HANDLE            mCpuHandle = NULL;
EFI_CPU_ARCH_PROTOCOL mCpu = {
  CpuFlushCpuDataCache,
  CpuEnableInterrupt,
  CpuDisableInterrupt,
  CpuGetInterruptState,
  CpuInit,
  CpuRegisterInterruptHandler,
  CpuGetTimerValue,
  CpuSetMemoryAttributes,
  0,          // NumberOfTimers
  4,          // DmaBufferAlignment
};

VOID
CpuDxeVectorHandler (
  IN EFI_SYSTEM_CONTEXT_PPC64 *Context
  )
{
  if (Context->Vec == EXCEPT_PPC64_HDEC) {
    if (mHDECHandler != NULL) {
      mHDECHandler(Context->Vec, (EFI_SYSTEM_CONTEXT) Context);
      CpuDxeRFI(Context);
    }
  }

  DEBUG((EFI_D_ERROR, "Unknown exception 0x%x for 0x%016lx!\n",
         Context->Vec, Context->LR));
  ASSERT(FALSE);
  while (1);
}

EFI_PHYSICAL_ADDRESS
CpuDxeMyTOC (
  VOID
  )
{
  register UINT64 R2 asm("r2");
  return R2;
}

EFI_STATUS
CpuDxeInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS Status;
  OPAL_STATUS OpalStatus;
  EFI_PHYSICAL_ADDRESS UnrecStack;
  EFI_PHYSICAL_ADDRESS Vectors;

  PcrGet()->CpuDxeTOC = CpuDxeMyTOC();

  /*
   * MSR.ILE controls supervisor exception endianness. OPAL
   * takes care of setting the hypervisor exception endianness
   * bit in an implementation-neutral fashion. Mambo systemsim
   * doesn't seem to report a PVR version that Skiboot recognises
   * as supporting HID0.HILE, so we'll manually set it until
   * the skiboot patch goes in.
   */
  OpalStatus = OpalReinitCPUs(OPAL_REINIT_CPUS_HILE_LE);
  if (OpalStatus != OPAL_SUCCESS) {
    DEBUG((EFI_D_INFO, "OPAL claims no HILE supported, pretend to know better...\n"));
    UINT64 HID0 = GET_HID0();
    SET_HID0(HID0 | HID0_HILE);
  }

  /*
   * Force external interrupts to HV mode.
   */
  SET_LPCR((GET_LPCR() & ~LPCR_LPES) | LPCR_ILE);

  /*
   * Allocate the stack we'll use when servicing exceptions
   * with MSR_RI 0.
   */
  Status = gBS->AllocatePages (AllocateAnyPages, EfiBootServicesData,
                               UNRECOVERABLE_EXCEPTION_STACK_PAGES,
                               &UnrecStack);
  ASSERT_EFI_ERROR (Status);
  PcrGet()->CpuDxeUnrecSP = UnrecStack;

  Vectors = PcrGet()->PhysVectorsBase;
  ASSERT((UINTN) &CpuDxeVectorsEnd -
         (UINTN) &CpuDxeVectorsStart <= PcrGet()->PhysVectorsSize);

  CopyMem((void *) Vectors, (VOID *) &CpuDxeVectorsStart,
          ((UINTN) &CpuDxeVectorsEnd - (UINTN) &CpuDxeVectorsStart));

  /*
   * Vectors expect HSPRG0 to contain the pointer to KPCR,
   * HSPRG1 is scratch.
   */
  SET_HSPRG0((UINTN) PcrGet());

  /*
   * Context is now recoverable.
   */
  mtmsrd(MSR_RI, 1);

  Status = gBS->InstallMultipleProtocolInterfaces (
                &mCpuHandle,
                &gEfiCpuArchProtocolGuid, &mCpu,
                NULL
                );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
