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
#include <Library/PcdLib.h>

#define FV_OFFSET (FixedPcdGet64(PcdFvBaseAddress) - \
                   FixedPcdGet64(PcdFdBaseAddress))

	ABI_NOTE

ASM_FUNC(_ModuleEntryPoint)
        /*
         * With credit to the Teensy ELF tutorial.
         *
         * Who needs linkers anyway?
         */
start:  .byte 0x7F, 'E', 'L', 'F', 2, 1, 1, 0  /* e_ident */
        .byte 0, 0, 0, 0, 0, 0, 0, 0
        .word 2                                /* e_type */
        .word 21                               /* e_machine */
        .long 1                                /* e_version */
        .quad FixedPcdGet64(PcdFdBaseAddress) + code - start /* e_entry */
        .quad phdr - start                     /* e_phoff */
        .quad 0                                /* e_shoff */
        .long 0                                /* e_flags */
        .word phdr - start                     /* e_ehsize */
        .word code - phdr                      /* e_phentsize */
        .word 1                                /* e_phnum */
        .word 0                                /* e_shentsize */
        .word 0                                /* e_shnum */
        .word 0                                /* e_shstrndx */
phdr:   .long 1                                /* e_ptype */
        .long 5                                /* p_flags */
        .quad 0                                /* p_offset */
        .quad FixedPcdGet64(PcdFdBaseAddress)  /* p_vaddr */
        .quad FixedPcdGet64(PcdFdBaseAddress)  /* p_paddr */
        .quad FixedPcdGet32(PcdFdSize)         /* p_filesz */
        .quad FixedPcdGet32(PcdFdSize)         /* p_memsz */
        .quad 0x1000                           /* P_align */

        /*
         * If we're not in LE, let's switch.
         */
code:   tdi   0, 0, 0x48; /* Reverse endian of b . + 8          */
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
        addi r1, r1, (FV_OFFSET) - (2b - start)
        ld   r1, 0(r1)
#ifdef PPC64_ABI_elfv1
        ld   r1, 0(r1)
#endif /* PPC64_ABI_elfv1 */
        mtlr r1
        blr
