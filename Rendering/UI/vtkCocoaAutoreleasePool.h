// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#import "vtkCocoaMacOSXSDKCompatibility.h" // Needed to support old SDKs
#import <Cocoa/Cocoa.h>

#if VTK_OBJC_IS_ARC
#error vtkCocoaAutoreleasePool must not be compiled with ARC
#endif

// RAII class to create an NSAutoreleasePool.
class vtkCocoaAutoreleasePool
{
public:
  vtkCocoaAutoreleasePool() { Pool = [[NSAutoreleasePool alloc] init]; }

  ~vtkCocoaAutoreleasePool()
  {
    // Drain the pool on destruction
    [Pool release];
    Pool = nil;
  }

  // Deleted copy constructor.
  vtkCocoaAutoreleasePool(const vtkCocoaAutoreleasePool& other) = delete;

  // Deleted assignment operator.
  vtkCocoaAutoreleasePool& operator=(const vtkCocoaAutoreleasePool& other) = delete;

private:
  NSAutoreleasePool* Pool;
};
