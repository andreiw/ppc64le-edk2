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

#ifndef __OPAL_LIB_H__
#define __OPAL_LIB_H__

typedef INTN OPAL_STATUS;

#define OPAL_SUCCESS            0
#define OPAL_PARAMETER          -1
#define OPAL_BUSY               -2
#define OPAL_PARTIAL            -3
#define OPAL_CONSTRAINED        -4
#define OPAL_CLOSED             -5
#define OPAL_HARDWARE           -6
#define OPAL_UNSUPPORTED        -7
#define OPAL_PERMISSION         -8
#define OPAL_NO_MEM             -9
#define OPAL_RESOURCE           -10
#define OPAL_INTERNAL_ERROR     -11
#define OPAL_BUSY_EVENT         -12
#define OPAL_HARDWARE_FROZEN    -13

#define OPAL_CONSOLE_WRITE       1
#define OPAL_CONSOLE_READ        2
#define OPAL_TERMINAL_0          0

#define OPAL_POLL_EVENTS         10
#define OPAL_EVENT_CONSOLE_INPUT 0x10

#define OPAL_REINIT_CPUS         70
#define OPAL_REINIT_CPUS_HILE_LE (1 << 1)

OPAL_STATUS
OpalConsoleWrite (
	IN UINTN Terminal,
	IN EFI_PHYSICAL_ADDRESS LengthPointer,
	IN EFI_PHYSICAL_ADDRESS Buf
	);

OPAL_STATUS
OpalConsoleRead (
	IN UINTN Terminal,
	IN EFI_PHYSICAL_ADDRESS OutLengthPointer,
	IN EFI_PHYSICAL_ADDRESS OutBuf
	);

OPAL_STATUS
OpalReinitCPUs (
	IN UINT64 Flags
	);

OPAL_STATUS
OpalPollEvents (
	IN EFI_PHYSICAL_ADDRESS OutEventMask
	);

#endif // __OPAL_LIB_H__
