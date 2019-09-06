/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
/**
 * @class   vtkBar
 * @brief   Bar class for vtk
 *
 * None.
*/

#ifndef vtkBar_h
#define vtkBar_h

#include "vtkObject.h"
#include "vtkmyCommonWin32Header.h"

class VTK_MY_COMMON_EXPORT vtkBar : public vtkObject
{
public:
  static vtkBar *New();
  vtkTypeMacro(vtkBar,vtkObject);

protected:
  vtkBar() {}
  ~vtkBar() override {}
private:
  vtkBar(const vtkBar&) = delete;
  void operator=(const vtkBar&) = delete;
};

#endif
