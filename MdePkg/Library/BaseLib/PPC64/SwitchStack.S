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

// 48 + 64 parameter save area
#define STACK_FRAME_MIN 112

.text

/**
//
//  This allows the caller to switch the stack and goes to the new entry point
//
// @param      EntryPoint   The pointer to the location to enter
// @param      Context      Parameter to pass in
// @param      Context2     Parameter2 to pass in
// @param      NewStack     New Location of the stack
//
// @return     Nothing. Goes to the Entry Point passing in the new parameters
//
VOID
EFIAPI
InternalSwitchStack (
  SWITCH_STACK_ENTRY_POINT EntryPoint,
  VOID  *Context,
  VOID  *Context2,
  VOID  *NewStack
  );
**/
ASM_FUNC(InternalSwitchStack)
    mtlr  %r3
    subi  %r1, %r6, STACK_FRAME_MIN
    mr    %r3, %r4
    mr    %r4, %r5    
    blr

