/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkmAverageToPoints
 * @brief   Accelerated point to cell interpolation filter.
 *
 * vtkmAverageToPoints is a filter that transforms point data (i.e., data
 * specified at cell points) into cell data (i.e., data specified per cell).
 * The method of transformation is based on averaging the data
 * values of all points used by particular cell. This filter will also
 * pass through any existing point and cell arrays.
 *
*/

#ifndef vtkmAverageToCells_h
#define vtkmAverageToCells_h

#include "vtkAcceleratorsVTKmModule.h" //required for correct implementation
#include "vtkDataSetAlgorithm.h"

class VTKACCELERATORSVTKM_EXPORT vtkmAverageToCells : public vtkDataSetAlgorithm
{
public:
  vtkTypeMacro(vtkmAverageToCells, vtkDataSetAlgorithm)
  void PrintSelf(ostream& os, vtkIndent indent) override;
  static vtkmAverageToCells* New();

protected:
  vtkmAverageToCells();
  ~vtkmAverageToCells();

  virtual int RequestData(vtkInformation*, vtkInformationVector**,
                          vtkInformationVector*) override;

private:
  vtkmAverageToCells(const vtkmAverageToCells&) = delete;
  void operator=(const vtkmAverageToCells&) = delete;
};

#endif // vtkmAverageToCells_h
// VTK-HeaderTest-Exclude: vtkmAverageToCells.h
