/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
/**
 * @class   vtkBar2
 * @brief   Bar2 class for vtk
 *
 * None.
*/

#ifndef vtkBar2_h
#define vtkBar2_h

#include "vtkObject.h"
#include "vtkmyUnsortedWin32Header.h"

class VTK_MY_UNSORTED_EXPORT vtkBar2 : public vtkObject
{
public:
  static vtkBar2 *New();
  vtkTypeMacro(vtkBar2,vtkObject);

protected:
  vtkBar2() {}
  ~vtkBar2() override {}
private:
  vtkBar2(const vtkBar2&) = delete;
  void operator=(const vtkBar2&) = delete;
};

#endif
