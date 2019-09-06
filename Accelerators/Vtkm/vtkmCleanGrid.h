/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkmCleanGrid
 * @brief   removes redundant or unused cells and/or points
 *
 * vtkmCleanGrid is a filter that takes vtkDataSet data as input and
 * generates vtkUnstructuredGrid as output. vtkmCleanGrid will convert all cells
 * to an explicit representation, and if enabled, will remove unused points.
 *
*/

#ifndef vtkmCleanGrid_h
#define vtkmCleanGrid_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkAcceleratorsVTKmModule.h" //required for correct implementation

class vtkDataSet;
class vtkUnstructuredGrid;

class VTKACCELERATORSVTKM_EXPORT vtkmCleanGrid : public vtkUnstructuredGridAlgorithm
{
public:
  vtkTypeMacro(vtkmCleanGrid, vtkUnstructuredGridAlgorithm)

  void PrintSelf(ostream& os, vtkIndent indent) override;
  static vtkmCleanGrid* New();

  //@{
  /**
   * Get/Set if the points from the input that are unused in the output should
   * be removed. This will take extra time but the result dataset may use
   * less memory. Off by default.
   */
  vtkSetMacro(CompactPoints, bool);
  vtkGetMacro(CompactPoints, bool);
  vtkBooleanMacro(CompactPoints, bool);
  //@}

protected:
  vtkmCleanGrid();
  ~vtkmCleanGrid();

  int FillInputPortInformation(int, vtkInformation *) override;
  int RequestData(vtkInformation *, vtkInformationVector **,
                  vtkInformationVector *) override;

  bool CompactPoints;

private:
  vtkmCleanGrid(const vtkmCleanGrid&) = delete;
  void operator=(const vtkmCleanGrid&) = delete;
};

#endif // vtkmCleanGrid_h
// VTK-HeaderTest-Exclude: vtkmCleanGrid.h
