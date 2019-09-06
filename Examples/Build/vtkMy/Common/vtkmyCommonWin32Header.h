/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
/**
 * @class   vtkmyCommonWin32Header
 * @brief   manage Windows system differences
 *
 * The vtkmyCommonWin32Header captures some system differences between Unix
 * and Windows operating systems.
*/

#ifndef vtkmyCommonWin32Header_h
#define vtkmyCommonWin32Header_h

#include <vtkmyConfigure.h>

#if defined(_WIN32) && !defined(VTKMY_STATIC)
#if defined(vtkmyCommon_EXPORTS)
#define VTK_MY_COMMON_EXPORT __declspec( dllexport )
#else
#define VTK_MY_COMMON_EXPORT __declspec( dllimport )
#endif
#else
#define VTK_MY_COMMON_EXPORT
#endif

#endif
