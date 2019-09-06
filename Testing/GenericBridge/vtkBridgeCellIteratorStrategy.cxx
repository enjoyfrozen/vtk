/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
// .NAME vtkBridgeCellIteratorStrategy - Interface used by vtkBridgeCellIterator
// vtkBridgeCellIterator has different behaviors depending on the way it is
// initialized. vtkBridgeCellIteratorStrategy is the interface for one of those
// behaviors. Concrete classes are vtkBridgeCellIteratorOnDataSet,
// vtkBridgeCellIteratorOnDataSetBoundaries,
// vtkBridgeCellIteratorOnCellBoundaries,
// vtkBridgeCellIteratorOnCellNeighbors,
// .SECTION See Also
// vtkCellIterator, vtkBridgeCellIterator, vtkBridgeDataSet, vtkBridgeCellIteratorOnDataSet, vtkBridgeCellIteratorOnDataSetBoundaries, vtkBridgeCellIteratorOnCellBoundaries, vtkBridgeCellIteratorOnCellNeighbors

#include "vtkBridgeCellIteratorStrategy.h"

#include <cassert>


//-----------------------------------------------------------------------------
void vtkBridgeCellIteratorStrategy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//-----------------------------------------------------------------------------
// Description:
// Create an empty cell. NOT USED
// \post result_exists: result!=0
vtkGenericAdaptorCell *vtkBridgeCellIteratorStrategy::NewCell()
{
  assert("check: should not be called: see vtkBridgeCellIterator::NewCell()"
         &&0);
  return nullptr;
}
