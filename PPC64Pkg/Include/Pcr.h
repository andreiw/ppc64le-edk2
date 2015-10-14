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

#ifndef __PCR_H__
#define __PCR_H__

#ifndef __ASSEMBLY__
typedef struct _PCR {
	UINT64 OPALBase;
	UINT64 OPALEntry;
	UINT64 CpuDxeUnrecSP;
	UINT64 CpuDxeTOC;
	UINT64 TBFreq;
	UINT64 PhysVectorsBase;
	UINT64 PhysVectorsSize;

} PCR;

STATIC inline PCR *
PcrGet(void)
{
	register PCR *pcr asm("r13");
	return pcr;
}
#else
#define PCR_OPALBase                 0
#define PCR_OPALEntry                8
#define PCR_CpuDxeUnrecSP           16
#define PCR_CpuDxeTOC               24
#define PCR_TBFreq                  32
#define PCR_PhysVectorsBase         40
#define PCR_PhysVectorsSize         48
#define PCR_Size                0x1000
#define PCR_R(name, reg) (PCR_ ## name)(reg)
#define PCR(name) PCR_R(name, r13)

#endif // __ASSEMBLY__
#endif // __PCR_H__

