/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkBridgeExport
 * @brief   manage Windows system differences
 *
 * The vtkBridgeExport captures some system differences between Unix and
 * Windows operating systems.
*/

#ifndef vtkBridgeExport_h
#define vtkBridgeExport_h
#include "vtkTestingGenericBridgeModule.h"
#include "vtkSystemIncludes.h"

#if 1
# define VTK_BRIDGE_EXPORT
#else

#if defined(_WIN32) && defined(VTK_BUILD_SHARED_LIBS)

 #if defined(vtkBridge_EXPORTS)
  #define VTK_BRIDGE_EXPORT __declspec( dllexport )
 #else
  #define VTK_BRIDGE_EXPORT __declspec( dllimport )
 #endif
#else
 #define VTK_BRIDGE_EXPORT
#endif

#endif //#if 1

#endif

// VTK-HeaderTest-Exclude: vtkBridgeExport.h
