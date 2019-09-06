/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

/**
 * @class   vtkArrayToTable
 * @brief   Converts one- and two-dimensional vtkArrayData
 * objects to vtkTable
 *
 * @par Thanks:
 * Developed by Timothy M. Shead (tshead@sandia.gov) at Sandia National Laboratories.
*/

#ifndef vtkArrayToTable_h
#define vtkArrayToTable_h

#include "vtkInfovisCoreModule.h" // For export macro
#include "vtkTableAlgorithm.h"

class VTKINFOVISCORE_EXPORT vtkArrayToTable : public vtkTableAlgorithm
{
public:
  static vtkArrayToTable* New();
  vtkTypeMacro(vtkArrayToTable, vtkTableAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkArrayToTable();
  ~vtkArrayToTable() override;

  int FillInputPortInformation(int, vtkInformation*) override;

  int RequestData(
    vtkInformation*,
    vtkInformationVector**,
    vtkInformationVector*) override;

private:
  vtkArrayToTable(const vtkArrayToTable&) = delete;
  void operator=(const vtkArrayToTable&) = delete;
};

#endif

