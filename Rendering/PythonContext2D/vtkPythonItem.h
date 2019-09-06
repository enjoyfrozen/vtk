/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkPythonItem
 * @brief   A vtkContextItem that can be implemented in Python
 *
 *
 * This class allows implementation of arbitrary context items in Python.
 *
 * @sa
 * vtkAbstractContextItem
*/

#ifndef vtkPythonItem_h
#define vtkPythonItem_h
#if !defined(__VTK_WRAP__) || defined(__VTK_WRAP_HIERARCHY__) || defined(__VTK_WRAP_PYTHON__)

#include "vtkPython.h" // Must be first

#include "vtkPythonContext2DModule.h" // For export macro
#include "vtkContextItem.h"

class vtkSmartPyObject;

class VTKPYTHONCONTEXT2D_EXPORT vtkPythonItem : public vtkContextItem
{
public:
  vtkTypeMacro(vtkPythonItem, vtkContextItem);
  void PrintSelf(ostream &os, vtkIndent indent) override;

  static vtkPythonItem * New();

  /**
   * Specify the Python object to use to operate on the data. A reference will
   * be taken on the object. This will also invoke Initialize() on the Python
   * object, providing an opportunity to perform tasks commonly done in the
   * constructor of C++ vtkContextItem subclasses.
   */
  void SetPythonObject(PyObject* obj);

  bool Paint(vtkContext2D *painter) override;

protected:
  vtkPythonItem();
  ~vtkPythonItem() override;

private:
  vtkPythonItem(const vtkPythonItem &) = delete;
  void operator=(const vtkPythonItem &) = delete;

  bool CheckResult(const char* method, const vtkSmartPyObject& res);

  PyObject* Object;
};

#endif // #ifndef vtkPythonItem_h
#endif
