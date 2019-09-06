/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkDataSetCellIterator
 * @brief   Implementation of vtkCellIterator using
 * vtkDataSet API.
*/

#ifndef vtkDataSetCellIterator_h
#define vtkDataSetCellIterator_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkCellIterator.h"
#include "vtkSmartPointer.h" // For vtkSmartPointer

class vtkDataSet;

class VTKCOMMONDATAMODEL_EXPORT vtkDataSetCellIterator: public vtkCellIterator
{
public:
  static vtkDataSetCellIterator *New();
  vtkTypeMacro(vtkDataSetCellIterator, vtkCellIterator)
  void PrintSelf(ostream& os, vtkIndent indent) override;

  bool IsDoneWithTraversal() override;
  vtkIdType GetCellId() override;

protected:
  vtkDataSetCellIterator();
  ~vtkDataSetCellIterator() override;

  void ResetToFirstCell() override;
  void IncrementToNextCell() override;
  void FetchCellType() override;
  void FetchPointIds() override;
  void FetchPoints() override;

  friend class vtkDataSet;
  void SetDataSet(vtkDataSet *ds);

  vtkSmartPointer<vtkDataSet> DataSet;
  vtkIdType CellId;

private:
  vtkDataSetCellIterator(const vtkDataSetCellIterator &) = delete;
  void operator=(const vtkDataSetCellIterator &) = delete;
};

#endif //vtkDataSetCellIterator_h
