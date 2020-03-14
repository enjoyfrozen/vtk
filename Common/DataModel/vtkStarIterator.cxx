#include "vtkStarIterator.h"

#include "vtkDataSet.h"
#include "vtkIdList.h"
#include "vtkObjectFactory.h"
#include "vtkVector.h"

#include <set>

vtkStandardNewMacro(vtkStarIterator);

vtkStarIterator::vtkStarIterator()
{
}

vtkStarIterator::~vtkStarIterator()
{
}

void vtkStarIterator::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "CellsAttachedToPt: " << this->CellsAttachedToPt << "\n";
  os << indent << "CellCornerPts: " << this->CellCornerPts << "\n";
}

// void vtkStarIterator::VisitStar1Cells(vtkDataSet* mesh, vtkIdType pointId, CellVisitor visitor);
// void vtkStarIterator::VisitStar1CellIds(vtkDataSet* mesh, vtkIdType pointId, CellIdVisitor visitor);

void vtkStarIterator::VisitStar1PointIds(vtkDataSet* mesh, vtkIdType pointId, PointIdVisitor visitor)
{
  if (!mesh || !visitor)
  {
    return;
  }
  std::set<vtkIdType> visited;
  visited.insert(pointId); // Never visit the center of the star.

  mesh->GetPointCells(pointId, this->CellsAttachedToPt);
  for (auto cellIt = this->CellsAttachedToPt->begin(); cellIt != this->CellsAttachedToPt->end(); ++cellIt)
  {
    mesh->GetCellPoints(*cellIt, this->CellCornerPts);
    for (auto pointIt = this->CellCornerPts->begin(); pointIt != this->CellCornerPts->end(); ++pointIt)
    {
      if (visited.find(*pointIt) == visited.end())
      {
        visited.insert(*pointIt);
        auto earlyExit = visitor(mesh, *pointIt);
        if (earlyExit == VisitAction::HALT)
        {
          return;
        }
      }
    }
  }
}
