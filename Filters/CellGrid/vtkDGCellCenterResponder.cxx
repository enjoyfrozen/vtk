// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkDGCellCenterResponder.h"

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

namespace {

void addSourceCenters(vtkDGCell* cell, const vtkDGCell::Source& spec, vtkIdTypeArray* cellIds, vtkDoubleArray* rst)
{
  vtkIdType nn = spec.Connectivity->GetNumberOfTuples();
  vtkIdType off = spec.Offset;
  vtkSMPTools::For(0, nn, [&](vtkIdType beg, vtkIdType end)
    {
      vtkVector3d param;
      for (vtkIdType ii = beg; ii < end; ++ii)
      {
        param = cell->GetParametricCenterOfSide(spec.SideType);
        cellIds->SetValue(ii + off, ii + off);
        rst->SetTuple(ii + off, param.GetData());
      }
    }
  );
}

} // anonymous namespace

vtkStandardNewMacro(vtkDGCellCenterResponder);

void vtkDGCellCenterResponder::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

bool vtkDGCellCenterResponder::Query(
  vtkCellGridCellCenters::Query* request, vtkCellMetadata* cellType, vtkCellGridResponders* caches)
{
  (void)request;
  (void)cellType;
  (void)caches;

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

  // Fetch arrays that define element shapes.
  auto shapeAtt = grid->GetShapeAttribute();
  if (!shapeAtt)
  {
    return false;
  }

  std::string cellTypeName = cellType->GetClassName();
  std::string idsname = cellTypeName + " source cell ids";
  std::string rstname = cellTypeName + " parametric coordinates";
  std::string xyzname = cellTypeName + " center coordinates";
  std::string vcnname = cellTypeName + " center conn";
  vtkIdType nn = dgCell->GetNumberOfCells();
  vtkNew<vtkIdTypeArray> cellIds;
  vtkNew<vtkDoubleArray> rst;
  cellIds->SetNumberOfTuples(nn);
  cellIds->SetName(idsname.c_str());
  rst->SetNumberOfTuples(nn);
  rst->SetName(rstname.c_str());
  static vtkNew<vtkDGInterpolateCalculator> interpolateProto;
  auto rawCalc = interpolateProto->PrepareForGrid(dgCell, shapeAtt);
  auto shapeCalc = vtkDGInterpolateCalculator::SafeDownCast(rawCalc);
  if (!shapeCalc)
  {
    return false;
  }
  addSourceCenters(dgCell, dgCell->GetCellSpec(), cellIds, rst);
  for (const auto& sideSpec : dgCell->GetSideSpecs())
  {
    addSourceCenters(dgCell, sideSpec, cellIds, rst);
  }
  vtkNew<vtkDoubleArray> coords;
  coords->SetNumberOfComponents(3);
  coords->SetNumberOfTuples(nn);
  coords->SetName(xyzname.c_str());
  shapeCalc->Evaluate(cellIds, rst, coords);
  vtkNew<vtkIntArray> vconn;
  vconn->SetNumberOfTuples(nn);
  vconn->SetName(vcnname.c_str());
  vtkSMPTools::For(0, nn, [&vconn](vtkIdType begin, vtkIdType end)
    {
      for (vtkIdType ii = begin; ii < end; ++ii)
      {
        vconn->SetValue(ii, ii);
      }
    });

  auto* verts = request->GetOutput()->AddCellMetadata<vtkDGVert>();
  // TODO: All types of DG cells output vtkDGVert; we need to combine
  //       them all into a single CellSpec object and a single array.
  //       This means a 2- or 3-pass query: count output size with offsets,
  //       allocate output arrays, and populate arrays.
  // TODO: We only support CG shape-attributes in the render responder,
  //       but we should support DG ones, especially for point clouds.
  verts->GetCellSpec().Connectivity = vconn;
  verts->GetCellSpec().SourceShape = vtkDGCell::Shape::Vertex;
  auto* vtxGroup = grid->GetAttributes(vtkStringToken(vcnname));
  vtxGroup->AddArray(vconn);
  vtxGroup->AddArray(cellIds);
  vtxGroup->AddArray(rst);
  vtxGroup->AddArray(coords);

  vtkCellAttribute* vertShape = request->GetOutput()->GetCellAttributeByName("vertex shape");
  if (!vertShape)
  {
    vtkNew<vtkCellAttribute> att;
    att->Initialize("vertex shape", "ℝ³", 3);
    request->GetOutput()->SetShapeAttribute(att);
    vertShape = att;
  }
  vtkCellAttribute::CellTypeInfo vertShapeInfo;
  vertShapeInfo.DOFSharing = vtkStringToken(vcnname);
  vertShapeInfo.FunctionSpace = "constant"_token;
  vertShapeInfo.Basis = "C"_token;
  vertShapeInfo.Order = 0;
  auto& vertShapeArrays = vertShapeInfo.ArraysByRole;
  vertShapeArrays["connectivity"_token] = vconn;
  vertShapeArrays["values"_token] = coords;
  vertShape->SetCellTypeInfo("vtkDGVert"_token, vertShapeInfo);

  return true;
}

VTK_ABI_NAMESPACE_END
