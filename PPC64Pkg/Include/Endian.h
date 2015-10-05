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

#ifndef __ENDIAN_H__
#define __ENDIAN_H__

//
// Suboptimal.
//

STATIC inline UINT16
Swab16(UINT16 x)
{
	return  ((x & (UINT16) 0x00ffU) << 8) |
		((x & (UINT16) 0xff00U) >> 8);
}

STATIC inline UINT32
Swab32(UINT32 x)
{
	return  ((x & (UINT32) 0x000000ffUL) << 24) |
		((x & (UINT32) 0x0000ff00UL) <<  8) |
		((x & (UINT32) 0x00ff0000UL) >>  8) |
		((x & (UINT32) 0xff000000UL) >> 24);
}

STATIC inline UINT64
Swab64(UINT64 x)
{
	return  (UINT64)((x & (UINT64) 0x00000000000000ffULL) << 56) |
		(UINT64)((x & (UINT64) 0x000000000000ff00ULL) << 40) |
		(UINT64)((x & (UINT64) 0x0000000000ff0000ULL) << 24) |
		(UINT64)((x & (UINT64) 0x00000000ff000000ULL) <<  8) |
		(UINT64)((x & (UINT64) 0x000000ff00000000ULL) >>  8) |
		(UINT64)((x & (UINT64) 0x0000ff0000000000ULL) >> 24) |
		(UINT64)((x & (UINT64) 0x00ff000000000000ULL) >> 40) |
		(UINT64)((x & (UINT64) 0xff00000000000000ULL) >> 56);
}

#endif /* __ENDIAN_H__ */
