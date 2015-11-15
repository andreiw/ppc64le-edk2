#/** @file
# PowerPC 64-bit LE on PowerNV/OPAL platforms.
#
# Copyright (c) 2015, Andrei Warkentin <andrey.warkentin@gmail.com>
#
#    This program and the accompanying materials
#    are licensed and made available under the terms and conditions of the BSD License
#    which accompanies this distribution. The full text of the license may be found at
#    http://opensource.org/licenses/bsd-license.php
#
#    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#**/

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = PPC64Platform
  PLATFORM_GUID                  = 5CFBD99E-3C44-4E7F-8054-9CDEAFF7711E
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/$(PLATFORM_NAME)
  SUPPORTED_ARCHITECTURES        = PPC64
  BUILD_TARGETS                  = DEBUG|RELEASE
  # FLASH_DEFINITION              = PPC64PlatformPkg/PPC64PlatformPkg.fdf
  SKUID_IDENTIFIER               = DEFAULT
  #
  # You can also build as elfv1, although the artifacts
  # produced are not interchangeable (and won't mix
  # on purpose due to different COFF architecture value).
  #
  ABI                            = elfv2

[BuildOptions]
  RELEASE_*_*_CC_FLAGS  = -DMDEPKG_NDEBUG
  *_*_PPC64_ABI_FLAGS   = $(ABI)

[LibraryClasses.common]
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf

[Components.common]
  PPC64PlatformPkg/Test/Test.inf
