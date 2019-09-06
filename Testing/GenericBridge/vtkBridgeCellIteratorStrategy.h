/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkBridgeCellIteratorStrategy
 * @brief   Interface used by vtkBridgeCellIterator
 * vtkBridgeCellIterator has different behaviors depending on the way it is
 * initialized. vtkBridgeCellIteratorStrategy is the interface for one of those
 * behaviors. Concrete classes are vtkBridgeCellIteratorOnDataSet,
 * vtkBridgeCellIteratorOnDataSetBoundaries,
 * vtkBridgeCellIteratorOnCellBoundaries,
 * vtkBridgeCellIteratorOnCellNeighbors,
 * @sa
 * vtkGenericCellIterator, vtkBridgeCellIterator, vtkBridgeDataSet, vtkBridgeCellIteratorOnDataSet, vtkBridgeCellIteratorOnDataSetBoundaries, vtkBridgeCellIteratorOnCellBoundaries, vtkBridgeCellIteratorOnCellNeighbors
*/

#ifndef vtkBridgeCellIteratorStrategy_h
#define vtkBridgeCellIteratorStrategy_h

#include "vtkBridgeExport.h" //for module export macro
#include "vtkGenericCellIterator.h"

class vtkBridgeCell;
class vtkBridgeDataSet;
class vtkBridgeCell;
class vtkIdList;

class VTKTESTINGGENERICBRIDGE_EXPORT vtkBridgeCellIteratorStrategy : public vtkGenericCellIterator
{
public:
  vtkTypeMacro(vtkBridgeCellIteratorStrategy,vtkGenericCellIterator);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Create an empty cell. NOT USED
   * \post result_exists: result!=0
   */
  vtkGenericAdaptorCell *NewCell() override;

protected:
  vtkBridgeCellIteratorStrategy() {}
  ~vtkBridgeCellIteratorStrategy() override {}

private:
  vtkBridgeCellIteratorStrategy(const vtkBridgeCellIteratorStrategy&) = delete;
  void operator=(const vtkBridgeCellIteratorStrategy&) = delete;
};

#endif
