/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkCastToConcrete
 * @brief   works around type-checking limitations
 *
 * vtkCastToConcrete is a filter that works around type-checking limitations
 * in the filter classes. Some filters generate abstract types on output,
 * and cannot be connected to the input of filters requiring a concrete
 * input type. For example, vtkElevationFilter generates vtkDataSet for output,
 * and cannot be connected to vtkDecimate, because vtkDecimate requires
 * vtkPolyData as input. This is true even though (in this example) the input
 * to vtkElevationFilter is of type vtkPolyData, and you know the output of
 * vtkElevationFilter is the same type as its input.
 *
 * vtkCastToConcrete performs run-time checking to insure that output type
 * is of the right type. An error message will result if you try to cast
 * an input type improperly. Otherwise, the filter performs the appropriate
 * cast and returns the data.
 *
 * @warning
 * You must specify the input before you can get the output. Otherwise an
 * error results.
 *
 * @sa
 * vtkDataSetAlgorithm vtkPointSetToPointSetFilter
*/

#ifndef vtkCastToConcrete_h
#define vtkCastToConcrete_h

#include "vtkCommonExecutionModelModule.h" // For export macro
#include "vtkDataSetAlgorithm.h"

class VTKCOMMONEXECUTIONMODEL_EXPORT vtkCastToConcrete : public vtkDataSetAlgorithm
{

public:
  static vtkCastToConcrete *New();
  vtkTypeMacro(vtkCastToConcrete,vtkDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkCastToConcrete() {}
  ~vtkCastToConcrete() override {}

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override; //insures compatibility; satisfies abstract api in vtkFilter
  int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
private:
  vtkCastToConcrete(const vtkCastToConcrete&) = delete;
  void operator=(const vtkCastToConcrete&) = delete;
};

#endif
