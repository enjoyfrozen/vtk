/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkArrayIteratorIncludes
 * @brief   centralize array iterator type includes
 * required when using the vtkArrayIteratorTemplateMacro.
 *
 *
 * A CXX file using vtkArrayIteratorTemplateMacro needs to include the
 * header files for all types of iterators supported by the macro.  As
 * new arrays and new iterators are added, vtkArrayIteratorTemplateMacro
 * will also need to be updated to switch to the additional cases.
 * However, this would imply any code using the macro will start giving
 * compilation errors unless they include the new iterator headers. The
 * vtkArrayIteratorIncludes.h will streamline this issue. Every file
 * using the vtkArrayIteratorTemplateMacro must include this
 * vtkArrayIteratorIncludes.h. As new iterators are added and the
 * vtkArrayIteratorTemplateMacro updated, one needs to update this header
 * file alone.
*/

#ifndef vtkArrayIteratorIncludes_h
#define vtkArrayIteratorIncludes_h

// Iterators.
#include "vtkArrayIteratorTemplate.h"
#include "vtkBitArrayIterator.h"

#endif

// VTK-HeaderTest-Exclude: vtkArrayIteratorIncludes.h
