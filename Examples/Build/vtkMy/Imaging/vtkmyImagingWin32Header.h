/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
/**
 * @class   vtkmyImagingWin32Header
 * @brief   manage Windows system differences
 *
 * The vtkmyImagingWin32Header captures some system differences between Unix
 * and Windows operating systems.
*/

#ifndef vtkmyImagingWin32Header_h
#define vtkmyImagingWin32Header_h

#include <vtkmyConfigure.h>

#if defined(_WIN32) && !defined(VTKMY_STATIC)
#if defined(vtkmyImaging_EXPORTS)
#define VTK_MY_IMAGING_EXPORT __declspec( dllexport )
#else
#define VTK_MY_IMAGING_EXPORT __declspec( dllimport )
#endif
#else
#define VTK_MY_IMAGING_EXPORT
#endif

#endif
