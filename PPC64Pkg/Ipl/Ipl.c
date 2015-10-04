/** @file
*
* Copyright (c) 2015, Andrei Warkentin <andrey.warkentin@gmail.com>
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

void *opal_base;

char *hi = "Hello from UEFI IPL on PPC64LE!\n";

VOID
CEntryPoint (unsigned long puts)
{
  asm volatile("mtctr %0\n\t"
               "mr 12, %0\n\t"
               "mr 3, %1\n\t"
               "bctrl\n\t" : : "r" (puts), "r" (hi) : "memory", "ctr", "lr", "r12", "r3");
}

