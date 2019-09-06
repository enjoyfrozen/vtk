/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkPointSetCellIterator.h"

#include "vtkPointSet.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkIdList.h"

vtkStandardNewMacro(vtkPointSetCellIterator)

//------------------------------------------------------------------------------
void vtkPointSetCellIterator::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "PointSet: " << this->PointSet << endl;
}

//------------------------------------------------------------------------------
void vtkPointSetCellIterator::SetPointSet(vtkPointSet *ds)
{
  this->PointSet = ds;
  this->PointSetPoints = ds ? ds->GetPoints() : nullptr;
  this->CellId = 0;
  if (this->PointSetPoints)
  {
    this->Points->SetDataType(this->PointSetPoints->GetDataType());
  }
}

//------------------------------------------------------------------------------
bool vtkPointSetCellIterator::IsDoneWithTraversal()
{
  return this->PointSet == nullptr
      || this->CellId >= this->PointSet->GetNumberOfCells();
}

//------------------------------------------------------------------------------
vtkIdType vtkPointSetCellIterator::GetCellId()
{
  return this->CellId;
}

//------------------------------------------------------------------------------
void vtkPointSetCellIterator::IncrementToNextCell()
{
  ++this->CellId;
}

//------------------------------------------------------------------------------
vtkPointSetCellIterator::vtkPointSetCellIterator()
  : vtkCellIterator(),
    PointSet(nullptr),
    PointSetPoints(nullptr),
    CellId(0)
{
}

//------------------------------------------------------------------------------
vtkPointSetCellIterator::~vtkPointSetCellIterator() = default;

//------------------------------------------------------------------------------
void vtkPointSetCellIterator::ResetToFirstCell()
{
  this->CellId = 0;
}

//------------------------------------------------------------------------------
void vtkPointSetCellIterator::FetchCellType()
{
  this->CellType = this->PointSet->GetCellType(this->CellId);
}

//------------------------------------------------------------------------------
void vtkPointSetCellIterator::FetchPointIds()
{
  this->PointSet->GetCellPoints(this->CellId, this->PointIds);
}

//------------------------------------------------------------------------------
void vtkPointSetCellIterator::FetchPoints()
{
  vtkIdList *pointIds = this->GetPointIds();
  this->PointSetPoints->GetPoints(pointIds, this->Points);
}
