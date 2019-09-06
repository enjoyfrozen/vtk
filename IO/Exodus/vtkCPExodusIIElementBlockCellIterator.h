/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkCPExodusIIElementBlockCellIterator
 * @brief   vtkCellIterator subclass
 * specialized for vtkCPExodusIIElementBlock.
*/

#ifndef vtkCPExodusIIElementBlockCellIterator_h
#define vtkCPExodusIIElementBlockCellIterator_h

#include "vtkCellIterator.h"
#include "vtkIOExodusModule.h" // For export macro

#include "vtkSmartPointer.h" // For smart pointer

class vtkCPExodusIIElementBlock;
class vtkCPExodusIIElementBlockPrivate;

class VTKIOEXODUS_EXPORT vtkCPExodusIIElementBlockCellIterator
    : public vtkCellIterator
{
public:
  typedef vtkCPExodusIIElementBlockPrivate StorageType;

  static vtkCPExodusIIElementBlockCellIterator *New();
  vtkTypeMacro(vtkCPExodusIIElementBlockCellIterator, vtkCellIterator)
  void PrintSelf(ostream &os, vtkIndent indent);

  bool IsValid();
  vtkIdType GetCellId();

protected:
  vtkCPExodusIIElementBlockCellIterator();
  ~vtkCPExodusIIElementBlockCellIterator();

  void ResetToFirstCell();
  void IncrementToNextCell();
  void FetchCellType();
  void FetchPointIds();
  void FetchPoints();

  friend class ::vtkCPExodusIIElementBlock;
  void SetStorage(vtkCPExodusIIElementBlock *eb);

private:
  vtkCPExodusIIElementBlockCellIterator(const vtkCPExodusIIElementBlockCellIterator &) = delete;
  void operator=(const vtkCPExodusIIElementBlockCellIterator &) = delete;

  vtkSmartPointer<StorageType> Storage;
  vtkSmartPointer<vtkPoints> DataSetPoints;
  vtkIdType CellId;
};

#endif //vtkCPExodusIIElementBlockCellIterator_h
