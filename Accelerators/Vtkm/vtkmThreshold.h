/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkmThreshold
 * @brief   extracts cells where scalar value in cell satisfies threshold criterion
 *
 * vtkmThreshold is a filter that extracts cells from any dataset type that
 * satisfy a threshold criterion. A cell satisfies the criterion if the
 * scalar value of every point or cell satisfies the criterion. The
 * criterion takes the form of between two values. The output of this
 * filter is an unstructured grid.
 *
 * Note that scalar values are available from the point and cell attribute
 * data. By default, point data is used to obtain scalars, but you can
 * control this behavior. See the AttributeMode ivar below.
 *
*/
#ifndef vtkmThreshold_h
#define vtkmThreshold_h

#include "vtkThreshold.h"
#include "vtkAcceleratorsVTKmModule.h" //required for correct implementation

class VTKACCELERATORSVTKM_EXPORT vtkmThreshold : public vtkThreshold
{
public:
  vtkTypeMacro(vtkmThreshold,vtkThreshold)
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkmThreshold* New();

protected:
  vtkmThreshold();
  ~vtkmThreshold();

  virtual int RequestData(vtkInformation*, vtkInformationVector**,
                          vtkInformationVector*) override;

private:
  vtkmThreshold(const vtkmThreshold&) = delete;
  void operator=(const vtkmThreshold&) = delete;
};

#endif // vtkmThreshold_h
// VTK-HeaderTest-Exclude: vtkmThreshold.h
