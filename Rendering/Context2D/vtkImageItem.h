/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkImageItem
 * @brief   a vtkContextItem that draws a supplied image in the
 * scene.
 *
 *
 * This vtkContextItem draws the supplied image in the scene.
*/

#ifndef vtkImageItem_h
#define vtkImageItem_h

#include "vtkRenderingContext2DModule.h" // For export macro
#include "vtkContextItem.h"
#include "vtkSmartPointer.h" // For SP ivars.

class vtkImageData;

class VTKRENDERINGCONTEXT2D_EXPORT vtkImageItem : public vtkContextItem
{
public:
  vtkTypeMacro(vtkImageItem, vtkContextItem);
  void PrintSelf(ostream &os, vtkIndent indent) override;

  static vtkImageItem *New();

  /**
   * Paint event for the item.
   */
  bool Paint(vtkContext2D *painter) override;

  /**
   * Set the image of the item.
   */
  void SetImage(vtkImageData *image);

  //@{
  /**
   * Get the image of the item.
   */
  vtkGetObjectMacro(Image, vtkImageData);
  //@}

  //@{
  /**
   * Set the position of the bottom corner of the image.
   */
  vtkSetVector2Macro(Position, float);
  //@}

  //@{
  /**
   * Get the position of the bottom corner of the image.
   */
  vtkGetVector2Macro(Position, float);
  //@}

protected:
  vtkImageItem();
  ~vtkImageItem() override;

  float Position[2];

  vtkImageData *Image;

private:
  vtkImageItem(const vtkImageItem &) = delete;
  void operator=(const vtkImageItem &) = delete;

};

#endif //vtkImageItem_h
