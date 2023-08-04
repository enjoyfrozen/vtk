// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#ifndef vtkRealtimeFrameProfiler_h
#define vtkRealtimeFrameProfiler_h

/**
 * @brief   a collection of macros that enable real-time frame profiling
 *
 * This header file enables real time frame profiling with Tracy client.
 * See vtkProfiler.h for how to use the profiler in code.
 */

#include "vtkProfiler.h"

#if VTK_ENABLE_PROFILER
// Mark a frame. Typically used before buffers are swapped.
#define vtkProfileFrame FrameMark
// Mark a frame with name.
#define vtkProfileFrameN(x) FrameMarkNamed(x)
// Start a frame.
#define vtkProfileFrameStart(x) FrameMarkStart(x)
// End a frame.
#define vtkProfileFrameEnd(x) FrameMarkEnd(x)
// Send an image to the profiler. Helps visually distinguish frames.
#define vtkProfileFrameImage(image, width, height, offset, flip)                                   \
  FrameImage(image, width, height, offset, flip)
#else
// Mark a frame. Typically used before buffers are swapped.
#define vtkProfileFrame
// Mark a frame with name.
#define vtkProfileFrameN(x)
// Start a frame.
#define vtkProfileFrameStart(x)
// End a frame.
#define vtkProfileFrameEnd(x)
// Send an image to the profiler. Helps visually distinguish frames.
#define vtkProfileFrameImage(image, width, height, offset, flip)
#endif // VTK_ENABLE_PROFILER
#endif // vtkRealtimeFrameProfiler_h
// VTK-HeaderTest-Exclude: vtkRealtimeFrameProfiler.h
