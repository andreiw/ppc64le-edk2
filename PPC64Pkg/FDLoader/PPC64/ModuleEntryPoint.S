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

#include <AsmMacroLib.h>

ASM_FUNC(_ModuleEntryPoint)
        /*
         * If we're not in LE, let's switch.
         */
0:      tdi   0, 0, 0x48; /* Reverse endian of b . + 8          */
        b     $+36;       /* Skip trampoline if endian is good  */
        .long 0x05009f42; /* bcl 20,31,$+4                      */
        .long 0xa602487d; /* mflr r10                           */
        .long 0x1c004a39; /* addi r10,r10,28                    */
        .long 0xa600607d; /* mfmsr r11                          */
        .long 0x01006b69; /* xori r11,r11,1                     */
        .long 0xa6035a7d; /* mtsrr0 r10                         */
        .long 0xa6037b7d; /* mtsrr1 r11                         */
        .long 0x2400004c  /* rfid                               */

        /*
         * This fussy nonsense jumps through an entry point
         * stored at 0b + 0x4000.
         *
         * For ABIv1 the entry point is a function descriptor,
         * hence the other bit of indirection.
         */
1:      bl   2f
2:      mflr r1
        addi r1, r1, (16 * 1024) - (2b - 0b)
        ld   r1, 0(r1)
        ld   r1, 0(r1)    /* Remove when we switch to ABIv2 */
        mtlr r1
        blr
