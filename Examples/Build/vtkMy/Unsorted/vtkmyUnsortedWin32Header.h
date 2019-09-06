/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
/**
 * @class   vtkmyUnsortedWin32Header
 * @brief   manage Windows system differences
 *
 * The vtkmyUnsortedWin32Header captures some system differences between Unix
 * and Windows operating systems.
*/

#ifndef vtkmyUnsortedWin32Header_h
#define vtkmyUnsortedWin32Header_h

#include <vtkmyConfigure.h>

#if defined(_WIN32) && !defined(VTKMY_STATIC)
#if defined(vtkmyUnsorted_EXPORTS)
#define VTK_MY_UNSORTED_EXPORT __declspec( dllexport )
#else
#define VTK_MY_UNSORTED_EXPORT __declspec( dllimport )
#endif
#else
#define VTK_MY_UNSORTED_EXPORT
#endif

#endif
