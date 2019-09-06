/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef vtkCompositeTransferFunctionItem_h
#define vtkCompositeTransferFunctionItem_h

#include "vtkChartsCoreModule.h" // For export macro
#include "vtkColorTransferFunctionItem.h"

class vtkPiecewiseFunction;

// Description:
// vtkPlot::Color and vtkPlot::Brush have no effect here.
class VTKCHARTSCORE_EXPORT vtkCompositeTransferFunctionItem: public vtkColorTransferFunctionItem
{
public:
  static vtkCompositeTransferFunctionItem* New();
  vtkTypeMacro(vtkCompositeTransferFunctionItem, vtkColorTransferFunctionItem);
  void PrintSelf(ostream &os, vtkIndent indent) override;

  void SetOpacityFunction(vtkPiecewiseFunction* opacity);
  vtkGetObjectMacro(OpacityFunction, vtkPiecewiseFunction);

protected:
  vtkCompositeTransferFunctionItem();
  ~vtkCompositeTransferFunctionItem() override;

  // Description:
  // Reimplemented to return the range of the piecewise function
  void ComputeBounds(double bounds[4]) override;

  void ComputeTexture() override;
  vtkPiecewiseFunction* OpacityFunction;

private:
  vtkCompositeTransferFunctionItem(const vtkCompositeTransferFunctionItem&) = delete;
  void operator=(const vtkCompositeTransferFunctionItem&) = delete;
};

#endif
