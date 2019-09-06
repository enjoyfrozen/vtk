/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#ifndef vtkOpenGL_h
#define vtkOpenGL_h

#include "vtkConfigure.h"
#include "vtkRenderingOpenGLConfigure.h" // For VTK_USE_COCOA

// To prevent gl.h to include glext.h provided by the system
#define GL_GLEXT_LEGACY
#if defined(__APPLE__) && defined(VTK_USE_COCOA)
# include <OpenGL/gl.h> // Include OpenGL API.
#else
# include "vtkWindows.h" // Needed to include OpenGL header on Windows.
# include <GL/gl.h> // Include OpenGL API.
#endif

#endif
// VTK-HeaderTest-Exclude: vtkOpenGL.h
