/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkPropItem
 * @brief   Embed a vtkProp in a vtkContextScene.
 *
 *
 * This class allows vtkProp objects to be drawn inside a vtkContextScene.
 * This is especially useful for constructing layered scenes that need to ignore
 * depth testing.
*/

#ifndef vtkPropItem_h
#define vtkPropItem_h

#include "vtkRenderingContext2DModule.h" // For export macro
#include "vtkAbstractContextItem.h"

class vtkProp;

class VTKRENDERINGCONTEXT2D_EXPORT vtkPropItem: public vtkAbstractContextItem
{
public:
  static vtkPropItem *New();
  vtkTypeMacro(vtkPropItem, vtkAbstractContextItem)
  void PrintSelf(ostream &os, vtkIndent indent) override;

  bool Paint(vtkContext2D *painter) override;
  void ReleaseGraphicsResources() override;

  /**
   * The actor to render.
   */
  virtual void SetPropObject(vtkProp *PropObject);
  vtkGetObjectMacro(PropObject, vtkProp)

protected:
  vtkPropItem();
  ~vtkPropItem() override;

  // Sync the active vtkCamera with the GL state set by the painter.
  virtual void UpdateTransforms();

  // Restore the vtkCamera state.
  virtual void ResetTransforms();

private:
  vtkProp *PropObject;

  vtkPropItem(const vtkPropItem &) = delete;
  void operator=(const vtkPropItem &) = delete;
};

#endif //vtkPropItem_h
