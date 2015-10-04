//
// Copyright (c) 2015, Andrei Warkentin <andrey.warkentin@gmail.com>
//
//  This program and the accompanying materials
//  are licensed and made available under the terms and conditions of the BSD License
//  which accompanies this distribution.  The full text of the license may be found at
//  http://opensource.org/licenses/bsd-license.php
//
//  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
//  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
//
//

#include <AutoGen.h>

.text
.align 3

#define r1 1
#define r2 2
#define r3 3
#define r4 4
#define r10 10
#define r5 5
#define r12 12

        .global _ModuleEntryPoint
_ModuleEntryPoint:
        .quad ._ModuleEntryPoint,.TOC.@tocbase,0
        .previous
        .global ._ModuleEntryPoint
 ._ModuleEntryPoint:

        // load TOC
        bl 1f
1:      mflr r10
        subi r10, r10, 1b-_ModuleEntryPoint
        ld r2, 8(r10)

        // save OPAL base
        ld r10, opal_base@toc(r2)
        std r4, 0(r10)

        // go C
        bl CEntryPoint
        b .

