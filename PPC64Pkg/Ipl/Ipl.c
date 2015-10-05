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
#include <Library/PrintLib.h>
#include <Endian.h>
#include <Pcr.h>

PCR PrimaryPCR;

VOID
CEntryPoint (UINT64 FDTBase)
{
	UINTN CharCount;
	CHAR8 Buffer[100];

	SerialPortInitialize();
	CharCount = AsciiSPrint (Buffer, sizeof(Buffer), "PPC64LE UEFI firmware (version %s built at %a on %a)\n\r",
				 (CHAR16*) PcdGetPtr(PcdFirmwareVersionString), __TIME__, __DATE__);
	SerialPortWrite ((UINT8 *) Buffer, CharCount);

	while(1) {
		UINT8 buf[5];
		UINTN len = 0;
		if (SerialPortPoll()) {
			len = SerialPortRead(buf, 5);
		}

		if (len != 0) {
			SerialPortWrite(buf, len);
		}
	}
}

