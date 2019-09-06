/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkRemoveGhosts
 * @brief   Remove ghost points, cells and arrays
 *
 *
 * Removes ghost points, cells and associated data arrays. Works on
 * vtkPolyDatas and vtkUnstructuredGrids.
*/

#ifndef vtkRemoveGhosts_h
#define vtkRemoveGhosts_h

#include "vtkFiltersParallelModule.h" // For export macro
#include "vtkPassInputTypeAlgorithm.h"

class vtkPolyData;
class vtkUnsignedCharArray;

class VTKFILTERSPARALLEL_EXPORT vtkRemoveGhosts : public vtkPassInputTypeAlgorithm
{
public:
  vtkTypeMacro(vtkRemoveGhosts, vtkPassInputTypeAlgorithm);
  void PrintSelf(ostream &os, vtkIndent indent) override;

  static vtkRemoveGhosts *New();

protected:
  vtkRemoveGhosts();
  ~vtkRemoveGhosts() override;

  int RequestUpdateExtent(vtkInformation *,
                          vtkInformationVector **,
                          vtkInformationVector *) override;
  int RequestData(vtkInformation *, vtkInformationVector **,
                  vtkInformationVector *) override;

  // see algorithm for more info
  int FillInputPortInformation(int port, vtkInformation* info) override;

private:
  vtkRemoveGhosts(const vtkRemoveGhosts &) = delete;
  void operator=(const vtkRemoveGhosts &) = delete;
};

#endif //_vtkRemoveGhosts_h
