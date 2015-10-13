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

#define SMT_VERY_LOW or 31, 31, 31
#define SMT_MEDIUM   or 2, 2, 2

/**
//
//  Requests CPU to pause for a short period of time.
//
//  Requests CPU to pause for a short period of time. Typically used in MP
//  systems to prevent memory starvation while waiting for a spin lock.
//
VOID
EFIAPI
CpuPause (
  VOID
  )
**/
ASM_FUNC(CpuPause)
        SMT_VERY_LOW
        nop
        nop
        nop
        nop
        nop
        SMT_MEDIUM
        blr
