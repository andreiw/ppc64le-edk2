/** @file

  Copyright (c) 2015, Andrei Warkentin <andrey.warkentin@gmail.com>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <Base.h>
#include <Library/PcdLib.h>

VOID
EFIAPI
InvalidateInstructionCache (
  VOID
  )
{
}

VOID
EFIAPI
InvalidateDataCache (
  VOID
  )
{
}

VOID *
EFIAPI
InvalidateInstructionCacheRange (
  IN      VOID                      *Address,
  IN      UINTN                     Length
  )
{
  return Address;
}

VOID
EFIAPI
WriteBackInvalidateDataCache (
  VOID
  )
{
}

VOID *
EFIAPI
WriteBackInvalidateDataCacheRange (
  IN      VOID                      *Address,
  IN      UINTN                     Length
  )
{
  return Address;
}

VOID
EFIAPI
WriteBackDataCache (
  VOID
  )
{
}

VOID *
EFIAPI
WriteBackDataCacheRange (
  IN      VOID                      *Address,
  IN      UINTN                     Length
  )
{
  return Address;
}

VOID *
EFIAPI
InvalidateDataCacheRange (
  IN      VOID                      *Address,
  IN      UINTN                     Length
  )
{
  return Address;
}
