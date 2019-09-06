/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "PyVTKEnum.h"
#include "vtkPythonUtil.h"

#include <cstddef>

//--------------------------------------------------------------------
// C API

//--------------------------------------------------------------------
// Add a wrapped enum type
PyTypeObject *PyVTKEnum_Add(PyTypeObject *pytype)
{
  // do not allow direct instantiation
  pytype->tp_new = nullptr;
  vtkPythonUtil::AddEnumToMap(pytype);
  return pytype;
}

//--------------------------------------------------------------------
PyObject *PyVTKEnum_New(PyTypeObject *pytype, int val)
{
  // our enums are subtypes of Python's int() type
#ifdef VTK_PY3K
  PyObject *args = Py_BuildValue("(i)", val);
  PyObject *obj = PyLong_Type.tp_new(pytype, args, nullptr);
  Py_DECREF(args);
  return obj;
#else
  PyIntObject *self = PyObject_New(PyIntObject, pytype);
  self->ob_ival = val;
  return (PyObject *)self;
#endif
}
