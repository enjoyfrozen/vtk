#include "vtkFiniteElementFieldDistributor.h"

#include "vtkCellArray.h"
#include "vtkCellArrayIterator.h"
#include "vtkCellData.h"
#include "vtkCellIterator.h"
#include "vtkCellType.h"
#include "vtkDataAssembly.h"
#include "vtkDoubleArray.h"
#include "vtkHexahedron.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkLagrangeCurve.h"
#include "vtkLagrangeHexahedron.h"
#include "vtkLagrangeQuadrilateral.h"
#include "vtkLagrangeTetra.h"
#include "vtkLagrangeTriangle.h"
#include "vtkLagrangeWedge.h"
#include "vtkLine.h"
#include "vtkLogger.h"
#include "vtkObjectFactory.h"
#include "vtkPartitionedDataSet.h"
#include "vtkPartitionedDataSetCollection.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkQuad.h"
#include "vtkStringArray.h"
#include "vtkTetra.h"
#include "vtkTriangle.h"
#include "vtkUnstructuredGrid.h"
#include "vtkWedge.h"

#include <iterator>
#include <numeric>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "vtkFEMBasisLagrangeProducts.h"

namespace
{
struct vtkFiniteElementSpec
{
  std::unordered_set<std::string> CGFields, DGFields;
  int CGBasisOrder, DGBasisOrder;
  VTKCellType ReferenceElementType;
};

vtkDataArray* InitializeNewArray(
  vtkDataArray* in, const char* name, const int ncomp, const vtkIdType& ntup)
{
  auto arr = in->NewInstance();
  arr->SetName(name);
  arr->SetNumberOfComponents(ncomp);
  arr->SetNumberOfTuples(ntup);
  arr->Fill(0.);
  return arr;
}

vtkIdType FindCellWithPoints(vtkUnstructuredGrid* mesh, vtkIdList* const ptIds)
{
  std::unordered_map<vtkIdType, int> cellCounts;
  if (mesh->GetCellLinks() == nullptr)
  {
    mesh->BuildLinks();
  }
  for (auto pt = ptIds->begin(); pt != ptIds->end(); ++pt)
  {
    vtkIdType nCells = 0;
    vtkIdType* cellIds = nullptr;
    mesh->GetPointCells(*pt, nCells, cellIds);

    for (vtkIdType i = 0; i < nCells; ++i)
    {
      cellCounts[cellIds[i]] = cellCounts[cellIds[i]] + 1;
    }
  }
  vtkIdType result = -1;
  for (const auto& cellCount : cellCounts)
  {
    if (cellCount.second == ptIds->GetNumberOfIds())
    {
      result = cellCount.first;
      break;
    }
  }
  return result;
}

std::vector<std::string> Split(const std::string& inString, const std::string& delimeter)
{
  std::vector<std::string> subStrings;
  size_t sIdx = 0;
  size_t eIdx = 0;
  while ((eIdx = inString.find(delimeter, sIdx)) < inString.size())
  {
    subStrings.push_back(inString.substr(sIdx, eIdx - sIdx));
    sIdx = eIdx + delimeter.size();
  }
  if (sIdx < inString.size())
  {
    subStrings.push_back(inString.substr(sIdx));
  }
  return subStrings;
}

vtkPartitionedDataSet* GetNamedPartitionedDataSet(
  const std::string& name, vtkPartitionedDataSetCollection* input)
{
  vtkDataAssembly* assembly = input->GetDataAssembly();
  const std::string selector = "//" + vtkDataAssembly::MakeValidNodeName(name.c_str());
  std::vector<int> nodeIds = assembly->SelectNodes({ selector });

  if (nodeIds.empty())
  {
    return nullptr;
  }

  const auto ids = assembly->GetDataSetIndices(nodeIds[0]);
  if (ids.empty())
  {
    return nullptr;
  }
  return input->GetPartitionedDataSet(ids[0]);
}

std::vector<double> GetEdgeAttributes(
  const std::string& name, vtkUnstructuredGrid* edges, vtkGenericCell* cell)
{
  std::vector<double> attrs;
  if (edges == nullptr)
  {
    return attrs;
  }
  vtkDataArray* inArr = edges->GetCellData()->GetArray(name.c_str());
  if (inArr == nullptr)
  {
    return attrs;
  }
  const int& nEdges = cell->GetNumberOfEdges();
  attrs.resize(nEdges, 0);
  for (int i = 0; i < nEdges; ++i)
  {
    vtkCell* line = cell->GetEdge(i);
    vtkIdType edgeId = ::FindCellWithPoints(edges, line->PointIds);
    if (edgeId >= 0)
    {
      inArr->GetTuple(edgeId, &attrs[i]);
    }
  }
  return attrs;
}

std::vector<double> GetFaceAttributes(
  const std::string& name, vtkUnstructuredGrid* faces, vtkGenericCell* cell)
{
  std::vector<double> attrs;
  if (faces == nullptr)
  {
    return attrs;
  }
  vtkDataArray* inArr = faces->GetCellData()->GetArray(name.c_str());
  if (inArr == nullptr)
  {
    return attrs;
  }
  const int& nFaces = cell->GetNumberOfFaces();
  attrs.resize(nFaces, 0);
  for (int i = 0; i < nFaces; ++i)
  {
    vtkCell* face = cell->GetFace(i);
    vtkIdType faceId = ::FindCellWithPoints(faces, face->PointIds);
    if (faceId >= 0)
    {
      inArr->GetTuple(faceId, &attrs[i]);
    }
  }
  return attrs;
}

}

class vtkFiniteElementFieldDistributor::vtkInternals
{
public:
  std::unordered_map<std::string, ::vtkFiniteElementSpec> femSpecs{ { "HCURL",
                                                                      ::vtkFiniteElementSpec() },
    { "HDIV", ::vtkFiniteElementSpec() }, { "HGRAD", ::vtkFiniteElementSpec() } };

  void Allocate(vtkPoints* newPoints, vtkCellArray* newCells, vtkUnsignedCharArray* newCellTypes,
    vtkPointData* hGradFields, vtkPointData* hCurlFields, vtkPointData* hDivFields,
    vtkUnstructuredGrid* elements, vtkUnstructuredGrid* edges, vtkUnstructuredGrid* faces);

  //  takes a continuous mesh and explodes the point set such that each element has
  // its own collection of points unshared by any other element. This also
  // converts the mesh into potentially a higher order mesh if the DG fields require it
  void ExplodeCell(const vtkIdType& cellId, const unsigned char& cellType, vtkPoints* oldPoints,
    vtkPoints* newPoints, vtkCellArray* oldCells, vtkCellArray* newCells,
    vtkUnsignedCharArray* newCellTypes, vtkPointData* oldPd, vtkPointData* newPd,
    vtkCellData* oldCd, vtkPointData* hGradFields);

  // Interpolates edge -> nodal dofs.
  // Interpolates face -> nodal dofs.
  void InterpolateCellToNodes(const vtkIdType& cellId, const int cellType, vtkCellArray* oldCells,
    vtkCellArray* newCells, vtkUnstructuredGrid* edges, vtkUnstructuredGrid* faces,
    vtkPointData* hCurlFields, vtkPointData* hDivFields);

  // clear the three slots of femSpecs.
  void ResetFemSpecs();

private:
  // for convenient access to spec
  inline ::vtkFiniteElementSpec& hCurlSpec() { return this->femSpecs["HCURL"]; }
  inline ::vtkFiniteElementSpec& hDivSpec() { return this->femSpecs["HDIV"]; }
  inline ::vtkFiniteElementSpec& hGradSpec() { return this->femSpecs["HGRAD"]; }

  void AllocateGeometry(vtkPoints* newPoints, const vtkIdType& maxCellSize, vtkCellArray* newCells,
    vtkUnsignedCharArray* newCellTypes, const vtkIdType& numCells);

  void AllocateFields(vtkPointData* hGradFields, vtkPointData* hCurlFields,
    vtkPointData* hDivFields, vtkUnstructuredGrid* elements, vtkUnstructuredGrid* edges,
    vtkUnstructuredGrid* faces, const vtkIdType& maxNumPoints);

  static void ExplodeDGHGradCellCenteredField(vtkCellData* inCd, vtkPointData* outPd,
    const char* name, const vtkIdType& cellId, const vtkIdType& npts, const vtkIdType* pts,
    const std::vector<int>& orderingTransform);

  static std::vector<int> GetIOSSTransformation(const unsigned char& cellType, const int& npts);

  void ExplodeLinearCell(const vtkIdType& cellId, const unsigned char& cellType,
    vtkPoints* oldPoints, vtkPoints* newPoints, vtkCellArray* oldCells, vtkCellArray* newCells,
    vtkUnsignedCharArray* newCellTypes, vtkPointData* oldPd, vtkPointData* newPd,
    vtkCellData* oldCd, vtkPointData* hGradFields);

  void ExplodeHigherOrderCell(const vtkIdType& cellId, const unsigned char& cellType,
    vtkPoints* oldPoints, vtkPoints* newPoints, vtkCellArray* oldCells, vtkCellArray* newCells,
    vtkUnsignedCharArray* newCellTypes, vtkPointData* oldPd, vtkPointData* newPd,
    vtkCellData* oldCd, vtkPointData* hGradFields);

  vtkNew<vtkDoubleArray> weights; // resized to maxCellSize in AllocateGeometry. Use it as you wish.
  // typed vtkCell instances allows easy access to parametric coordinates, edges, faces, ...
  vtkNew<vtkGenericCell> genCell;
  vtkNew<vtkHexahedron> hex;
  vtkNew<vtkLine> line;
  vtkNew<vtkQuad> quad;
  vtkNew<vtkTriangle> tri;
  vtkNew<vtkTetra> tet;
  vtkNew<vtkWedge> wedge;
  vtkNew<vtkLagrangeHexahedron> lagHex;
  vtkNew<vtkLagrangeCurve> lagCurve;
  vtkNew<vtkLagrangeQuadrilateral> lagQuad;
  vtkNew<vtkLagrangeTriangle> lagTri;
  vtkNew<vtkLagrangeTetra> lagTet;
  vtkNew<vtkLagrangeWedge> lagWedge;
};

//----------------------------------------------------------------------------
void vtkFiniteElementFieldDistributor::vtkInternals::ResetFemSpecs()
{
  for (auto& femSpec : this->femSpecs)
  {
    femSpec.second = ::vtkFiniteElementSpec();
  }
}

//----------------------------------------------------------------------------
void vtkFiniteElementFieldDistributor::vtkInternals::AllocateGeometry(vtkPoints* newPoints,
  const vtkIdType& maxCellSize, vtkCellArray* newCells, vtkUnsignedCharArray* newCellTypes,
  const vtkIdType& numCells)
{
  const vtkIdType maxNumPoints = numCells * maxCellSize;
  newCellTypes->SetNumberOfComponents(1);
  newCellTypes->SetNumberOfValues(numCells);
  newCells->AllocateEstimate(numCells, maxCellSize);
  newPoints->Allocate(maxNumPoints);
  this->weights->SetNumberOfValues(maxCellSize);
}

//----------------------------------------------------------------------------
void vtkFiniteElementFieldDistributor::vtkInternals::AllocateFields(vtkPointData* hGradFields,
  vtkPointData* hCurlFields, vtkPointData* hDivFields, vtkUnstructuredGrid* elements,
  vtkUnstructuredGrid* edges, vtkUnstructuredGrid* faces, const vtkIdType& maxNumPoints)
{
  vtkCellData* edgesCd = edges == nullptr ? nullptr : edges->GetCellData();
  vtkCellData* facesCd = faces == nullptr ? nullptr : faces->GetCellData();
  vtkCellData* elemCd = elements->GetCellData();

  // Prepare HGRAD fields
  for (const auto& field : this->hGradSpec().DGFields)
  {
    const char* name = field.c_str();
    vtkDataArray* inArr = elemCd->GetArray(name);
    if (inArr == nullptr)
    {
      continue;
    }
    auto arr = vtk::TakeSmartPointer(::InitializeNewArray(inArr, name, 1, 0));
    arr->Allocate(maxNumPoints);
    hGradFields->AddArray(arr);
  }
  // The new nodal form of HCurl fields will go into point data.
  if (edgesCd != nullptr)
  {
    for (const auto& fields : { this->hCurlSpec().CGFields, this->hCurlSpec().DGFields })
    {
      for (const auto& field : fields)
      {
        const char* name = field.c_str();
        vtkDataArray* inArr = edgesCd->GetArray(name);
        auto arr = vtk::TakeSmartPointer(::InitializeNewArray(inArr, name, 3, 0));
        arr->Allocate(maxNumPoints);
        hCurlFields->AddArray(arr);
      }
    }
  }
  // The new nodal form of HDiv fields will go into point data.
  if (facesCd != nullptr)
  {
    for (const auto& fields : { this->hDivSpec().CGFields, this->hDivSpec().DGFields })
    {
      for (const auto& field : fields)
      {
        const char* name = field.c_str();
        vtkDataArray* inArr = facesCd->GetArray(name);
        auto arr = vtk::TakeSmartPointer(::InitializeNewArray(inArr, name, 3, 0));
        arr->Allocate(maxNumPoints);
        hDivFields->AddArray(arr);
      }
    }
  }
}

//----------------------------------------------------------------------------
void vtkFiniteElementFieldDistributor::vtkInternals::Allocate(vtkPoints* newPoints,
  vtkCellArray* newCells, vtkUnsignedCharArray* newCellTypes, vtkPointData* hGradFields,
  vtkPointData* hCurlFields, vtkPointData* hDivFields, vtkUnstructuredGrid* elements,
  vtkUnstructuredGrid* edges, vtkUnstructuredGrid* faces)
{
  if (elements == nullptr)
  {
    return;
  }

  const vtkIdType& nCells = elements->GetNumberOfCells();
  const vtkIdType& maxCellSize = elements->GetCells()->GetMaxCellSize();
  this->AllocateGeometry(newPoints, maxCellSize, newCells, newCellTypes, nCells);
  this->AllocateFields(
    hGradFields, hCurlFields, hDivFields, elements, edges, faces, nCells * maxCellSize);
}

//----------------------------------------------------------------------------
void vtkFiniteElementFieldDistributor::vtkInternals::ExplodeCell(const vtkIdType& cellId,
  const unsigned char& cellType, vtkPoints* oldPoints, vtkPoints* newPoints, vtkCellArray* oldCells,
  vtkCellArray* newCells, vtkUnsignedCharArray* newCellTypes, vtkPointData* oldPd,
  vtkPointData* newPd, vtkCellData* oldCd, vtkPointData* hGradFields)
{
  bool isCGLinear = this->hGradSpec().CGBasisOrder == 1;
  bool isDGLinear = this->hGradSpec().DGBasisOrder == 1;
  bool isLinear = isCGLinear && isDGLinear;

  // loop over cell connectivity, redo the connectivity s.t each cell is
  // disconnected from other cells and then copy associated points into
  // the point array.
  if (isLinear)
  {
    this->ExplodeLinearCell(cellId, cellType, oldPoints, newPoints, oldCells, newCells,
      newCellTypes, oldPd, newPd, oldCd, hGradFields);
  }
  else
  {
    this->ExplodeHigherOrderCell(cellId, cellType, oldPoints, newPoints, oldCells, newCells,
      newCellTypes, oldPd, newPd, oldCd, hGradFields);
  }

  vtkIdType newNpts = 0;
  const vtkIdType* newPts = nullptr;
  newCells->GetCellAtId(cellId, newNpts, newPts);
  // the field components follow ioss element ordering.
  std::vector<int> ordering = this->GetIOSSTransformation(cellType, newNpts);
  // ioss elements are 1-indexed. transform to 0-indexed lists.
  std::transform(ordering.cbegin(), ordering.cend(), ordering.begin(),
    [](const vtkIdType& val) { return val - 1; });
  // explode HGrad dg fields with the transformation.
  for (const auto& field : this->hGradSpec().DGFields)
  {
    const char* name = field.c_str();
    this->ExplodeDGHGradCellCenteredField(
      oldCd, hGradFields, name, cellId, newNpts, newPts, ordering);
  }
}

//----------------------------------------------------------------------------
void vtkFiniteElementFieldDistributor::vtkInternals::ExplodeLinearCell(const vtkIdType& cellId,
  const unsigned char& cellType, vtkPoints* oldPoints, vtkPoints* newPoints, vtkCellArray* oldCells,
  vtkCellArray* newCells, vtkUnsignedCharArray* newCellTypes, vtkPointData* oldPd,
  vtkPointData* newPd, vtkCellData* oldCd, vtkPointData* hGradFields)
{
  vtkIdType newNpts = 0, oldNpts = 0, ind = newPoints->GetNumberOfPoints();
  const vtkIdType* oldPts = nullptr;
  double coord[3] = {};

  oldCells->GetCellAtId(cellId, oldNpts, oldPts);
  newCellTypes->SetValue(cellId, cellType);
  newCells->InsertNextCell(oldNpts);

  for (vtkIdType i = 0; i < oldNpts; ++i, ++ind)
  {
    const auto& oldId = oldPts[i];
    oldPoints->GetPoint(oldId, coord);
    newPoints->InsertPoint(ind, coord);
    newCells->InsertCellPoint(ind);
    // copy over the non-dg fields from old -> new point data
    newPd->CopyData(oldPd, oldId, ind);
  }
}

//----------------------------------------------------------------------------
void vtkFiniteElementFieldDistributor::vtkInternals::ExplodeHigherOrderCell(const vtkIdType& cellId,
  const unsigned char& cellType, vtkPoints* oldPoints, vtkPoints* newPoints, vtkCellArray* oldCells,
  vtkCellArray* newCells, vtkUnsignedCharArray* newCellTypes, vtkPointData* oldPd,
  vtkPointData* newPd, vtkCellData* oldCd, vtkPointData* hGradFields)
{
  vtkNonLinearCell* nonLinCell = nullptr;
  vtkCell* linearCell = nullptr;
  vtkIdType newNpts = 0, oldNpts = 0, ind = newPoints->GetNumberOfPoints();
  vtkNew<vtkIdList> oldPtIds;
  double* pCoords = nullptr;
  double coord[3] = {};
  int subId = 0;

  oldCells->GetCellAtId(cellId, oldPtIds);

  // Determine the order from no. of components in HGrad DG field arrays.
  std::unordered_set<int> nCompsSet;
  for (const auto& field : this->hGradSpec().DGFields)
  {
    const char* name = field.c_str();
    vtkDataArray* arr = oldCd->GetArray(name);
    if (arr != nullptr)
    {
      const int& nComps = arr->GetNumberOfComponents();
      nCompsSet.insert(nComps);
    }
  }
  if (nCompsSet.size() != 1)
  {
    vtkLog(
      WARNING, << "Invalid no. of components for HGrad DG fields. Cannot determine order of cell "
               << cellId);
    return;
  }

  const int nComps = *(nCompsSet.begin());
  switch (cellType)
  {
    case VTK_LINE:
      switch (nComps)
      {
        case 3:
        case 4:
          if (oldNpts != nComps)
          {
            // bump to VTK_LAGRANGE_CURVE order 2
            newNpts = nComps;
            nonLinCell = this->lagCurve;
            linearCell = this->line;
          }
          break;
        default:
          vtkLog(WARNING, << "Unsupported no. of components in HGRAD field for cell - VTK_LINE."
                          << "Supported: One of 3, 4 "
                          << "Got: " << nComps);
          break;
      }
      break;
    case VTK_TRIANGLE:
      switch (nComps)
      {
        case 6:
        case 10:
          if (oldNpts != nComps)
          {
            // bump to VTK_LAGRANGE_TRIANGLE order 2
            newNpts = nComps;
            nonLinCell = this->lagTri;
            linearCell = this->tri;
          }
          break;
        default:
          vtkLog(WARNING, << "Unsupported no. of components in HGRAD field for cell - VTK_TRIANGLE."
                          << "Supported: One of 6, 10"
                          << "Got: " << nComps);
          break;
      }
      break;
    case VTK_QUAD:
      switch (nComps)
      {
        case 9:
        case 16:
          if (oldNpts != nComps)
          {
            // bump to VTK_LAGRANGE_QUADRILATERAL
            newNpts = nComps;
            this->lagQuad->SetUniformOrderFromNumPoints(newNpts);
            nonLinCell = this->lagQuad;
            linearCell = this->quad;
          }
          break;
        default:
          vtkLog(WARNING, << "Unsupported no. of components in HGRAD field for cell - VTK_QUAD."
                          << "Supported: One of 9, 16 "
                          << "Got: " << nComps);
          break;
      }
      break;
    case VTK_TETRA:
      switch (nComps)
      {
        case 10:
        case 11:
        case 15:
          if (oldNpts != nComps)
          {
            // bump to VTK_LAGRANGE_TETRAHEDRON
            newNpts = nComps;
            nonLinCell = this->lagTet;
            linearCell = this->tet;
          }
          break;
        default:
          vtkLog(WARNING, << "Unsupported no. of components in HGRAD field for cell - VTK_TETRA."
                          << "Supported: One of 10, 11, 15 "
                          << "Got: " << nComps);
          break;
      }
      break;
    case VTK_PYRAMID:
      // vtk does not have vtkHigherOrderPyramid.
      switch (nComps)
      {
        case 13:
        case 14:
        case 19:
        default:
          vtkLog(WARNING, << "Unsupported no. of components in HGRAD field for cell - VTK_PYRAMID."
                          << "Supported: None "
                          << "Got: " << nComps);
          break;
      }
      break;
    case VTK_WEDGE:
      switch (nComps)
      {
        case 15:
        case 18:
        case 21:
          if (oldNpts != nComps)
          {
            // bump to VTK_LAGRANGE_WEDGE
            newNpts = nComps;
            this->lagWedge->SetUniformOrderFromNumPoints(newNpts);
            nonLinCell = this->lagWedge;
            linearCell = this->wedge;
          }
          break;
        default:
          vtkLog(WARNING, << "Unsupported no. of components in HGRAD field for cell - VTK_WEDGE."
                          << "Supported: 15, 18, 21 "
                          << "Got: " << nComps);
          break;
      }
      break;
    case VTK_HEXAHEDRON:
      switch (nComps)
      {
        case 20:
        case 27:
          if (oldNpts != nComps)
          {
            // bump to VTK_LAGRANGE_HEXAHEDRON
            newNpts = nComps;
            this->lagHex->SetUniformOrderFromNumPoints(newNpts);
            nonLinCell = this->lagHex;
            linearCell = this->hex;
          }
          break;
        default:
          vtkLog(
            WARNING, << "Unsupported no. of components in HGRAD field for cell - VTK_HEXAHEDRON."
                     << "Supported: 20, 27 "
                     << "Got: " << nComps);
          break;
      }
      break;
    default:
      break;
  }

  if (linearCell != nullptr && nonLinCell != nullptr)
  {
    const vtkIdType* oldPts = oldPtIds->GetPointer(0);
    const vtkIdType& oldNpts = oldPtIds->GetNumberOfIds();
    linearCell->Initialize(oldNpts, oldPts, oldPoints);
    newCells->InsertNextCell(newNpts);
    newCellTypes->SetValue(cellId, nonLinCell->GetCellType());

    // insert points on corners
    for (unsigned short i = 0; i < oldNpts; ++i, ++ind)
    {
      const auto& oldId = oldPts[i];
      oldPoints->GetPoint(oldId, coord);
      newPoints->InsertPoint(ind, coord);
      newCells->InsertCellPoint(ind);
      // copy over the non-dg fields from old -> new point data
      newPd->CopyData(oldPd, oldId, ind);
    }

    // add points at mid-edge, mid-face locations or at volume center.
    nonLinCell->Points->SetNumberOfPoints(newNpts);
    nonLinCell->PointIds->SetNumberOfIds(newNpts);
    this->weights->FillValue(0.0);
    pCoords = nonLinCell->GetParametricCoords();
    for (unsigned short i = oldNpts; i < newNpts; ++i, ++ind)
    {
      linearCell->EvaluateLocation(subId, &pCoords[3 * i], coord, this->weights->GetPointer(0));
      newPoints->InsertPoint(ind, coord);
      newCells->InsertCellPoint(ind);
      // interpolate the non-dg fields from old -> new point data
      newPd->InterpolatePoint(oldPd, ind, oldPtIds, this->weights->GetPointer(0));
    }
  }
}

//----------------------------------------------------------------------------
void vtkFiniteElementFieldDistributor::vtkInternals::ExplodeDGHGradCellCenteredField(
  vtkCellData* inCd, vtkPointData* outPd, const char* name, const vtkIdType& cellId,
  const vtkIdType& npts, const vtkIdType* pts, const std::vector<int>& orderingTransform)
{
  vtkDataArray* const inArr = inCd->GetArray(name);
  vtkDataArray* const outArr = outPd->GetArray(name);
  if (inArr == nullptr || outArr == nullptr)
  {
    vtkLog(WARNING, << "Invalid HGRAD DG field data. Cannot find array : " << name);
    return;
  }

  if (inArr->GetNumberOfComponents() == npts)
  {
    if (orderingTransform.size() == npts)
    {
      for (vtkIdType i = 0; i < npts; ++i)
      {
        double value =
          inArr->GetComponent(cellId, orderingTransform[i]); // get the transformed i'th component
        outArr->InsertComponent(pts[i], 0, value);
      }
    }
    else
    { // fallback to naïve ordering
      for (vtkIdType i = 0; i < npts; ++i)
      {
        double value = inArr->GetComponent(cellId, i);
        outArr->InsertComponent(pts[i], 0, value);
      }
    }
  }
  else
  {
    vtkLog(WARNING, << "HGRAD field(" << name << ") component mismatch. CellSize(" << npts
                    << ") != nComps(" << inArr->GetNumberOfComponents() << ")");
  }
}

//----------------------------------------------------------------------------
std::vector<int> vtkFiniteElementFieldDistributor::vtkInternals::GetIOSSTransformation(
  const unsigned char& cellType, const int& npts)
{
  std::vector<int> result;
  switch (cellType)
  {
    case VTK_LINE:
      switch (npts)
      {
        case 2:
        case 3:
        case 4:
          result.resize(npts, 0);
          std::iota(result.begin(), result.end(), 1);
          break;
        default:
          vtkLog(WARNING, << "Unsupported no. of points for cell - VTK_LINE."
                          << "Supported: One of 2, 3, 4 "
                          << "Got: " << npts);
          break;
      }
      break;
    case VTK_TRIANGLE:
      switch (npts)
      {
        case 3:
        case 6:
        case 10:
          result.resize(npts, 0);
          std::iota(result.begin(), result.end(), 1);
          break;
        default:
          vtkLog(WARNING, << "Unsupported no. of points for cell - VTK_TRIANGLE."
                          << "Supported: One of 3, 6, 10"
                          << "Got: " << npts);
          break;
      }
      break;
    case VTK_QUAD:
      switch (npts)
      {
        case 4:
        case 9:
        case 16:
          result.resize(npts, 0);
          std::iota(result.begin(), result.end(), 1);
          break;
        default:
          vtkLog(WARNING, << "Unsupported no. of points for cell - VTK_QUAD."
                          << "Supported: One of 4, 9, 16 "
                          << "Got: " << npts);
          break;
      }
      break;
    case VTK_TETRA:
      switch (npts)
      {
        case 4:
        case 10:
        case 11:
        case 15:
          result.resize(npts, 0);
          std::iota(result.begin(), result.end(), 1);
          break;
        default:
          vtkLog(WARNING, << "Unsupported no. of points for cell - VTK_TETRA."
                          << "Supported: One of 4, 10, 11, 15 "
                          << "Got: " << npts);
          break;
      }
      break;
    case VTK_PYRAMID:
      // vtk does not have vtkHigherOrderPyramid.
      switch (npts)
      {
        case 5:
        case 13:
        case 14:
        case 19:
        default:
          vtkLog(WARNING, << "Unsupported no. of points for cell - VTK_PYRAMID."
                          << "Supported: None "
                          << "Got: " << npts);
          break;
      }
      break;
    case VTK_WEDGE:
      switch (npts)
      {
        case 6:
          result = { 4, 5, 6, 1, 2, 3 };
          break;
        case 15:
          // clang-format off
          result = {
            4, 5, 6, 1, 2, 3,
            13, 14, 15,
            7, 8, 9,
            10, 11, 12
          };
          // clang-format on
          break;
        case 18:
          // clang-format off
          result = {
            /* 2 triangles */
            4, 5, 6, 1, 2, 3,

            /* edge centers */
            13, 14, 15,
            7, 8, 9,
            10, 11, 12,

            /* quad-centers */
            16, 17, 18
          };
          // clang-format on
          break;
        case 21:
          result.resize(npts, 0);
          std::iota(result.begin(), result.end(), 1);
          break;
        default:
          vtkLog(WARNING, << "Unsupported no. of points for cell - VTK_WEDGE."
                          << "Supported: 15, 18, 21 "
                          << "Got: " << npts);
          break;
      }
      break;
    case VTK_HEXAHEDRON:
      switch (npts)
      {
        case 8:
          result.resize(npts, 0);
          std::iota(result.begin(), result.end(), 1);
          break;
        case 20:
          // clang-format off
          result = {
            /* 8 corners */
            1, 2, 3, 4,
            5, 6, 7, 8,

            /* 12 mid-edge nodes */
            9, 10, 11, 12,
            17, 18, 19, 20,
            13, 14, 15, 16
          };
          // clang-format on
          break;
        case 27:
          // clang-format off
          result = {
            /* 8 corners */
            1, 2, 3, 4,
            5, 6, 7, 8,

            /* 12 mid-edge nodes */
            9, 10, 11, 12,
            17, 18, 19, 20,
            13, 14, 15, 16,

            /* 6 mid-face nodes */
            24, 25, 26, 27, 22, 23,

            /* mid-volume node*/
            21
          };
          // clang-format on
          break;
        default:
          vtkLog(WARNING, << "Unsupported no. of points for cell - VTK_HEXAHEDRON."
                          << "Supported: 8, 20, 27 "
                          << "Got: " << npts);
          break;
      }
      break;
    default:
      break;
  }
  return result;
}

//----------------------------------------------------------------------------
void vtkFiniteElementFieldDistributor::vtkInternals::InterpolateCellToNodes(const vtkIdType& cellId,
  const int cellType, vtkCellArray* oldCells, vtkCellArray* newCells, vtkUnstructuredGrid* edges,
  vtkUnstructuredGrid* faces, vtkPointData* hCurlFields, vtkPointData* hDivFields)
{
  const vtkIdType *oldPts = nullptr, *newPts = nullptr;
  vtkIdType oldNpts = 0, newNpts = 0;
  this->genCell->SetCellType(cellType);
  oldCells->GetCellAtId(cellId, oldNpts, oldPts);
  newCells->GetCellAtId(cellId, newNpts, newPts);
  this->genCell->Points->SetNumberOfPoints(oldNpts);
  this->genCell->PointIds->SetNumberOfIds(oldNpts);
  for (vtkIdType i = 0; i < oldNpts; ++i)
  {
    this->genCell->PointIds->SetId(i, oldPts[i]);
  }

  switch (cellType)
  {
    case VTK_HEXAHEDRON:
      for (const auto& fieldName : this->hCurlSpec().CGFields)
      {
        std::vector<double> edgeCvs = ::GetEdgeAttributes(fieldName, edges, this->genCell);
        if (edgeCvs.empty())
        {
          continue;
        }
        vtkDataArray* outArr = hCurlFields->GetArray(fieldName.c_str());
        switch (newNpts)
        {
          case 8:
            for (int j = 0; j < newNpts; ++j)
            {
              const vtkIdType& ptId = newPts[j];
              std::vector<double> value(3, 0);
              // interpolate field from edge -> nodal dof
              for (int basisComp = 0; basisComp < 3; ++basisComp)
              {
                for (int i = 0; i < 12; ++i)
                {
                  const int& ii = vblp::hcurl::hex::vtk2IntrepidEdgeMap[i];
                  value[basisComp] += vblp::hcurl::hex::i1c1_8[basisComp][j][ii] * edgeCvs[i];
                } // for i'th edge.
              }   // for every component of vector basis function.
              // save new values.
              outArr->InsertTuple(ptId, value.data());
            }
            break;
          case 20:
            for (int j = 0; j < newNpts; ++j)
            {
              const vtkIdType& ptId = newPts[j];
              std::vector<double> value(3, 0);
              // interpolate field from edge -> nodal dof
              for (int basisComp = 0; basisComp < 3; ++basisComp)
              {
                for (int i = 0; i < 12; ++i)
                {
                  const int& ii = vblp::hcurl::hex::vtk2IntrepidEdgeMap[i];
                  value[basisComp] += vblp::hcurl::hex::i1c2_20[basisComp][j][ii] * edgeCvs[i];
                } // for i'th edge.
              }   // for every component of vector basis function.
              // save new values.
              outArr->InsertTuple(ptId, value.data());
            }
            break;
          case 27:
            for (int j = 0; j < newNpts; ++j)
            {
              const vtkIdType& ptId = newPts[j];
              std::vector<double> value(3, 0);
              // interpolate field from edge -> nodal dof
              for (int basisComp = 0; basisComp < 3; ++basisComp)
              {
                for (int i = 0; i < 12; ++i)
                {
                  const int& ii = vblp::hcurl::hex::vtk2IntrepidEdgeMap[i];
                  value[basisComp] += vblp::hcurl::hex::i1c2_27[basisComp][j][ii] * edgeCvs[i];
                } // for i'th edge.
              }   // for every component of vector basis function.
              // save new values.
              outArr->InsertTuple(ptId, value.data());
            }
            break;
          default:
            break;
        }
      }
      for (const auto& fieldName : this->hDivSpec().CGFields)
      {
        std::vector<double> faceCvs = ::GetFaceAttributes(fieldName, faces, this->genCell);
        if (faceCvs.empty())
        {
          continue;
        }
        vtkDataArray* outArr = hDivFields->GetArray(fieldName.c_str());
        switch (newNpts)
        {
          case 8:
            for (int j = 0; j < newNpts; ++j)
            {
              const vtkIdType& ptId = newPts[j];
              std::vector<double> value(3, 0);
              // interpolate field from face -> nodal dof
              for (int basisComp = 0; basisComp < 3; ++basisComp)
              {
                for (int i = 0; i < 6; ++i)
                {
                  const int& ii = vblp::hdiv::hex::vtk2IntrepidFaceMap[i];
                  value[basisComp] += vblp::hdiv::hex::i1c1_8[basisComp][j][ii] * faceCvs[i];
                } // for i'th face.
              }   // for every component of vector basis function.
              // save new values.
              outArr->InsertTuple(ptId, value.data());
            }
            break;
          case 20:
            for (int j = 0; j < newNpts; ++j)
            {
              const vtkIdType& ptId = newPts[j];
              std::vector<double> value(3, 0);
              // interpolate field from face -> nodal dof
              for (int basisComp = 0; basisComp < 3; ++basisComp)
              {
                for (int i = 0; i < 6; ++i)
                {
                  const int& ii = vblp::hdiv::hex::vtk2IntrepidFaceMap[i];
                  value[basisComp] += vblp::hdiv::hex::i1c2_20[basisComp][j][ii] * faceCvs[i];
                } // for i'th face.
              }   // for every component of vector basis function.
              // save new values.
              outArr->InsertTuple(ptId, value.data());
            }
            break;
          case 27:
            for (int j = 0; j < newNpts; ++j)
            {
              const vtkIdType& ptId = newPts[j];
              std::vector<double> value(3, 0);
              // interpolate field from face -> nodal dof
              for (int basisComp = 0; basisComp < 3; ++basisComp)
              {
                for (int i = 0; i < 6; ++i)
                {
                  const int& ii = vblp::hdiv::hex::vtk2IntrepidFaceMap[i];
                  value[basisComp] += vblp::hdiv::hex::i1c2_27[basisComp][j][ii] * faceCvs[i];
                } // for i'th face.
              }   // for every component of vector basis function.
              // save new values.
              outArr->InsertTuple(ptId, value.data());
            }
            break;
          default:
            break;
        }
      }
      break;
    case VTK_QUAD:
      for (const auto& fieldName : this->hCurlSpec().CGFields)
      {
        std::vector<double> edgeCvs = ::GetEdgeAttributes(fieldName, edges, this->genCell);
        if (edgeCvs.empty())
        {
          continue;
        }
        vtkDataArray* outArr = hCurlFields->GetArray(fieldName.c_str());
        switch (newNpts)
        {
          case 4:
            for (int j = 0; j < newNpts; ++j)
            {
              const vtkIdType& ptId = newPts[j];
              std::vector<double> value(3, 0);
              // interpolate field from edge -> nodal dof
              for (int basisComp = 0; basisComp < 3; ++basisComp)
              {
                for (int i = 0; i < 4; ++i)
                {
                  value[basisComp] += vblp::hcurl::quad::i1c1_4[basisComp][j][i] * edgeCvs[i];
                } // for i'th edge.
              }   // for every component of vector basis function.
              // save new values.
              outArr->InsertTuple(ptId, value.data());
            }
            break;
          case 8:
            for (int j = 0; j < newNpts; ++j)
            {
              const vtkIdType& ptId = newPts[j];
              std::vector<double> value(3, 0);
              // interpolate field from edge -> nodal dof
              for (int basisComp = 0; basisComp < 3; ++basisComp)
              {
                for (int i = 0; i < 4; ++i)
                {
                  value[basisComp] += vblp::hcurl::quad::i1c2_8[basisComp][j][i] * edgeCvs[i];
                } // for i'th edge.
              }   // for every component of vector basis function.
              // save new values.
              outArr->InsertTuple(ptId, value.data());
            }
            break;
          case 9:
            for (int j = 0; j < newNpts; ++j)
            {
              const vtkIdType& ptId = newPts[j];
              std::vector<double> value(3, 0);
              // interpolate field from edge -> nodal dof
              for (int basisComp = 0; basisComp < 3; ++basisComp)
              {
                for (int i = 0; i < 4; ++i)
                {
                  value[basisComp] += vblp::hcurl::quad::i1c2_9[basisComp][j][i] * edgeCvs[i];
                } // for i'th edge.
              }   // for every component of vector basis function.
              // save new values.
              outArr->InsertTuple(ptId, value.data());
            }
            break;
          default:
            break;
        }
      }
      for (const auto& fieldName : this->hDivSpec().CGFields)
      {
        std::vector<double> edgeCvs = ::GetEdgeAttributes(fieldName, edges, this->genCell);
        if (edgeCvs.empty())
        {
          continue;
        }
        vtkDataArray* outArr = hCurlFields->GetArray(fieldName.c_str());
        switch (newNpts)
        {
          case 4:
            for (int j = 0; j < newNpts; ++j)
            {
              const vtkIdType& ptId = newPts[j];
              std::vector<double> value(3, 0);
              // interpolate field from edge -> nodal dof
              for (int basisComp = 0; basisComp < 3; ++basisComp)
              {
                for (int i = 0; i < 4; ++i)
                {
                  value[basisComp] += vblp::hdiv::quad::i1c1_4[basisComp][j][i] * edgeCvs[i];
                } // for i'th edge.
              }   // for every component of vector basis function.
              // save new values.
              outArr->InsertTuple(ptId, value.data());
            }
            break;
          case 8:
          case 9:
          default:
            break;
        }
      }
      break;
    case VTK_TRIANGLE:
      for (const auto& fieldName : this->hCurlSpec().CGFields)
      {
        std::vector<double> edgeCvs = ::GetEdgeAttributes(fieldName, edges, this->genCell);
        if (edgeCvs.empty())
        {
          continue;
        }
        vtkDataArray* outArr = hCurlFields->GetArray(fieldName.c_str());
        switch (newNpts)
        {
          case 3:
            for (int j = 0; j < newNpts; ++j)
            {
              const vtkIdType& ptId = newPts[j];
              std::vector<double> value(3, 0);
              // interpolate field from edge -> nodal dof
              for (int basisComp = 0; basisComp < 3; ++basisComp)
              {
                for (int i = 0; i < 3; ++i)
                {
                  value[basisComp] += vblp::hcurl::tri::i1c1_3[basisComp][j][i] * edgeCvs[i];
                } // for i'th edge.
              }   // for every component of vector basis function.
              // save new values.
              outArr->InsertTuple(ptId, value.data());
            }
            break;
          default:
            break;
        }
      }
      for (const auto& fieldName : this->hDivSpec().CGFields)
      {
        std::vector<double> edgeCvs = ::GetEdgeAttributes(fieldName, edges, this->genCell);
        if (edgeCvs.empty())
        {
          continue;
        }
        vtkDataArray* outArr = hCurlFields->GetArray(fieldName.c_str());
        switch (newNpts)
        {
          case 3:
            for (int j = 0; j < newNpts; ++j)
            {
              const vtkIdType& ptId = newPts[j];
              std::vector<double> value(3, 0);
              // interpolate field from edge -> nodal dof
              for (int basisComp = 0; basisComp < 3; ++basisComp)
              {
                for (int i = 0; i < 3; ++i)
                {
                  value[basisComp] += vblp::hdiv::tri::i1c1_3[basisComp][j][i] * edgeCvs[i];
                } // for i'th edge.
              }   // for every component of vector basis function.
              // save new values.
              outArr->InsertTuple(ptId, value.data());
            }
            break;
          default:
            break;
        }
      }
      break;
    default:
      break;
  }
}

vtkStandardNewMacro(vtkFiniteElementFieldDistributor);

//----------------------------------------------------------------------------
vtkFiniteElementFieldDistributor::vtkFiniteElementFieldDistributor()
  : Internals(new vtkFiniteElementFieldDistributor::vtkInternals())
{
}

//----------------------------------------------------------------------------
vtkFiniteElementFieldDistributor::~vtkFiniteElementFieldDistributor() = default;

//----------------------------------------------------------------------------
void vtkFiniteElementFieldDistributor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
int vtkFiniteElementFieldDistributor::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkPartitionedDataSetCollection* input = vtkPartitionedDataSetCollection::GetData(inputVector[0]);
  vtkPartitionedDataSetCollection* output = vtkPartitionedDataSetCollection::GetData(outputVector);

  // Look for special string array containing information records.
  vtkFieldData* fd = input->GetFieldData();
  vtkStringArray* infoRecords =
    vtkStringArray::SafeDownCast(fd->GetAbstractArray("Information Records"));
  if (infoRecords == nullptr)
  {
    vtkErrorMacro(<< "Failed to find a string array 'Information Records'");
    return 0;
  }

  // Parse the information records.
  std::string elementBlockName, edgeBlockName, faceBlockName;
  std::unordered_map<std::string, std::unordered_set<std::string>> block2BasisTypes;
  this->Internals->ResetFemSpecs();
  for (vtkIdType i = 0; i < infoRecords->GetNumberOfValues(); ++i)
  {
    const auto& record = infoRecords->GetValue(i);
    ::vtkFiniteElementSpec* femSpec = nullptr;

    const std::vector<std::string> data = ::Split(record, "::");
    // Examples:
    // "HDIV::eblock-0_0_0::CG::basis::Intrepid2_HDIV_HEX_I1_FEM"
    //    0       1         2     3              4
    //
    // "HGRAD::eblock-0_0::DG::basis::Intrepid2_HGRAD_QUAD_C2_FEM"
    //    0       1        2     3              4
    //
    // "HCURL::eblock-0_0_0::CG::basis::Intrepid2_HCURL_HEX_I1_FEM"
    //    0       1          2     3              4
    //
    // "HCURL::eblock-0_0_0::CG::field::E_Field"
    //    0       1          2     3      4
    if (data.size() < 5)
    {
      continue;
    }
    // within this context, an entity is either a basis or a field.
    const std::string& basisType = data[0];
    const std::string& blockName = data[1];
    const std::string& galerkinType = data[2];
    const std::string& entityType = data[3];
    const std::string& entityName = data[4];
    // Look for valid FEM element callouts.
    if (!(basisType == "HCURL" || basisType == "HDIV" || basisType == "HGRAD"))
    {
      continue;
    }

    femSpec = &(this->Internals->femSpecs[basisType]);
    block2BasisTypes[blockName].insert(basisType);

    if (entityType == "basis")
    {
      const auto& intrepidName = entityName;
      const std::vector<std::string> nameParts = ::Split(intrepidName, "_");
      // Examples:
      // "Intrepid2_HCURL_HEX_I1_FEM"
      //      0       1    2  3   4
      const auto& elementName = nameParts[2];
      if (elementName == "HEX")
      {
        femSpec->ReferenceElementType = VTK_HEXAHEDRON;
      }
      else if (elementName == "LINE")
      {
        femSpec->ReferenceElementType = VTK_LINE;
      }
      else if (elementName == "PYR")
      {
        femSpec->ReferenceElementType = VTK_PYRAMID;
      }
      else if (elementName == "QUAD")
      {
        femSpec->ReferenceElementType = VTK_QUAD;
      }
      else if (elementName == "TET")
      {
        femSpec->ReferenceElementType = VTK_TETRA;
      }
      else if (elementName == "TRI")
      {
        femSpec->ReferenceElementType = VTK_TRIANGLE;
      }
      else if (elementName == "WEDGE")
      {
        femSpec->ReferenceElementType = VTK_WEDGE;
      }
      const int currentBasisOrder = nameParts[3][1] - '0';
      if (galerkinType == "CG")
      {
        femSpec->CGBasisOrder = currentBasisOrder;
      }
      else if (galerkinType == "DG")
      {
        femSpec->DGBasisOrder = currentBasisOrder;
      }
    }
    else if (entityType == "field" && femSpec != nullptr)
    {
      // these fields will be attached to a basis.
      if (galerkinType == "CG")
      {
        femSpec->CGFields.insert(entityName);
      }
      else if (galerkinType == "DG")
      {
        femSpec->DGFields.insert(entityName);
      }
    }
  }

  for (auto& blockBasis : block2BasisTypes)
  {
    const auto& blockName = blockBasis.first;
    const auto& basisTypes = blockBasis.second;

    if (basisTypes.count("HGRAD"))
    {
      elementBlockName = blockName;
    }
    else if (basisTypes.count("HCURL") && edgeBlockName.empty())
    {
      edgeBlockName = blockName;
    }
    else if (basisTypes.count("HDIV") && faceBlockName.empty())
    {
      faceBlockName = blockName;
    }
  }

  vtkPartitionedDataSet *elementsPds = nullptr, *edgesPds = nullptr, *facesPds = nullptr;
  // Find an element block.
  if (!elementBlockName.empty())
  {
    elementsPds = ::GetNamedPartitionedDataSet(elementBlockName, input);
  }
  // Find an edge block.
  if (!edgeBlockName.empty())
  {
    edgesPds = ::GetNamedPartitionedDataSet(edgeBlockName, input);
  }
  // Find a face block.
  if (!faceBlockName.empty())
  {
    facesPds = ::GetNamedPartitionedDataSet(faceBlockName, input);
  }

  if (elementsPds == nullptr)
  {
    vtkErrorMacro(<< "Failed to find an elements block!");
    return 0;
  }
  // Sanity check no. of partitions for elements, (and edges, faces if they exist)
  else if (edgesPds != nullptr)
  {
    if (edgesPds->GetNumberOfPartitions() != elementsPds->GetNumberOfPartitions())
    {
      vtkErrorMacro(<< "No. of elements partitions != No. of edges partitions");
      return 0;
    }
  }
  else if (facesPds != nullptr)
  {
    if (facesPds->GetNumberOfPartitions() != elementsPds->GetNumberOfPartitions())
    {
      vtkErrorMacro(<< "No. of elements partitions != No. of faces partitions");
      return 0;
    }
  }

  const unsigned int numParts = elementsPds->GetNumberOfPartitions();
  // TODO: mpi-fy this thing..
  for (unsigned int partIdx = 0; partIdx < numParts; ++partIdx)
  {
    vtkUnstructuredGrid* elements =
      vtkUnstructuredGrid::SafeDownCast(elementsPds->GetPartition(partIdx));
    vtkUnstructuredGrid* edges = edgesPds != nullptr
      ? vtkUnstructuredGrid::SafeDownCast(edgesPds->GetPartition(partIdx))
      : nullptr;
    vtkUnstructuredGrid* faces = facesPds != nullptr
      ? vtkUnstructuredGrid::SafeDownCast(facesPds->GetPartition(partIdx))
      : nullptr;

    vtkPoints* oldPoints = elements->GetPoints();
    if (oldPoints == nullptr || !oldPoints->GetNumberOfPoints())
    { // no points
      continue;
    }
    vtkCellArray* oldCells = elements->GetCells();
    if (oldCells == nullptr || !oldCells->GetNumberOfCells())
    { // no cells.
      continue;
    }
    vtkUnsignedCharArray* oldCellTypes = elements->GetCellTypesArray();
    vtkNew<vtkUnstructuredGrid> newMesh;
    vtkNew<vtkUnsignedCharArray> newCellTypes;
    vtkNew<vtkPointData> hGradFields, hCurlFields, hDivFields;
    auto newPoints = vtk::TakeSmartPointer(oldPoints->NewInstance());
    auto newCells = vtk::TakeSmartPointer(oldCells->NewInstance());
    this->Internals->Allocate(newPoints, newCells, newCellTypes, hGradFields, hCurlFields,
      hDivFields, elements, edges, faces);

    // copy/interpolate dataset attributes.
    vtkCellData *oldCd = elements->GetCellData(), *newCd = newMesh->GetCellData();
    vtkPointData *oldPd = elements->GetPointData(), *newPd = newMesh->GetPointData();
    vtkFieldData *oldFd = elements->GetFieldData(), *newFd = newMesh->GetFieldData();
    newPd->InterpolateAllocate(oldPd);
    newCd->CopyAllocate(oldCd);
    newFd->DeepCopy(oldFd);

    const vtkIdType& nCells = oldCells->GetNumberOfCells();
    for (vtkIdType c = 0; c < nCells; ++c)
    {
      const unsigned char cellType = oldCellTypes->GetValue(c);

      this->Internals->ExplodeCell(c, cellType, oldPoints, newPoints, oldCells, newCells,
        newCellTypes, oldPd, newPd, oldCd, hGradFields);
      this->Internals->InterpolateCellToNodes(
        c, cellType, oldCells, newCells, edges, faces, hCurlFields, hDivFields);

      newCd->CopyData(oldCd, c, c);
      this->UpdateProgress(static_cast<double>(c) / nCells);
    } // for each cell

    // Copy over the hgrad/hcurl/hdiv fields into output point data.
    for (int i = 0; i < hGradFields->GetNumberOfArrays(); ++i)
    {
      const char* name = hGradFields->GetArrayName(i);
      newPd->AddArray(hGradFields->GetArray(name));
      newCd->RemoveArray(name);
    }
    for (int i = 0; i < hCurlFields->GetNumberOfArrays(); ++i)
    {
      const char* name = hCurlFields->GetArrayName(i);
      newPd->AddArray(hCurlFields->GetArray(name));
    }
    for (int i = 0; i < hDivFields->GetNumberOfArrays(); ++i)
    {
      const char* name = hDivFields->GetArrayName(i);
      newPd->AddArray(hDivFields->GetArray(name));
    }
    newMesh->SetPoints(newPoints);
    newMesh->SetCells(newCellTypes, newCells);
    output->SetPartition(0, partIdx, newMesh);
    output->GetMetaData(int(0))->Set(vtkCompositeDataSet::NAME(), elementBlockName.c_str());
  } // for each partition
  return 1;
}
