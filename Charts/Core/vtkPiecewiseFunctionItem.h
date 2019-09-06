/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef vtkPiecewiseFunctionItem_h
#define vtkPiecewiseFunctionItem_h

#include "vtkChartsCoreModule.h" // For export macro
#include "vtkScalarsToColorsItem.h"

class vtkPiecewiseFunction;
class vtkImageData;

/// vtkPiecewiseFunctionItem internal uses vtkPlot::Color, white by default
class VTKCHARTSCORE_EXPORT vtkPiecewiseFunctionItem: public vtkScalarsToColorsItem
{
public:
  static vtkPiecewiseFunctionItem* New();
  vtkTypeMacro(vtkPiecewiseFunctionItem, vtkScalarsToColorsItem);
  void PrintSelf(ostream &os, vtkIndent indent) override;

  void SetPiecewiseFunction(vtkPiecewiseFunction* t);
  vtkGetObjectMacro(PiecewiseFunction, vtkPiecewiseFunction);

protected:
  vtkPiecewiseFunctionItem();
  ~vtkPiecewiseFunctionItem() override;

  // Description:
  // Reimplemented to return the range of the piecewise function
  void ComputeBounds(double bounds[4]) override;

  // Description
  // Compute the texture from the PiecewiseFunction
  void ComputeTexture() override;

  vtkPiecewiseFunction* PiecewiseFunction;

private:
  vtkPiecewiseFunctionItem(const vtkPiecewiseFunctionItem &) = delete;
  void operator=(const vtkPiecewiseFunctionItem &) = delete;
};

#endif
