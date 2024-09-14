// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

/**
 * @class vtkStarIterator
 *
 * This class simplifies iteration over points that neighbor (via cell connectivity)
 * a given input point in unstructured data. It will call a provided functor once
 * for each neighbor point and never include the input \a pointId.
 */

#ifndef vtkStarIterator_h
#define vtkStarIterator_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkNew.h"                   // For vtkNew
#include "vtkObject.h"

#include <functional> // for std::function

VTK_ABI_NAMESPACE_BEGIN
class vtkDataSet;
class vtkIdList;

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

  using PointIdVisitor = std::function<VisitAction(vtkDataSet*, vtkIdType)>;

  void VisitStar1PointIds(vtkDataSet* mesh, vtkIdType pointId, const PointIdVisitor& visitor);

  vtkStarIterator() = default;

private:
  void operator=(const vtkStarIterator&) = delete;
  vtkStarIterator(const vtkStarIterator&) = delete;

  vtkNew<vtkIdList> CellsAttachedToPt;
  vtkNew<vtkIdList> CellCornerPts;
};
VTK_ABI_NAMESPACE_END

#endif // vtkStarIterator_h
