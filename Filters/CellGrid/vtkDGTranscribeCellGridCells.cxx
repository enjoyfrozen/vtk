// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkDGTranscribeCellGridCells.h"

#include "vtkBoundingBox.h"
#include "vtkCellArray.h"
#include "vtkCellAttribute.h"
#include "vtkCellGrid.h"
#include "vtkCellGridElevationQuery.h"
#include "vtkDoubleArray.h"
#include "vtkDGInterpolateCalculator.h"
#include "vtkDGVert.h"
#include "vtkDataSetAttributes.h"
#include "vtkFloatArray.h"
#include "vtkIncrementalOctreePointLocator.h"
#include "vtkIntArray.h"
#include "vtkIdTypeArray.h"
#include "vtkObjectFactory.h"
#include "vtkSMPTools.h"
#include "vtkStringToken.h"
#include "vtkTypeInt64Array.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnsignedCharArray.h"
#include "vtkVectorOperators.h"

#include <unordered_set>

VTK_ABI_NAMESPACE_BEGIN

using namespace vtk::literals;

namespace {

int vtkCellTypeForDGShape(vtkDGCell::Shape shape)
{
  switch (shape)
  {
  case vtkDGCell::Shape::Vertex:        return VTK_VERTEX;
  case vtkDGCell::Shape::Edge:          return VTK_LINE;
  case vtkDGCell::Shape::Triangle:      return VTK_TRIANGLE;
  case vtkDGCell::Shape::Quadrilateral: return VTK_QUAD;
  case vtkDGCell::Shape::Tetrahedron:   return VTK_TETRA;
  case vtkDGCell::Shape::Hexahedron:    return VTK_HEXAHEDRON;
  case vtkDGCell::Shape::Wedge:         return VTK_WEDGE;
  case vtkDGCell::Shape::Pyramid:       return VTK_PYRAMID;
  default: break;
  }
  return VTK_EMPTY_CELL;
}

void vtkCellInfoFromDGType(
  vtkCellGridToUnstructuredGrid::Query::OutputAllocation& alloc,
  vtkDGCell* dgCell)
{
  auto shape = dgCell->GetShape();
  alloc.CellType = vtkCellTypeForDGShape(shape);
  alloc.NumberOfCells = 0;
  alloc.NumberOfConnectivityEntries = 0;
  for (int ii = -1; ii < dgCell->GetNumberOfCellSources(); ++ii)
  {
    const auto& source(dgCell->GetCellSource(ii));
    if (source.Blanked) { continue; }

    // Fetch the range of side indices that have the shape corresponding to source.SideType:
    auto sideRange = dgCell->GetSideRangeForType(source.SideType);
    shape = dgCell->GetSideShape(sideRange.first);
    vtkIdType pointsPerSide = vtkDGCell::GetShapeCornerCount(shape);
    vtkIdType numCells = source.Connectivity->GetNumberOfTuples();
    alloc.NumberOfCells += numCells;
    alloc.NumberOfConnectivityEntries += (pointsPerSide + 1) * numCells;
  }
}

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
  if (vend - vbegin != nn)
  {
    vtkGenericWarningMacro("Interval [" << vbegin << ", " << vend << "[ is"
      " size " << (vend - vbegin) << " numcells " << nn << ".");
  }
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
          cellIds->SetValue(vbegin + ii + off, ii + off);
          rst->SetTuple(vbegin + ii + off, param.GetData());
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
          cellIds->SetValue(vbegin + ii + off, ii + off);
          rst->SetTuple(vbegin + ii + off, param.GetData());
        }
      }
    );
  }
  vbegin += nn;
}

} // anonymous namespace

vtkStandardNewMacro(vtkDGTranscribeCellGridCells);

void vtkDGTranscribeCellGridCells::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

bool vtkDGTranscribeCellGridCells::Query(
  TranscribeQuery* request, vtkCellMetadata* cellType, vtkCellGridResponders* caches)
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
  case TranscribeQuery::PassType::CountOutputs:
    {
      auto& alloc(request->GetOutputAllocations());
      vtkCellInfoFromDGType(alloc[dgCell->GetClassName()], dgCell);
    }
    break;
  case TranscribeQuery::PassType::GenerateConnectivity:
    this->GenerateConnectivity(request, dgCell);
    break;
  case TranscribeQuery::PassType::GeneratePointData:
    this->GeneratePointData(request, dgCell);
    break;
  default:
    vtkErrorMacro("Unknown pass " << request->GetPass());
  }

  return true;
}

void vtkDGTranscribeCellGridCells::GenerateConnectivity(
  TranscribeQuery* request, vtkDGCell* cellType)
{
  vtkStringToken cellTypeToken = cellType->GetClassName();
  auto& alloc = request->GetOutputAllocations();
  auto ait = alloc.find(cellTypeToken);
  if (ait == alloc.end())
  {
    return;
  }

  auto* cellArray = request->GetOutput()->GetCells();
  auto* cellTypes = request->GetOutput()->GetCellTypesArray();
  auto* locator = request->GetLocator();
  auto& pointMap = request->GetConnectivityTransform(cellTypeToken);
  auto& pointCounts = request->GetConnectivityCount();
  auto shapeAtt = request->GetInput()->GetShapeAttribute();
  auto shapeInfo = shapeAtt->GetCellTypeInfo(cellTypeToken);
  auto shapePoints = shapeInfo.GetArrayForRoleAs<vtkDataArray>("values"_token);
  auto shapeConn = shapeInfo.GetArrayForRoleAs<vtkDataArray>("connectivity"_token);
  vtkIdType numCellTuples = shapeConn->GetNumberOfTuples();
  // Insert points, add to map, and write output-cell connectivity
  // NB: We currently assume the shape attribute uses a constant (vertices) or HGRAD
  //     function space. If not, we would need to interpolate values here instead of
  //     copying from the shape attribute.
  for (int ii = -1; ii < static_cast<int>(cellType->GetSideSpecs().size()); ++ii)
  {
    auto& source(cellType->GetCellSource(ii));
    if (source.Blanked) { continue; }
    // source.Connectivity is either the connectivity of the cells (when
    // source.SideType < 0) or (cellId, sideIndex) 2-tuples (when
    // source.SideType >= 0). Either way, the number of tuples is the
    // number of cells corresponding to \a source:
    vtkIdType numSideTuples = source.Connectivity->GetNumberOfTuples();
    std::vector<vtkTypeUInt64> inConn;
    std::vector<vtkIdType> outConn;
    inConn.resize(shapeConn->GetNumberOfComponents());
    outConn.reserve(shapeConn->GetNumberOfComponents());
    std::array<double, 3> xx;
    std::array<vtkTypeUInt64, 2> sideTuple; // (cellId, sideIndex)
    if (source.SideType < 0)
    {
      for (vtkIdType cc = 0; cc < numSideTuples; ++cc)
      {
        outConn.clear();
        // source is the CellSpec.
        source.Connectivity->GetUnsignedTuple(cc, inConn.data());
        for (const auto& inPointId : inConn)
        {
          vtkIdType outPointId;
          shapePoints->GetTuple(inPointId, xx.data());
          if (locator->InsertUniquePoint(xx.data(), outPointId) != 0)
          {
            pointMap[static_cast<vtkIdType>(inPointId)] = outPointId;
          }
          ++pointCounts[outPointId];
          outConn.push_back(outPointId);
        }
        cellArray->InsertNextCell(outConn.size(), outConn.data());
        cellTypes->InsertNextValue(ait->second.CellType);
      }
    }
    else
    {
      // source is a SideSpec; fetch the side 2-tuple, then fetch
      // the cell's connectivity, then fetch a subset of the values
      // using the connectivity and side-connectivity.
      auto sideRange = cellType->GetSideRangeForType(source.SideType);
      auto shape = cellType->GetSideShape(sideRange.first);
      vtkIdType pointsPerSide = vtkDGCell::GetShapeCornerCount(shape);
      unsigned char sideShapeVTK = vtkCellTypeForDGShape(shape);
      for (vtkIdType cc = 0; cc < numSideTuples; ++cc)
      {
        outConn.clear();
        source.Connectivity->GetUnsignedTuple(cc, sideTuple.data());
        shapeConn->GetUnsignedTuple(sideTuple[0], inConn.data());
        const auto& sideConn = cellType->GetSideConnectivity(sideTuple[1]);
        for (const auto& sidePointId : sideConn)
        {
          vtkIdType inPointId = inConn[sidePointId];
          vtkIdType outPointId;
          shapePoints->GetTuple(inPointId, xx.data());
          if (locator->InsertUniquePoint(xx.data(), outPointId) != 0)
          {
            pointMap[static_cast<vtkIdType>(inPointId)] = outPointId;
          }
          ++pointCounts[outPointId];
          outConn.push_back(outPointId);
        }
        cellArray->InsertNextCell(outConn.size(), outConn.data());
        cellTypes->InsertNextValue(sideShapeVTK);
      }
    }
  }
}

void vtkDGTranscribeCellGridCells::GeneratePointData(TranscribeQuery* request, vtkDGCell* cellType)
{
  auto& alloc = request->GetOutputAllocations();
  auto ait = alloc.find("vtkDGVert"_token);
  if (ait == alloc.end())
  {
    return;
  }

#if 0
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
#endif

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
