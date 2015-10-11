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

.text

#/**
#  Saves the current CPU context that can be restored with a call to LongJump() and returns 0.#
#
#  Saves the current CPU context in the buffer specified by JumpBuffer and returns 0.  The initial
#  call to SetJump() must always return 0.  Subsequent calls to LongJump() cause a non-zero
#  value to be returned by SetJump().
#
#  If JumpBuffer is NULL, then ASSERT().
#  For IPF CPUs, if JumpBuffer is not aligned on a 16-byte boundary, then ASSERT().
#
#  @param  JumpBuffer    A pointer to CPU context buffer.
#
#**/
#
#UINTN
#EFIAPI
#SetJump (
#  IN      BASE_LIBRARY_JUMP_BUFFER  *JumpBuffer  // r3
#  );
#
ASM_FUNC(SetJump)
        std     %r1,  0(%r3)
        stdu    %r2,  8(%r3)
        stdu    %r14, 8(%r3)
        stdu    %r15, 8(%r3)
        stdu    %r16, 8(%r3)
        stdu    %r17, 8(%r3)
        stdu    %r18, 8(%r3)
        stdu    %r19, 8(%r3)
        stdu    %r20, 8(%r3)
        stdu    %r21, 8(%r3)
        stdu    %r22, 8(%r3)
        stdu    %r23, 8(%r3)
        stdu    %r24, 8(%r3)
        stdu    %r25, 8(%r3)
        stdu    %r26, 8(%r3)
        stdu    %r27, 8(%r3)
        stdu    %r28, 8(%r3)
        stdu    %r29, 8(%r3)
        stdu    %r30, 8(%r3)
        stdu    %r31, 8(%r3)
        mflr    %r0
        stdu    %r0,  8(%r3)
        mfcr    %r0
        stdu    %r0,  8(%r3)
        li      %r3,  0
        blr

#/**
#  Restores the CPU context that was saved with SetJump().#
#
#  Restores the CPU context from the buffer specified by JumpBuffer.
#  This function never returns to the caller.
#  Instead is resumes execution based on the state of JumpBuffer.
#
#  @param  JumpBuffer    A pointer to CPU context buffer.
#  @param  Value         The value to return when the SetJump() context is restored.
#
#**/
#VOID
#EFIAPI
#InternalLongJump (
#  IN      BASE_LIBRARY_JUMP_BUFFER  *JumpBuffer,  // r3
#  IN      UINTN                     Value         // r4
#  );
#
ASM_FUNC(InternalLongJump)
        ld      %r1,  0(%r3)
        ldu     %r2,  8(%r3)
        ldu     %r14, 8(%r3)
        ldu     %r15, 8(%r3)
        ldu     %r16, 8(%r3)
        ldu     %r17, 8(%r3)
        ldu     %r18, 8(%r3)
        ldu     %r19, 8(%r3)
        ldu     %r20, 8(%r3)
        ldu     %r21, 8(%r3)
        ldu     %r22, 8(%r3)
        ldu     %r23, 8(%r3)
        ldu     %r24, 8(%r3)
        ldu     %r25, 8(%r3)
        ldu     %r26, 8(%r3)
        ldu     %r27, 8(%r3)
        ldu     %r28, 8(%r3)
        ldu     %r29, 8(%r3)
        ldu     %r30, 8(%r3)
        ldu     %r31, 8(%r3)
        ldu     %r0,  8(%r3)
        mtlr    %r0
        ldu     %r0,  8(%r3)
        mtcr    %r0
        mr      %r3,  %r4
        blr
