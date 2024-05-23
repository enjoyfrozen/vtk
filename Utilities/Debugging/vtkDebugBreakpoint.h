// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#ifndef vtkDebugBreakpoint_h
#define vtkDebugBreakpoint_h

#if defined(__has_builtin) && !defined(__ibmxl__)
#if __has_builtin(__builtin_debugtrap)
#define VTK_DEBUG_BREAKPOINT() __builtin_debugtrap()
#elif __has_builtin(__debugbreak)
#define VTK_DEBUG_BREAKPOINT() __debugbreak()
#endif
#endif

#if !defined(VTK_DEBUG_BREAKPOINT)
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#define VTK_DEBUG_BREAKPOINT() __debugbreak()
#elif defined(__ARMCC_VERSION)
#define VTK_DEBUG_BREAKPOINT() __breakpoint(42)
#elif defined(__ibmxl__) || defined(__xlC__)
#include <builtins.h>
#define VTK_DEBUG_BREAKPOINT() __trap(42)
#elif defined(__i386__) || defined(__x86_64__)
static inline void VTK_DEBUG_BREAKPOINT()
{
  __asm__ __volatile__("int3");
}
#elif defined(__aarch64__)
static inline void VTK_DEBUG_BREAKPOINT()
{
  __asm__ __volatile__(".inst 0xd4200000");
}
#elif defined(__arm__)
static inline void VTK_DEBUG_BREAKPOINT()
{
  __asm__ __volatile__(".inst 0xe7f001f0");
}
#elif defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0) && defined(__GNUC__)
#define VTK_DEBUG_BREAKPOINT() __builtin_trap()
#else
#include <signal.h>
#if defined(SIGTRAP)
#define VTK_DEBUG_BREAKPOINT() raise(SIGTRAP)
#else
#define VTK_DEBUG_BREAKPOINT() raise(SIGABRT)
#endif
#endif
#endif

#if !defined(NDEBUG)
#if defined(__GNUC__) && (__GNUC__ >= 3)
#define VTK_LIKELY(expr) __builtin_expect(!!(expr), 1)
#else
#define VTK_LIKELY(expr) (!!(expr))
#endif
#define VTK_DEBUG_ASSERT(expr)                                                                     \
  do                                                                                               \
  {                                                                                                \
    if (!VTK_LIKELY(expr))                                                                         \
    {                                                                                              \
      VTK_DEBUG_BREAKPOINT();                                                                      \
    }                                                                                              \
  } while (0)
#else
#define VTK_DEBUG_ASSERT(expr)
#endif
#endif // vtkDebugBreakpoint_h
