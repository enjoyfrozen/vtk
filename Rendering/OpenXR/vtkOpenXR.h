#ifndef vtkOpenXR_h
#define vtkOpenXR_h

// Needed for WIN32 and VTK_USE_X
#include "vtkRenderingOpenGLConfigure.h"
#include "vtk_glew.h"

#ifdef _WIN32
#define XR_USE_PLATFORM_WIN32
#include "GL/gl.h"
#include "Unknwn.h"
#include "vtkWindows.h"
#endif

#ifdef VTK_USE_X
#define XR_USE_PLATFORM_XLIB
// Required headers for the XrGraphicsBindingOpenGLXlibKHR struct
#include <GL/glx.h>
#include <X11/Xlib.h>
#endif

// Needed for XR_KHR_OPENGL_ENABLE_EXTENSION_NAME
#define XR_USE_GRAPHICS_API_OPENGL

#ifdef OpenXR_USE_REMOTING
// Needed for XR_KHR_D3D11_ENABLE_EXTENSION_NAME
#define XR_USE_GRAPHICS_API_D3D11
// Required headers for the XrGraphicsRequirementsD3D11KHR struct
#include "d3d11.h"
#endif

#include <openxr.h>
#include <openxr_platform.h>
#include <openxr_reflection.h>
#ifdef OpenXR_USE_REMOTING
#include <openxr_msft_holographic_remoting.h>
#endif

#include "XrExtensions.h"

#define HAND_COUNT 2

#define LEFT_EYE 0
#define RIGHT_EYE 1

#endif
// VTK-HeaderTest-Exclude: vtkOpenXR.h
