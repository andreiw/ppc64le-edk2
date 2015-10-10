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

#include "BaseLibInternals.h"

UINTN
EFIAPI
SetJump (
	IN BASE_LIBRARY_JUMP_BUFFER  *JumpBuffer
	)
{
	ASSERT(0);
	return 0;
}

VOID
EFIAPI
InternalLongJump (
	IN BASE_LIBRARY_JUMP_BUFFER  *JumpBuffer,
	IN UINTN                     Value
	)
{
	ASSERT(0);
}
