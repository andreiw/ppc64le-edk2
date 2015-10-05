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
	UINT64 Toc;
	UINT64 OPALBase;
	UINT64 OPALEntry;
	UINT64 SLBSize;
	UINT64 UnrecSP;
	UINT64 KernSP;
	UINT64 TBFreq;
} PCR;

STATIC inline PCR *
PcrGet(void)
{
	register PCR *pcr asm("r13");
	return pcr;
}
#else
#define PCR_Toc        0
#define PCR_OPALBase   8
#define PCR_OPALEntry 16
#define PCR_SLBSize   24
#define PCR_UnrecSP   32
#define PCR_KernSP    40
#define PCR_TBFreq    48
#define PCR_R(name, reg) (PCR_ ## name)(reg)
#define PCR(name) PCR_R(name, r13)

#endif // __ASSEMBLY__
#endif // __PCR_H__

