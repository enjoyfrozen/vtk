// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkStarIterator.h"

#include "vtkDataSet.h"
#include "vtkIdList.h"
#include "vtkObjectFactory.h"
#include "vtkVector.h"

#include <unordered_set>

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkStarIterator);

//------------------------------------------------------------------------------
void vtkStarIterator::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "CellsAttachedToPt: " << this->CellsAttachedToPt << "\n";
  os << indent << "CellCornerPts: " << this->CellCornerPts << "\n";
}

//------------------------------------------------------------------------------
void vtkStarIterator::VisitStar1PointIds(
  vtkDataSet* mesh, vtkIdType pointId, const vtkStarIterator::PointIdVisitor& visitor)
{
  if (!mesh || !visitor)
  {
    return;
  }
  std::unordered_set<vtkIdType> visited;
  visited.insert(pointId); // Never visit the center of the star.

  mesh->GetPointCells(pointId, this->CellsAttachedToPt);
  for (auto& cellIt : *this->CellsAttachedToPt)
  {
    mesh->GetCellPoints(cellIt, this->CellCornerPts);
    for (auto& pointIt : *this->CellCornerPts)
    {
      if (visited.find(pointIt) == visited.end())
      {
        visited.insert(pointIt);
        auto earlyExit = visitor(mesh, pointIt);
        if (earlyExit == VisitAction::HALT)
        {
          return;
        }
      }
    }
  }
}
VTK_ABI_NAMESPACE_END
