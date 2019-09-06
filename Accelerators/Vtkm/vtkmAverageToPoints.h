/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkmAverageToPoints
 * @brief   Accelerated cell to point interpolation filter.
 *
 * vtkmAverageToPoints is a filter that transforms cell data (i.e., data
 * specified per cell) into point data (i.e., data specified at cell
 * points). The method of transformation is based on averaging the data
 * values of all cells using a particular point. This filter will also
 * pass through any existing point and cell arrays.
 *
*/
#ifndef vtkmAverageToPoints_h
#define vtkmAverageToPoints_h

#include "vtkDataSetAlgorithm.h"
#include "vtkAcceleratorsVTKmModule.h" //required for correct implementation

class VTKACCELERATORSVTKM_EXPORT vtkmAverageToPoints : public vtkDataSetAlgorithm
{
public:
  vtkTypeMacro(vtkmAverageToPoints,vtkDataSetAlgorithm)
  void PrintSelf(ostream& os, vtkIndent indent) override;
  static vtkmAverageToPoints* New();

protected:
  vtkmAverageToPoints();
  ~vtkmAverageToPoints();

  virtual int RequestData(vtkInformation *,
                          vtkInformationVector **,
                          vtkInformationVector *) override;

private:
  vtkmAverageToPoints(const vtkmAverageToPoints&) = delete;
  void operator=(const vtkmAverageToPoints&) = delete;
};

#endif // vtkmAverageToPoints_h
// VTK-HeaderTest-Exclude: vtkmAverageToPoints.h
