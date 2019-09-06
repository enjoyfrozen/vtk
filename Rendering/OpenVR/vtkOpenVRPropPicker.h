/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
* @class   vtkOpenVRPropPicker
* @brief   Deprecated. Use vtkPropPicker directly
*/

#ifndef vtkOpenVRPropPicker_h
#define vtkOpenVRPropPicker_h

#include "vtkRenderingOpenVRModule.h" // For export macro
#include "vtkPropPicker.h"

class vtkProp;

class VTKRENDERINGOPENVR_EXPORT vtkOpenVRPropPicker : public vtkPropPicker
{
public:
  static vtkOpenVRPropPicker *New();

  vtkTypeMacro(vtkOpenVRPropPicker, vtkPropPicker);

  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkOpenVRPropPicker();
  ~vtkOpenVRPropPicker() override;

  void Initialize() override;


private:
  vtkOpenVRPropPicker(const vtkOpenVRPropPicker&) = delete;// Not implemented.
  void operator=(const vtkOpenVRPropPicker&) = delete;// Not implemented.
};

#endif
