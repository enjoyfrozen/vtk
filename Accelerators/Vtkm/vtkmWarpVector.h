/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkmWarpVector
 * @brief   deform geometry with vector data
 *
 * vtkWarpVector is a filter that modifies point coordinates by moving
 * points along vector times the scale factor. Useful for showing flow
 * profiles or mechanical deformation.
 *
 * The filter passes both its point data and cell data to its output.
*/

#ifndef vtkmWarpVector_h
#define vtkmWarpVector_h

#include "vtkWarpVector.h"
#include "vtkAcceleratorsVTKmModule.h" // required for correct export

class VTKACCELERATORSVTKM_EXPORT vtkmWarpVector : public vtkWarpVector
{
public:
  vtkTypeMacro(vtkmWarpVector, vtkWarpVector)
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkmWarpVector* New();

protected:
  vtkmWarpVector();
  ~vtkmWarpVector();

  int RequestData(vtkInformation* , vtkInformationVector**,
                          vtkInformationVector*) override;

private:
  vtkmWarpVector(const vtkmWarpVector&) = delete;
  void operator=(const vtkmWarpVector&) = delete;
};

#endif // vtkmWarpVector_h

// VTK-HeaderTest-Exclude: vtkmWarpVector.h
