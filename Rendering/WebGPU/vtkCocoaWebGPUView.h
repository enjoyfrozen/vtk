/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkCocoaWebGPUView.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkCocoaWebGPUView
 * @brief   Cocoa WebGPU rendering context
 *
 *
 * This class is a subclass of Cocoa's NSView; it uses Objective-C++.
 * This class overrides several NSView methods.
 * To provide the usual VTK keyboard user interface, it overrides the
 * following methods: acceptsFirstResponder, keyDown:,
 * keyUp:, and flagsChanged:
 * To provide the usual VTK mouse user interface, it overrides the
 * following methods: mouseMoved:, mouseEntered:,
 * mouseExited: scrollWheel:, mouseDown:, rightMouseDown:,
 * otherMouseDown:, mouseDragged:, rightMouseDragged:, otherMouseDragged:,
 * and updateTrackingAreas.
 * To provide file dropping support, it implements the following methods:
 * draggingEntered: and performDragOperation:.
 * To be able to render and draw onscreen, it overrides drawRect:.
 *
 * @sa
 * vtkCocoaWebGPURenderWindow vtkCocoaRenderWindowInteractor
 */

#ifndef vtkCocoaWebGPUView_h
#define vtkCocoaWebGPUView_h

#import "vtkRenderingWebGPUModule.h" // For export macro
#import <Cocoa/Cocoa.h>

#import "vtk_wgpu.h"

// Note: This file should be includable by both pure Objective-C and Objective-C++ source files.
// To achieve this, we use the neat technique below:
#ifdef __cplusplus
// Forward declarations
VTK_ABI_NAMESPACE_BEGIN
class vtkCocoaWebGPURenderWindow;
class vtkCocoaRenderWindowInteractor;

// Type declarations
typedef vtkCocoaWebGPURenderWindow* vtkCocoaRenderWindowRef;
typedef vtkCocoaRenderWindowInteractor* vtkCocoaRenderWindowInteractorRef;

typedef wgpu::SurfaceDescriptor* wgpuSurfaceDescriptorRef;
VTK_ABI_NAMESPACE_END
#else
// Type declarations
typedef void* vtkCocoaRenderWindowRef;
typedef void* vtkCocoaRenderWindowInteractorRef;
#endif

VTKRENDERINGWEBGPU_EXPORT
@interface vtkCocoaWebGPUView : NSView<NSDraggingDestination>
{
@private
  vtkCocoaRenderWindowRef _myVTKRenderWindow;
  NSTrackingArea* _rolloverTrackingArea;
  wgpu::SurfaceDescriptorFromMetalLayer* _mySurfaceDescriptor;
}

- (vtkCocoaRenderWindowRef)getVTKRenderWindow;
- (void)setVTKRenderWindow:(vtkCocoaRenderWindowRef)theVTKRenderWindow;

- (vtkCocoaRenderWindowInteractorRef)getInteractor;

  (wgpuSurfaceDescriptorRef)getSurfaceDescriptor; 

@end

#endif /* vtkCocoaWebGPUView_h */
// VTK-HeaderTest-Exclude: vtkCocoaWebGPUView.h
