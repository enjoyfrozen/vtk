/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkInteractorStyleSwitchBase
 * @brief   dummy interface class.
 *
 * The class vtkInteractorStyleSwitchBase is here to allow the
 * vtkRenderWindowInteractor to instantiate a default interactor style and
 * preserve backward compatible behavior when the object factory is overridden
 * and vtkInteractorStyleSwitch is returned.
 *
 * @sa
 * vtkInteractorStyleSwitchBase vtkRenderWindowInteractor
*/

#ifndef vtkInteractorStyleSwitchBase_h
#define vtkInteractorStyleSwitchBase_h

#include "vtkRenderingCoreModule.h" // For export macro
#include "vtkInteractorStyle.h"

class VTKRENDERINGCORE_EXPORT vtkInteractorStyleSwitchBase
  : public vtkInteractorStyle
{
public:
  static vtkInteractorStyleSwitchBase *New();
  vtkTypeMacro(vtkInteractorStyleSwitchBase, vtkInteractorStyle);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkRenderWindowInteractor* GetInteractor() override;

protected:
  vtkInteractorStyleSwitchBase();
  ~vtkInteractorStyleSwitchBase() override;

private:
  vtkInteractorStyleSwitchBase(const vtkInteractorStyleSwitchBase&) = delete;
  void operator=(const vtkInteractorStyleSwitchBase&) = delete;
};

#endif
