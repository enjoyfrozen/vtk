/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef PyVTKReference_h
#define PyVTKReference_h

#include "vtkWrappingPythonCoreModule.h" // For export macro
#include "vtkPython.h"
#include "vtkSystemIncludes.h"

// The PyVTKReference is a wrapper around a PyObject of
// type int or float.
struct PyVTKReference {
  PyObject_HEAD
  PyObject *value;
};

extern VTKWRAPPINGPYTHONCORE_EXPORT PyTypeObject PyVTKReference_Type;
extern VTKWRAPPINGPYTHONCORE_EXPORT PyTypeObject PyVTKNumberReference_Type;
extern VTKWRAPPINGPYTHONCORE_EXPORT PyTypeObject PyVTKStringReference_Type;
extern VTKWRAPPINGPYTHONCORE_EXPORT PyTypeObject PyVTKTupleReference_Type;

#define PyVTKReference_Check(obj) \
  PyObject_TypeCheck(obj, &PyVTKReference_Type)

extern "C"
{
// Set the value held by a mutable object.  It steals the reference
// of the provided value.  Only float, long, and int are allowed.
// A return value of -1 indicates than an error occurred.
VTKWRAPPINGPYTHONCORE_EXPORT
int PyVTKReference_SetValue(PyObject *self, PyObject *val);

// Get the value held by a mutable object.  A borrowed reference
// is returned.
VTKWRAPPINGPYTHONCORE_EXPORT
PyObject *PyVTKReference_GetValue(PyObject *self);
}

#endif
