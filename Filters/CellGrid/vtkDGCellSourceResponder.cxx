// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkDGCellSourceResponder.h"

#include "vtkBoundingBox.h"
#include "vtkCellAttribute.h"
#include "vtkCellGrid.h"
#include "vtkCellGridElevationQuery.h"
#include "vtkDoubleArray.h"
#include "vtkDGInterpolateCalculator.h"
#include "vtkDGVert.h"
#include "vtkDataSetAttributes.h"
#include "vtkFloatArray.h"
#include "vtkIntArray.h"
#include "vtkIdTypeArray.h"
#include "vtkObjectFactory.h"
#include "vtkSMPTools.h"
#include "vtkStringToken.h"
#include "vtkTypeInt64Array.h"
#include "vtkVectorOperators.h"

#include <unordered_set>

VTK_ABI_NAMESPACE_BEGIN

using namespace vtk::literals;

vtkStandardNewMacro(vtkDGCellSourceResponder);

void vtkDGCellSourceResponder::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

bool vtkDGCellSourceResponder::Query(
  vtkCellGridCellSource::Query* request, vtkCellMetadata* cellType, vtkCellGridResponders* caches)
{
  (void)caches;

  vtkStringToken cellTypeToken = cellType->GetClassName();
  vtkStringToken requestedType = request->GetCellType();
  if (requestedType != cellTypeToken)
  {
    // Do not create any cells of this type if not asked.
    return true;
  }

  auto* dgCell = vtkDGCell::SafeDownCast(cellType);
  if (!dgCell)
  {
    return false;
  }

  auto* grid = dgCell->GetCellGrid();
  if (!grid)
  {
    return false;
  }

  auto npts = dgCell->GetNumberOfCorners();

  vtkNew<vtkDoubleArray> coords;
  coords->SetName("coords");
  coords->SetNumberOfComponents(3);
  coords->SetNumberOfTuples(npts);
  for (int ii = 0; ii < npts; ++ii)
  {
    coords->SetTuple(ii, dgCell->GetCornerParameter(ii).data());
  }

  vtkNew<vtkIdTypeArray> conn;
  conn->SetName("connectivity");
  conn->SetNumberOfComponents(npts);
  conn->SetNumberOfTuples(1);
  std::vector<vtkTypeUInt64> connTuple(npts);
  for (int ii = 0; ii < npts; ++ii)
  {
    connTuple[ii] = ii;
  }
  conn->SetUnsignedTuple(0, connTuple.data());

  auto* arrayGroup = grid->GetAttributes(cellTypeToken);
  arrayGroup->SetScalars(conn);
  dgCell->GetCellSpec().Connectivity = conn;

  arrayGroup = grid->GetAttributes("points"_token);
  arrayGroup->SetScalars(coords);

  vtkNew<vtkCellAttribute> shape;
  shape->Initialize("shape"_token, "ℝ³", 3);
  vtkCellAttribute::CellTypeInfo shapeInfo;
  shapeInfo.DOFSharing = "points"_token;
  if (cellTypeToken == "vtkDGVert"_token)
  {
    shapeInfo.FunctionSpace = "constant"_token;
    shapeInfo.Order = 0;
  }
  else
  {
    shapeInfo.FunctionSpace = "HGRAD"_token;
    shapeInfo.Order = 1;
  }
  shapeInfo.Basis = "C"_token;
  shapeInfo.ArraysByRole["connectivity"_token] = conn;
  shapeInfo.ArraysByRole["values"_token] = coords;
  shape->SetCellTypeInfo(cellTypeToken, shapeInfo);
  grid->SetShapeAttribute(shape);

  return true;
}

VTK_ABI_NAMESPACE_END
