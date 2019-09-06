/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkContextItem
 * @brief   base class for items that are part of a vtkContextScene.
 *
 *
 * Derive from this class to create custom items that can be added to a
 * vtkContextScene.
*/

#ifndef vtkContextItem_h
#define vtkContextItem_h

#include "vtkRenderingContext2DModule.h" // For export macro
#include "vtkAbstractContextItem.h"

class VTKRENDERINGCONTEXT2D_EXPORT vtkContextItem : public vtkAbstractContextItem
{
public:
  vtkTypeMacro(vtkContextItem, vtkAbstractContextItem);
  void PrintSelf(ostream &os, vtkIndent indent) override;

  //@{
  /**
   * Get the opacity of the item.
   */
  vtkGetMacro(Opacity, double);
  //@}

  //@{
  /**
   * Set the opacity of the item.
   * 1.0 by default.
   */
  vtkSetMacro(Opacity, double);
  //@}

protected:
  vtkContextItem();
  ~vtkContextItem() override;

  double Opacity;

private:
  vtkContextItem(const vtkContextItem &) = delete;
  void operator=(const vtkContextItem &) = delete;

};

#endif //vtkContextItem_h
