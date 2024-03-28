// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#ifndef vtkOpenGLRealtimeFrameProfiler_h
#define vtkOpenGLRealtimeFrameProfiler_h
/**
 * @brief   a collection of macros that enable real-time OpenGL GPU profiling.
 *
 * Refer to documentation in vtkProfiler.h
 */

#include "vtkRealtimeFrameProfiler.h"

#if VTK_ENABLE_PROFILER
#include "tracy/TracyOpenGL.hpp"
// Inform tracy about your OpenGL GPU context after initialization.
#define vtkProfileOpenGLGPUContextReady TracyGpuContext
// Provide a custom name for the context.
#define vtkProfileOpenGLGPUContextReadyN(x) TracyGpuContextName(x, strlen(x))
// Call this after buffers are swapped to collect all GPU events.
#define vtkProfileOpenGLGPUCollect TracyGpuCollect
// Call this to profile GPU zones. Ex: glDrawArrays, etc.
#define vtkProfileOpenGLGPUZone(x) TracyGpuZone(x)
#else
// Inform tracy about your OpenGL GPU context after initialization.
#define vtkProfileOpenGLGPUContextReady
// Provide a custom name for the context.
#define vtkProfileOpenGLGPUContextReadyN(x)
// Call this after buffers are swapped to collect all GPU events.
#define vtkProfileOpenGLGPUCollect
// Call this to profile GPU zones. Ex: glDrawArrays, etc.
#define vtkProfileOpenGLGPUZone(x)
#endif

#endif // vtkOpenGLRealtimeFrameProfiler_h
// VTK-HeaderTest-Exclude: vtkOpenGLRealtimeFrameProfiler.h
