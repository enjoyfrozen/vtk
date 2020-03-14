/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkStarIterator.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef vtkStarIterator_h
#define vtkStarIterator_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkDataObject.h"
#include "vtkNew.h"

#include <functional>

class vtkCell;
class vtkDataSet;
class vtkIdList;
class vtkVector3d;

class VTKCOMMONDATAMODEL_EXPORT vtkStarIterator : public vtkObject
{
public:
  static vtkStarIterator* New();
  vtkTypeMacro(vtkStarIterator, vtkObject);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  /// Return codes visitors supply to control iteration.
  enum VisitAction
  {
    CONTINUE, //!< Continue visiting items.
    HALT      //!< Abandon further iteration.
  };

  using CellIdVisitor = std::function<VisitAction(vtkDataSet*, vtkIdType)>;
  using CellVisitor = std::function<VisitAction(vtkDataSet*, vtkCell*, vtkIdType)>;
  using PointIdVisitor = std::function<VisitAction(vtkDataSet*, vtkIdType)>;
  using PointVisitor = std::function<VisitAction(vtkDataSet*, const vtkVector3d&, vtkIdType)>;

  // void VisitStar1Cells(vtkDataSet* mesh, vtkIdType pointId, CellVisitor visitor);
  // void VisitStar1CellIds(vtkDataSet* mesh, vtkIdType pointId, CellIdVisitor visitor);

  void VisitStar1PointIds(vtkDataSet* mesh, vtkIdType pointId, PointIdVisitor visitor);

protected:
  vtkStarIterator();
  ~vtkStarIterator() override;

  vtkNew<vtkIdList> CellsAttachedToPt;
  vtkNew<vtkIdList> CellCornerPts;
};

#endif // vtkStarIterator_h
