/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef PyVTKNamespace_h
#define PyVTKNamespace_h

#include "vtkWrappingPythonCoreModule.h" // For export macro
#include "vtkPython.h"
#include "vtkSystemIncludes.h"

extern VTKWRAPPINGPYTHONCORE_EXPORT PyTypeObject PyVTKNamespace_Type;

#define PyVTKNamespace_Check(obj) \
  (Py_TYPE(obj) == &PyVTKNamespace_Type)

extern "C"
{
VTKWRAPPINGPYTHONCORE_EXPORT
PyObject *PyVTKNamespace_New(const char *name);

VTKWRAPPINGPYTHONCORE_EXPORT
PyObject *PyVTKNamespace_GetDict(PyObject *self);

VTKWRAPPINGPYTHONCORE_EXPORT
const char *PyVTKNamespace_GetName(PyObject *self);
}

#endif
