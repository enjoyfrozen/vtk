/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkUnstructuredGridCellIterator
 * @brief   Implementation of vtkCellIterator
 * specialized for vtkUnstructuredGrid.
*/

#ifndef vtkUnstructuredGridCellIterator_h
#define vtkUnstructuredGridCellIterator_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkCellIterator.h"
#include "vtkSmartPointer.h" // For vtkSmartPointer

class vtkCellArray;
class vtkUnsignedCharArray;
class vtkUnstructuredGrid;
class vtkPoints;

class VTKCOMMONDATAMODEL_EXPORT vtkUnstructuredGridCellIterator :
    public vtkCellIterator
{
public:
  static vtkUnstructuredGridCellIterator *New();
  vtkTypeMacro(vtkUnstructuredGridCellIterator, vtkCellIterator)
  void PrintSelf(ostream& os, vtkIndent indent) override;

  bool IsDoneWithTraversal() override;
  vtkIdType GetCellId() override;

protected:
  vtkUnstructuredGridCellIterator();
  ~vtkUnstructuredGridCellIterator() override;

  void ResetToFirstCell() override;
  void IncrementToNextCell() override;
  void FetchCellType() override;
  void FetchPointIds() override;
  void FetchPoints() override;
  void FetchFaces() override;

  friend class vtkUnstructuredGrid;
  void SetUnstructuredGrid(vtkUnstructuredGrid *ug);

  unsigned char *CellTypeBegin;
  unsigned char *CellTypePtr;
  unsigned char *CellTypeEnd;

  vtkIdType *ConnectivityBegin;
  vtkIdType *ConnectivityPtr;
  vtkIdType *FacesBegin;
  vtkIdType *FacesLocsBegin;
  vtkIdType *FacesLocsPtr;

  // Cache misses make updating ConnectivityPtr in IncrementToNextCell too
  // expensive, so we wait to walk through the array until the point ids are
  // needed. This variable keeps track of how far we need to increment.
  vtkIdType SkippedCells;
  void CatchUpSkippedCells();

  vtkSmartPointer<vtkPoints> UnstructuredGridPoints;

private:
  vtkUnstructuredGridCellIterator(const vtkUnstructuredGridCellIterator &) = delete;
  void operator=(const vtkUnstructuredGridCellIterator &) = delete;
};

#endif //vtkUnstructuredGridCellIterator_h
