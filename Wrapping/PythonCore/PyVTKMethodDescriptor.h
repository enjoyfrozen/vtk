/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/

#ifndef PyVTKMethodDescriptor_h
#define PyVTKMethodDescriptor_h

#include "vtkWrappingPythonCoreModule.h" // For export macro
#include "vtkPython.h"
#include "vtkSystemIncludes.h"

extern VTKWRAPPINGPYTHONCORE_EXPORT PyTypeObject PyVTKMethodDescriptor_Type;

#define PyVTKMethodDescriptor_Check(obj) \
  (Py_TYPE(obj) == &PyVTKMethodDescriptor_Type)

extern "C"
{
// Create a new method descriptor from a PyMethodDef.
VTKWRAPPINGPYTHONCORE_EXPORT
PyObject *PyVTKMethodDescriptor_New(
  PyTypeObject *cls, PyMethodDef *meth);
}

#endif
