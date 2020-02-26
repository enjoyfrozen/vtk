/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPointFeatures.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPointFeatures.h"

#include "vtkCellArray.h"
#include "vtkCellArrayIterator.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkDataSetAttributes.h"
#include "vtkGenerateGlobalIds.h"
#include "vtkGeometryFilter.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkFeatureEdges.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPPolyDataNormals.h"
#include "vtkVector.h"

#include <set>
#include <map>

vtkStandardNewMacro(vtkPointFeatures);

vtkPointFeatures::vtkPointFeatures()
  : FeatureAngle(30.0)
{
}

vtkPointFeatures::~vtkPointFeatures()
{
}

void vtkPointFeatures::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "FeatureAngle: " << this->FeatureAngle << "\n";
}

int vtkPointFeatures::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // get the info objects
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // get the input and output
  vtkDataSet* input = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData* output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // This set holds pairs of global point IDs that form ridgelines.
  std::set<std::pair<vtkIdType, vtkIdType>> edges;
  // This map goes from global ID to an array of 3-tuples.
  // The first 3-tuple is always the point coordinates.
  // The following 3-tuples are normals at the point.
  std::map<vtkIdType, std::vector<vtkVector3d>> features;

  vtkNew<vtkGenerateGlobalIds> globalIds;
  vtkNew<vtkGeometryFilter> surface;
  vtkNew<vtkFeatureEdges> featureEdges;

  if (!input->GetPointData()->GetGlobalIds())
  {
    globalIds->SetInputDataObject(0, input);
    surface->SetInputConnection(globalIds->GetOutputPort());
  }
  else
  {
    surface->SetInputDataObject(0, input);
  }
  surface->MergingOn();

  // Handle special test-data case:
  surface->Update();
  if (surface->GetOutput()->GetNumberOfPolys() == 0 && surface->GetOutput()->GetNumberOfLines() > 0)
  {
    this->EdgeCorners(surface->GetOutput(), features);
  }
  else
  {
    featureEdges->SetInputConnection(surface->GetOutputPort());
    featureEdges->SetFeatureAngle(this->FeatureAngle);
    featureEdges->BoundaryEdgesOn();
    featureEdges->FeatureEdgesOn();
    featureEdges->ColoringOn(); // Generate 'Edge Types' cell-data
    featureEdges->Update();

    // Add edge points
    this->EdgePoints(featureEdges->GetOutput(), edges);

    vtkNew<vtkPPolyDataNormals> normals;
    normals->ComputePointNormalsOn();
    normals->ComputeCellNormalsOn();
    normals->SplittingOff();
    normals->SetInputConnection(surface->GetOutputPort());
    normals->Update();
    this->SurfaceCorners(normals->GetOutput(), edges, features);
  }

  // Generate output from collected global IDs
  vtkNew<vtkPoints> pts;
  vtkNew<vtkCellArray> verts;
  output->Initialize();
  output->SetPoints(pts);
  output->SetVerts(verts);

  auto numPoints = static_cast<vtkIdType>(features.size());
  pts->Allocate(numPoints);

  vtkNew<vtkDoubleArray> normsOut;
  normsOut->SetName("normal");
  normsOut->SetNumberOfComponents(3);
  normsOut->Allocate(numPoints * 3);

  vtkNew<vtkIdTypeArray> globIdsOut;
  globIdsOut->SetName("globalID");
  globIdsOut->Allocate(numPoints);

  for (auto fit = features.begin(); fit != features.end(); ++fit)
  {
    if (fit->second.empty())
    {
      continue;
    }
    auto nit = fit->second.begin();
    vtkVector3d coords(*nit);
    ++nit; // Advance past point coordinates
    vtkIdType conn = pts->InsertNextPoint(coords.GetData());
    for (int uu = 0/*no init*/; nit != fit->second.end(); ++nit, ++uu)
    {
      vtkIdType vertCell = output->InsertNextCell(VTK_VERTEX, 1, &conn);
      (void) vertCell; // Keep result for debugging, avoid compiler warnings.
      normsOut->InsertNextTuple(nit->GetData());
      globIdsOut->InsertNextValue(fit->first);
    }
  }
  vtkDebugMacro(
    "Have " << features.size() << " unique points, " <<
    globIdsOut->GetNumberOfTuples() << " features.");

  auto cdataOut = output->GetCellData();
  cdataOut->SetNormals(normsOut);
  cdataOut->SetGlobalIds(globIdsOut);

  return 1;
}

int vtkPointFeatures::FillInputPortInformation(int, vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}

template<typename Container>
void vtkPointFeatures::EdgePoints(vtkPolyData* data, Container& edges)
{
  auto pdata = data->GetPointData();
  auto globIds = vtkIdTypeArray::SafeDownCast(pdata->GetGlobalIds());
  if (!globIds)
  {
    return;
  }
  // Build a map from line points to surface points using global IDs
  vtkCellArray* lines = data->GetLines();
  auto it = vtk::TakeSmartPointer(lines->NewIterator());
  for (it->GoToFirstCell(); !it->IsDoneWithTraversal(); it->GoToNextCell())
  {
    vtkIdType numPointsThisCell;
    const vtkIdType* cellPoints;
    it->GetCurrentCell(numPointsThisCell, cellPoints);
    if (numPointsThisCell == 2)
    {
      auto g0 = globIds->GetValue(cellPoints[0]);
      auto g1 = globIds->GetValue(cellPoints[1]);
      // Always store edge directed from lowest id to highest.
      std::pair<vtkIdType, vtkIdType> edge(g0 < g1 ? g0 : g1, g0 < g1 ? g1 : g0);
      edges.insert(edge);
    }
    else
    {
      vtkErrorMacro("Feature edge "
        << it->GetCurrentCellId()
        << " had " << numPointsThisCell << " points, not 2. Skipping.");
    }
  }
}

template<typename SurfaceNormalConstraints, typename EdgeSet>
void vtkPointFeatures::SurfaceCorners(vtkPolyData* data, EdgeSet& edges, SurfaceNormalConstraints& featurePoints)
{
  auto pdata = data->GetPointData();
  auto pts = data->GetPoints();
  auto globIds = vtkIdTypeArray::SafeDownCast(pdata->GetGlobalIds());
  auto norms = pdata->GetNormals();
  auto cnorms = data->GetCellData()->GetNormals();
  if (!cnorms || !norms || !globIds)
  {
    return;
  }

  // Build a map from global point ids to an array holding a point and any
  // normals the point's frame-field should be constrained to.
  std::vector<bool> isFeature;
  vtkCellArray* polys = data->GetPolys();
  auto it = vtk::TakeSmartPointer(polys->NewIterator());
  for (it->GoToFirstCell(); !it->IsDoneWithTraversal(); it->GoToNextCell())
  {
    vtkIdType numPointsThisCell;
    const vtkIdType* cellPoints;
    it->GetCurrentCell(numPointsThisCell, cellPoints);
    isFeature.clear();
    isFeature.resize(numPointsThisCell);
    // Find feature-edge segments along this polygon's border.
    // For each segment bounding this polygon, add an entry to isFeature:
    for (vtkIdType ii = 0; ii < numPointsThisCell; ++ii)
    {
      auto g0 = globIds->GetValue(cellPoints[ii]);
      auto g1 = globIds->GetValue(cellPoints[(ii + 1) % numPointsThisCell]);
      std::pair<vtkIdType, vtkIdType> edge(g0 < g1 ? g0 : g1, g0 < g1 ? g1 : g0);
      isFeature[ii] = edges.find(edge) != edges.end();
    }
    // For each point on this polygon, insert any required constraints:
    for (vtkIdType ii = 0; ii < numPointsThisCell; ++ii)
    {
      auto g0 = globIds->GetValue(cellPoints[ii]);
      bool f0 = isFeature[(ii + numPointsThisCell - 1) % numPointsThisCell];
      bool f1 = isFeature[ii];
      typename SurfaceNormalConstraints::iterator featIt = featurePoints.find(g0);
      // If the point coordinates have not been inserted, do so now:
      if (featIt == featurePoints.end())
      {
        typename SurfaceNormalConstraints::value_type blank{g0,{}};
        auto result = featurePoints.insert(blank);
        featIt = result.first;
      }
      if (featIt->second.empty())
      {
        vtkVector3d x0;
        pts->GetPoint(cellPoints[ii], x0.GetData());
        featIt->second.push_back(x0);
      }
      if (f0 || f1)
      {
        // The point is adjacent to a feature edge.
        // Add a constraint for the *face* normal.
        vtkVector3d n0;
        cnorms->GetTuple(it->GetCurrentCellId(), n0.GetData());
        featIt->second.push_back(n0);
      }
      else
      {
        // The point is not on the interior of a feature edge.
        // If no normal has been recorded for this point,
        // add a constraint for the *point* normal.
        if (featIt->second.size() < 2)
        {
          vtkVector3d n0;
          norms->GetTuple(cellPoints[ii], n0.GetData());
          featIt->second.push_back(n0);
        }
      }
    }
  }
}

template<typename SurfaceNormalConstraints>
void vtkPointFeatures::EdgeCorners(vtkPolyData* edges, SurfaceNormalConstraints& features)
{
  if (!edges)
  {
    return;
  }
  auto pts = edges->GetPoints();
  auto vec = edges->GetPointData()->GetVectors();
  auto gid = vtkIdTypeArray::SafeDownCast(edges->GetPointData()->GetGlobalIds());
  if (!vec || !gid)
  {
    return;
  }
  vtkIdType np = pts->GetNumberOfPoints();
  vtkVector3d pc;
  vtkVector3d vv;
  vtkVector3d zz{0, 0, 1};
  for (vtkIdType pp = 0; pp < np; ++pp)
  {
    pts->GetPoint(pp, pc.GetData());
    vec->GetTuple(pp, vv.GetData());
    auto gg = gid->GetValue(pp);
    if (vv.SquaredNorm() > 1e-8)
    {
      typename SurfaceNormalConstraints::value_type blank{gg,{}};
      auto result = features.insert(blank);
      auto featIt = result.first;
      featIt->second.push_back(pc);
      featIt->second.push_back(vv);
      featIt->second.push_back(vv.Cross(zz));
    }
  }
}
