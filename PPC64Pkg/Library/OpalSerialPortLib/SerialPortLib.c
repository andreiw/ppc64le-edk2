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

#include <Uefi.h>
#include <Endian.h>
#include <Library/OpalLib.h>

/*

  Programmed hardware of Serial port.

  @return    Always return EFI_UNSUPPORTED.

**/
RETURN_STATUS
EFIAPI
SerialPortInitialize (
  VOID
  )
{
	return RETURN_SUCCESS;
}

/**
  Write data to serial device.

  @param  Buffer           Point of data buffer which need to be writed.
  @param  NumberOfBytes    Number of output bytes which are cached in Buffer.

  @retval 0                Write data failed.
  @retval !0               Actual number of bytes writed to serial device.

**/

#define PRINT_BUFFER_SIZE       512
#define PRINT_BUFFER_THRESHOLD  (PRINT_BUFFER_SIZE - 4)

UINTN
EFIAPI
SerialPortWrite (
	IN UINT8     *Buffer,
	IN UINTN     NumberOfBytes
)
{
	UINT8 PrintBuffer[PRINT_BUFFER_SIZE];
	UINTN SourceIndex      = 0;
	UINTN DestinationIndex = 0;
	UINTN OpalLen;

	while (SourceIndex < NumberOfBytes)
	{
		PrintBuffer[DestinationIndex++] = Buffer[SourceIndex++];
		if (DestinationIndex > PRINT_BUFFER_THRESHOLD)
		{
			PrintBuffer[DestinationIndex] = '\0';
			OpalLen = Swab64(DestinationIndex);
			OpalConsoleWrite(OPAL_TERMINAL_0,
					 (EFI_PHYSICAL_ADDRESS) &OpalLen,
					 (EFI_PHYSICAL_ADDRESS) PrintBuffer);

			DestinationIndex = 0;
		}
	}

	if (DestinationIndex > 0)
	{
			OpalLen = Swab64(DestinationIndex);
			OpalConsoleWrite(OPAL_TERMINAL_0,
					 (EFI_PHYSICAL_ADDRESS) &OpalLen,
					 (EFI_PHYSICAL_ADDRESS) PrintBuffer);
	}

	return NumberOfBytes;
}


/**
  Read data from serial device and save the datas in buffer.

  @param  Buffer           Point of data buffer which need to be writed.
  @param  NumberOfBytes    Number of output bytes which are cached in Buffer.

  @retval 0                Read data failed.
  @retval !0               Aactual number of bytes read from serial device.

**/
UINTN
EFIAPI
SerialPortRead (
  OUT UINT8     *Buffer,
  IN  UINTN     NumberOfBytes
)
{
	OPAL_STATUS Status;
	UINTN OpalLen = Swab64(NumberOfBytes);

	Status = OpalConsoleRead(OPAL_TERMINAL_0,
				 (EFI_PHYSICAL_ADDRESS) &OpalLen,
				 (EFI_PHYSICAL_ADDRESS) Buffer);
	if (Status != OPAL_SUCCESS) {
		return 0;
	}

	return Swab64(OpalLen);
}



/**
  Check to see if any data is avaiable to be read from the debug device.

  @retval TRUE       At least one byte of data is avaiable to be read
  @retval FALSE      No data is avaiable to be read

**/
BOOLEAN
EFIAPI
SerialPortPoll (
  VOID
  )
{
	UINTN EventMask;
	OPAL_STATUS Status;

	Status = OpalPollEvents((EFI_PHYSICAL_ADDRESS) &EventMask);
	if (Status != OPAL_SUCCESS) {
		return FALSE;
	}

	EventMask = Swab64(EventMask);
	if ((EventMask & OPAL_EVENT_CONSOLE_INPUT) != 0) {
		return TRUE;
	}

	return FALSE;
}

