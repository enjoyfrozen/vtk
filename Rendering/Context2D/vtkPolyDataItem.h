/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkPolyDataItem
 * @brief   Filter that translate a vtkPolyData 2D mesh into vtkContextItems.
 *
 * @warning
 * The input vtkPolyData should be a 2D mesh.
 *
 */

#ifndef vtkPolyDataItem_h
#define vtkPolyDataItem_h

#include "vtkRenderingContext2DModule.h" // For export macro
#include "vtkContextItem.h"


class vtkPolyData;
class vtkUnsignedCharArray;

class VTKRENDERINGCONTEXT2D_EXPORT vtkPolyDataItem : public vtkContextItem
{
public:
  vtkTypeMacro(vtkPolyDataItem, vtkContextItem);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkPolyDataItem *New();

  /**
   * Paint event for the item.
   */
  bool Paint(vtkContext2D* painter) override;

  /**
   * Set the PolyData of the item.
   */
  void SetPolyData(vtkPolyData* polyData);

  /**
   * Set mapped colors. User-selected scalars are mapped to a color lookup
   * table externally.
   */
  void SetMappedColors(vtkUnsignedCharArray* colors);

  /**
   * Get the image of the item.
   */
  vtkGetObjectMacro(PolyData, vtkPolyData);

  /**
   * Set the position of the bottom corner of the image.
   */
  vtkSetVector2Macro(Position, float);

  /**
   * Set the data scalar mode.
   */
  vtkSetMacro(ScalarMode, int);

protected:
  vtkPolyDataItem();
  ~vtkPolyDataItem() override;

  class DrawHintsHelper;
  DrawHintsHelper* HintHelper;

  float Position[2];

  vtkPolyData* PolyData;

  vtkUnsignedCharArray* MappedColors;

  int ScalarMode;

private:
  vtkPolyDataItem(const vtkPolyDataItem&) = delete;
  void operator=(const vtkPolyDataItem&) = delete;
};

#endif
