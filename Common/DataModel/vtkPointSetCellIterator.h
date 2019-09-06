/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkPointSetCellIterator
 * @brief   Implementation of vtkCellIterator using
 * vtkPointSet API.
*/

#ifndef vtkPointSetCellIterator_h
#define vtkPointSetCellIterator_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkCellIterator.h"
#include "vtkSmartPointer.h" // For vtkSmartPointer

class vtkPoints;
class vtkPointSet;

class VTKCOMMONDATAMODEL_EXPORT vtkPointSetCellIterator: public vtkCellIterator
{
public:
  static vtkPointSetCellIterator *New();
  vtkTypeMacro(vtkPointSetCellIterator, vtkCellIterator)
  void PrintSelf(ostream& os, vtkIndent indent) override;

  bool IsDoneWithTraversal() override;
  vtkIdType GetCellId() override;

protected:
  vtkPointSetCellIterator();
  ~vtkPointSetCellIterator() override;

  void ResetToFirstCell() override;
  void IncrementToNextCell() override;
  void FetchCellType() override;
  void FetchPointIds() override;
  void FetchPoints() override;

  friend class vtkPointSet;
  void SetPointSet(vtkPointSet *ds);

  vtkSmartPointer<vtkPointSet> PointSet;
  vtkSmartPointer<vtkPoints> PointSetPoints;
  vtkIdType CellId;

private:
  vtkPointSetCellIterator(const vtkPointSetCellIterator &) = delete;
  void operator=(const vtkPointSetCellIterator &) = delete;
};

#endif //vtkPointSetCellIterator_h
