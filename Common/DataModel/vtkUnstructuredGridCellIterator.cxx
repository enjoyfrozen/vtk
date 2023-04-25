/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkUnstructuredGridCellIterator.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkUnstructuredGridCellIterator.h"

#include "vtkCellArray.h"
#include "vtkCellArrayIterator.h"
#include "vtkIdList.h"
#include "vtkIdTypeArray.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnstructuredGrid.h"

#include <cassert>

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkUnstructuredGridCellIterator);

//------------------------------------------------------------------------------
void vtkUnstructuredGridCellIterator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  if (this->Cells)
  {
    os << indent << "Cells:\n";
    this->Cells->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << indent << "Cells: (none)" << endl;
  }

  if (this->Types)
  {
    os << indent << "Types:\n";
    this->Types->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << indent << "Types: (none)" << endl;
  }

  if (this->PolyFaceConn)
  {
    os << indent << "FaceConn:\n";
    this->PolyFaceConn->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << indent << "FaceConn: (none)" << endl;
  }

  if (this->PolyFaceLocs)
  {
    os << indent << "FaceLocs:\n";
    this->PolyFaceLocs->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << indent << "FaceLocs: (none)" << endl;
  }

  if (this->Coords)
  {
    os << indent << "Coords:\n";
    this->Coords->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << indent << "Coords: (none)" << endl;
  }
}

//------------------------------------------------------------------------------
void vtkUnstructuredGridCellIterator::SetUnstructuredGrid(vtkUnstructuredGrid* ug)
{
  // If the unstructured grid has not been initialized yet, these may not exist:
  vtkUnsignedCharArray* cellTypeArray = ug ? ug->GetCellTypesArray() : nullptr;
  vtkCellArray* cellArray = ug ? ug->GetCells() : nullptr;
  vtkPoints* points = ug ? ug->GetPoints() : nullptr;

  if (points)
  {
    this->Points->SetDataType(points->GetDataType());
  }

  if (ug && cellTypeArray && cellArray && points)
  {
    this->Cells = vtk::TakeSmartPointer(cellArray->NewIterator());
    this->Cells->GoToFirstCell();

    this->Types = cellTypeArray;
    this->PolyFaceConn = ug->GetPolyhedronFaces();
    this->PolyFaceLocs = ug->GetPolyhedronFaceLocations();
    this->Coords = points;
  }
}

//------------------------------------------------------------------------------
bool vtkUnstructuredGridCellIterator::IsDoneWithTraversal()
{
  return this->Cells ? this->Cells->IsDoneWithTraversal() : true;
}

//------------------------------------------------------------------------------
vtkIdType vtkUnstructuredGridCellIterator::GetCellId()
{
  return this->Cells->GetCurrentCellId();
}

//------------------------------------------------------------------------------
void vtkUnstructuredGridCellIterator::IncrementToNextCell()
{
  this->Cells->GoToNextCell();
}

//------------------------------------------------------------------------------
vtkUnstructuredGridCellIterator::vtkUnstructuredGridCellIterator() = default;

//------------------------------------------------------------------------------
vtkUnstructuredGridCellIterator::~vtkUnstructuredGridCellIterator() = default;

//------------------------------------------------------------------------------
void vtkUnstructuredGridCellIterator::ResetToFirstCell()
{
  if (this->Cells)
  {
    this->Cells->GoToFirstCell();
  }
}

//------------------------------------------------------------------------------
void vtkUnstructuredGridCellIterator::FetchCellType()
{
  const vtkIdType cellId = this->Cells->GetCurrentCellId();
  this->CellType = this->Types->GetValue(cellId);
}

//------------------------------------------------------------------------------
void vtkUnstructuredGridCellIterator::FetchPointIds()
{
  this->Cells->GetCurrentCell(this->PointIds);
}

//------------------------------------------------------------------------------
void vtkUnstructuredGridCellIterator::FetchPoints()
{
  this->Coords->GetPoints(this->GetPointIds(), this->Points);
}

//------------------------------------------------------------------------------
void vtkUnstructuredGridCellIterator::FetchFaces()
{
  if (this->PolyFaceLocs)
  {
    const vtkIdType cellId = this->Cells->GetCurrentCellId();
    const vtkIdType* faceIds;
    vtkIdType nfaces;
    this->PolyFaceLocs->GetCellAtId(cellId, nfaces, faceIds);
    vtkIdType facesSize = 1;
    for (vtkIdType faceNum = 0; faceNum < nfaces; ++faceNum)
    {
      facesSize += this->PolyFaceConn->GetCellSize(faceIds[faceNum]) + 1;
    }
    this->Faces->SetNumberOfIds(facesSize);
    this->Faces->SetId(0, nfaces);
    vtkIdType loc = 0;
    const vtkIdType* facePtr;
    for (vtkIdType faceNum = 0; faceNum < nfaces; ++faceNum)
    {
      vtkIdType npts;
      this->PolyFaceConn->GetCellAtId(faceIds[faceNum], npts, facePtr);
      this->Faces->SetId(++loc, npts);
      for (vtkIdType i = 0; i < npts; ++i)
      {
        this->Faces->SetId(++loc, facePtr[i]);
      }
    }
  }
  else
  {
    this->Faces->SetNumberOfIds(0);
  }
}
VTK_ABI_NAMESPACE_END
