/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef PyVTKEnum_h
#define PyVTKEnum_h

#include "vtkWrappingPythonCoreModule.h" // For export macro
#include "vtkPython.h"
#include "vtkSystemIncludes.h"

extern "C"
{
VTKWRAPPINGPYTHONCORE_EXPORT
PyTypeObject *PyVTKEnum_Add(PyTypeObject *pytype);

VTKWRAPPINGPYTHONCORE_EXPORT
PyObject *PyVTKEnum_New(PyTypeObject *pytype, int val);
}

#endif
