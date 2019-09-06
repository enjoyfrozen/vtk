/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef PyVTKTemplate_h
#define PyVTKTemplate_h

#include "vtkWrappingPythonCoreModule.h" // For export macro
#include "vtkPython.h"
#include "vtkSystemIncludes.h"

extern VTKWRAPPINGPYTHONCORE_EXPORT PyTypeObject PyVTKTemplate_Type;

#define PyVTKTemplate_Check(obj) \
  (Py_TYPE(obj) == &PyVTKTemplate_Type)

extern "C"
{
VTKWRAPPINGPYTHONCORE_EXPORT
PyObject *PyVTKTemplate_New(const char *name, const char *docstring);

VTKWRAPPINGPYTHONCORE_EXPORT
int PyVTKTemplate_AddItem(PyObject *self, PyObject *val);
}

#endif
