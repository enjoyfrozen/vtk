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

void addSourceCenters(
  vtkDGCell* cell,
  const vtkDGCell::Source& spec,
  vtkIdTypeArray* cellIds,
  vtkDoubleArray* rst,
  vtkIdType& vbegin,
  vtkIdType& vend)
{
  if (spec.Blanked) { return; }

  vtkIdType nn = spec.Connectivity->GetNumberOfTuples();
  vtkIdType off = spec.Offset;
  // Note that vend - vbegin > nn when multiple vtkDGCell::Source instances
  // contribute to the output for a single cell shape. For example, we may
  // output face-, edge-, and vertex-sides of a 3D cell shape. Each will
  // consume a portion of the [vbegin, vend[ range.
  if (spec.SideType < 0)
  {
    // Compute center of (non-blanked) cell
    vtkSMPTools::For(0, nn, [&](vtkIdType beg, vtkIdType end)
      {
        vtkVector3d param;
        for (vtkIdType ii = beg; ii < end; ++ii)
        {
          // param = cell->GetParametricCenterOfSide(spec.SideType);
          param = cell->GetParametricCenterOfSide(spec.SideType);
          cellIds->SetValue(vbegin + ii, ii + off);
          rst->SetTuple(vbegin + ii, param.GetData());
        }
      }
    );
  }
  else
  {
    // Compute center of side of a cell.
    vtkSMPTools::For(0, nn, [&](vtkIdType beg, vtkIdType end)
      {
        vtkVector3d param;
        std::array<vtkTypeUInt64, 2> sideConn;
        for (vtkIdType ii = beg; ii < end; ++ii)
        {
          spec.Connectivity->GetUnsignedTuple(ii, sideConn.data());
          param = cell->GetParametricCenterOfSide(sideConn[1]);
          cellIds->SetValue(vbegin + ii, ii + off);
          rst->SetTuple(vbegin + ii, param.GetData());
        }
      }
    );
  }
  vbegin += nn;
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

  switch (request->GetPass())
  {
  case vtkCellGridCellCenters::Query::PassType::CountOutputs:
    request->AddOutputCenters(dgCell->GetClassName(), "vtkDGVert"_token, dgCell->GetNumberOfCells());
    break;
  case vtkCellGridCellCenters::Query::PassType::AllocateOutputs:
    if (!request->GetOutput()->GetCellsOfType<vtkDGVert>())
    {
      this->AllocateOutputVertices(request);
    }
    // Now fill in the cell-id and parametric-coordinate values in the allocated arrays.
    this->AddCellCenters(request, dgCell);
    break;
  case vtkCellGridCellCenters::Query::PassType::GenerateOutputs:
    // Evaluate attributes at the (cell-id, parametric coordinates) values added in the
    // previous pass:
    this->GenerateOutputVertices(request, dgCell);
    break;
  default:
    vtkErrorMacro("Unknown pass " << request->GetPass());
  }

  return true;
}

void vtkDGCellCenterResponder::AllocateOutputVertices(vtkCellGridCellCenters::Query* request)
{
  // We have one set of vertices for all input DG cell types.
  auto& alloc = request->GetOutputAllocations();
  auto ait = alloc.find("vtkDGVert"_token);
  if (ait == alloc.end())
  {
    return;
  }

  // Sum the counts for each input cell type to get offsets.
  std::multimap<int, vtkStringToken> partialOrder;
  for (auto bit = ait->second.begin(); bit != ait->second.end(); ++bit)
  {
    auto* dgCell = vtkDGCell::SafeDownCast(request->GetInput()->GetCellType(bit->first));
    if (!dgCell) { continue; }

    partialOrder.emplace(dgCell->GetDimension(), bit->first);
  }
  vtkTypeUInt64 offset = 0;
  for (const auto& entry : partialOrder)
  {
    vtkTypeUInt64 nn = ait->second[entry.second];
    std::cout << "  " << entry.second.Data() << ": " << offset << "–" << nn << "\n";
    ait->second[entry.second] = offset;
    offset += nn;
  }

  // Our cumulative sum is now the number of output vertices in total:
  vtkIdType nn = static_cast<vtkIdType>(offset);

  std::string rstname = "center parametric coordinates";
  std::string xyzname = "center coordinates";
  std::string vcnname = "center conn";

  // Create a "connectivity" array of point IDs for all output vertices.
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
  auto dgVert = request->GetOutput()->AddCellMetadata<vtkDGVert>();
  auto* vtxGroup = request->GetOutput()->GetAttributes("vtkDGVert"_token);
  vtxGroup->SetScalars(vconn);
  dgVert->GetCellSpec().Connectivity = vconn;
  dgVert->GetCellSpec().SourceShape = vtkDGCell::Shape::Vertex;
  dgVert->GetCellSpec().Blanked = false;

  vtkNew<vtkIdTypeArray> cellIds;
  vtkNew<vtkDoubleArray> rst;
  cellIds->SetNumberOfTuples(nn);
  cellIds->SetName("source id");
  rst->SetNumberOfComponents(3);
  rst->SetNumberOfTuples(nn);
  rst->SetName(rstname.c_str());
  vtxGroup->AddArray(cellIds);
  vtxGroup->AddArray(rst);

  // TODO: Add CellTypeInfo to each output attribute; allocate arrays as needed.
  for (const auto& inCellAtt : request->GetInput()->GetCellAttributeList())
  {
    vtkCellAttribute::CellTypeInfo info;
    if (inCellAtt == request->GetInput()->GetShapeAttribute())
    {
      // The shape attribute must be "continuous" (i.e. have connectivity)
      // for the sake of the render-responder. No other attributes need this.
      info.DOFSharing = "vtkDGVert"_token;
      info.ArraysByRole["connectivity"_token] = vconn;
    }
    info.FunctionSpace = "constant"_token;
    info.Basis = "C"_token;
    info.Order = 0;
    vtkNew<vtkDoubleArray> values;
    values->SetName(inCellAtt->GetName().Data().c_str());
    values->SetNumberOfComponents(inCellAtt->GetNumberOfComponents());
    values->SetNumberOfTuples(nn);
    vtxGroup->AddArray(values);
    info.ArraysByRole["values"] = values;
    auto* outCellAtt = request->GetOutputAttribute(inCellAtt);
    outCellAtt->SetCellTypeInfo("vtkDGVert"_token, info);
  }
}

void vtkDGCellCenterResponder::AddCellCenters(
  vtkCellGridCellCenters::Query* request, vtkDGCell* cellType)
{
  auto& alloc = request->GetOutputAllocations();
  auto ait = alloc.find("vtkDGVert"_token);
  if (ait == alloc.end())
  {
    return;
  }
  auto cit = ait->second.find(cellType->GetClassName());
  if (cit == ait->second.end())
  {
    // No allocation for \a cellType.
    return;
  }
  auto vertBegin = cit->second;
  auto vertEnd = vertBegin + cellType->GetNumberOfCells();

  auto* vtxGroup = request->GetOutput()->GetAttributes("vtkDGVert"_token);
  auto* cellIds = vtkIdTypeArray::SafeDownCast(vtxGroup->GetArray("source id"));
  auto* rst = vtkDoubleArray::SafeDownCast(vtxGroup->GetArray("center parametric coordinates"));

  addSourceCenters(cellType, cellType->GetCellSpec(), cellIds, rst, vertBegin, vertEnd);
  for (const auto& sideSpec : cellType->GetSideSpecs())
  {
    addSourceCenters(cellType, sideSpec, cellIds, rst, vertBegin, vertEnd);
  }
}

void vtkDGCellCenterResponder::GenerateOutputVertices(vtkCellGridCellCenters::Query* request, vtkDGCell* cellType)
{
  auto& alloc = request->GetOutputAllocations();
  auto ait = alloc.find("vtkDGVert"_token);
  if (ait == alloc.end())
  {
    return;
  }
  auto cit = ait->second.find(cellType->GetClassName());
  if (cit == ait->second.end())
  {
    // No allocation for \a cellType.
    return;
  }

  auto* vtxGroup = request->GetOutput()->GetAttributes("vtkDGVert"_token);
  auto* cellIds = vtkIdTypeArray::SafeDownCast(vtxGroup->GetArray("source id"));
  auto* rst = vtkDoubleArray::SafeDownCast(vtxGroup->GetArray("center parametric coordinates"));
  vtkNew<vtkDGInterpolateCalculator> interpolateProto;
  for (const auto& inCellAtt : request->GetInput()->GetCellAttributeList())
  {
    auto vertBegin = cit->second;
    auto vertEnd = vertBegin + cellType->GetNumberOfCells();

    auto outCellAtt = request->GetOutputAttribute(inCellAtt);
    if (!outCellAtt)
    {
      vtkWarningMacro("No output attribute matching \"" << inCellAtt->GetName().Data() << "\".");
      continue;
    }
    auto outCellTypeInfo = outCellAtt->GetCellTypeInfo("vtkDGVert"_token);
    auto rawCalc = interpolateProto->PrepareForGrid(cellType, inCellAtt);
    auto dgCalc = vtkDGInterpolateCalculator::SafeDownCast(rawCalc);
    vertBegin = cit->second;
    auto* attValues = outCellTypeInfo.GetArrayForRoleAs<vtkDoubleArray>("values"_token);
    int nc = attValues->GetNumberOfComponents();
    vtkNew<vtkDoubleArray> attWindow;
    attWindow->SetNumberOfComponents(nc);
    attWindow->SetArray(attValues->GetPointer(0) + nc * vertBegin, (vertEnd - vertBegin) * nc, /* save */1);
    dgCalc->Evaluate(cellIds, rst, attWindow);
  }

#if 0
  auto* vtxGroup = request->GetOutput()->GetAttributes(vtkStringToken(vcnname));
  vtxGroup->AddArray(vconn);
  vtxGroup->AddArray(cellIds);

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

  vtxGroup->AddArray(rst);
  vtkCellAttribute* vertRST = request->GetOutput()->GetCellAttributeByName("parametric coordinates");
  if (!vertRST)
  {
    vtkNew<vtkCellAttribute> att;
    att->Initialize("parametric coordinates", "ℝ³", 3);
    request->GetOutput()->AddCellAttribute(att);
    vertRST = att;
  }
  vtkCellAttribute::CellTypeInfo vertRSTInfo;
  vertRSTInfo.DOFSharing = vtkStringToken(vcnname);
  vertRSTInfo.FunctionSpace = "constant"_token;
  vertRSTInfo.Basis = "C"_token;
  vertRSTInfo.Order = 0;
  auto& vertRSTArrays = vertRSTInfo.ArraysByRole;
  vertRSTArrays["connectivity"_token] = vconn;
  vertRSTArrays["values"_token] = rst;
  vertRST->SetCellTypeInfo("vtkDGVert"_token, vertRSTInfo);

  // Now iterate through other cell attributes

  return true;
#endif
}

VTK_ABI_NAMESPACE_END
