/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
#ifndef QVTKWin32Header_h
#define QVTKWin32Header_h

#include "vtkSystemIncludes.h"
#include "vtkABI.h"

#if defined(VTK_BUILD_SHARED_LIBS)
# if defined(QVTK_EXPORTS) || defined(QVTKWidgetPlugin_EXPORTS)
#  define QVTK_EXPORT VTK_ABI_EXPORT
# else
#  define QVTK_EXPORT VTK_ABI_IMPORT
# endif
#else
# define QVTK_EXPORT
#endif

#endif /*QVTKWin32Header_h*/
