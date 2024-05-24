// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkNewFeatureEdges.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkFloatArray.h"
#include "vtkIdTypeArray.h"
#include "vtkIncrementalPointLocator.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkMergePoints.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolygon.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTriangleStrip.h"
#include "vtkUnsignedCharArray.h"

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkNewFeatureEdges);

//------------------------------------------------------------------------------
// Construct object with feature angle = 30; all types of edges, except
// manifold edges, are extracted and colored.
vtkNewFeatureEdges::vtkNewFeatureEdges()
{
  this->FeatureAngle = 30.0;
  this->BoundaryEdges = true;
  this->FeatureEdges = true;
  this->NonManifoldEdges = true;
  this->ManifoldEdges = false;
  this->Coloring = true;
  this->GeneratePedigreeIds = false;
  this->Locator = nullptr;
  this->OutputPointsPrecision = vtkAlgorithm::DEFAULT_PRECISION;
}

//------------------------------------------------------------------------------
vtkNewFeatureEdges::~vtkNewFeatureEdges()
{
  if (this->Locator)
  {
    this->Locator->UnRegister(this);
    this->Locator = nullptr;
  }
}

//------------------------------------------------------------------------------
// Generate feature edges for mesh
int vtkNewFeatureEdges::RequestData(vtkInformation* /*request*/, vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  // get the info objects
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // get the input and output
  vtkPolyData* input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData* output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkPoints* inPts;
  vtkPoints* newPts;
  vtkNew<vtkFloatArray> newScalars;
  vtkCellArray* newLines;
  vtkPolyData* Mesh;
  int i;
  vtkIdType j;
  vtkIdType numNei;
  vtkIdType cellId;
  vtkIdType numNonManifoldEdges;
  vtkIdType numFedges;
  vtkIdType numManifoldEdges;
  double scalar;
  double n[3];
  double x1[3];
  double x2[3];
  double cosAngle = 0;
  vtkIdType lineIds[2];
  vtkIdType npts = 0;
  const vtkIdType* pts = nullptr;
  vtkCellArray* inPolys;
  vtkCellArray* inStrips;
  vtkCellArray* newPolys;
  vtkNew<vtkFloatArray> polyNormals;
  vtkIdType numPts;
  vtkIdType numCells;
  vtkIdType numPolys;
  vtkIdType numStrips;
  vtkIdType nei;
  vtkIdList* neighbors;
  vtkIdType p1;
  vtkIdType p2;
  vtkIdType newId;
  vtkPointData* pd = input->GetPointData();
  vtkPointData* outPD = output->GetPointData();
  vtkCellData* cd = input->GetCellData();
  vtkCellData* outCD = output->GetCellData();
  unsigned char* ghosts = nullptr;
  vtkDebugMacro(<< "Executing feature edges");

  vtkDataArray* temp = nullptr;
  if (cd)
  {
    temp = cd->GetArray(vtkDataSetAttributes::GhostArrayName());
  }
  if ((!temp) || (temp->GetDataType() != VTK_UNSIGNED_CHAR) || (temp->GetNumberOfComponents() != 1))
  {
    vtkDebugMacro("No appropriate ghost levels field available.");
  }
  else
  {
    ghosts = static_cast<vtkUnsignedCharArray*>(temp)->GetPointer(0);
  }

  //  Check input
  //
  inPts = input->GetPoints();
  numCells = input->GetNumberOfCells();
  numPolys = input->GetNumberOfPolys();
  numStrips = input->GetNumberOfStrips();
  if ((numPts = input->GetNumberOfPoints()) < 1 || !inPts || (numPolys < 1 && numStrips < 1))
  {
    vtkDebugMacro(<< "No input data!");
    return 1;
  }

  if (!this->BoundaryEdges && !this->NonManifoldEdges && !this->FeatureEdges &&
    !this->ManifoldEdges)
  {
    vtkDebugMacro(<< "All edge types turned off!");
  }

  // Build cell structure.  Might have to triangulate the strips.
  Mesh = vtkPolyData::New();
  Mesh->SetPoints(inPts);
  inPolys = input->GetPolys();
  if (numStrips > 0)
  {
    newPolys = vtkCellArray::New();
    if (numPolys > 0)
    {
      newPolys->DeepCopy(inPolys);
    }
    else
    {
      newPolys->AllocateEstimate(numStrips, 5);
    }
    inStrips = input->GetStrips();
    for (inStrips->InitTraversal(); inStrips->GetNextCell(npts, pts);)
    {
      vtkTriangleStrip::DecomposeStrip(npts, pts, newPolys);
    }
    Mesh->SetPolys(newPolys);
    newPolys->Delete();
  }
  else
  {
    newPolys = inPolys;
    Mesh->SetPolys(newPolys);
  }
  Mesh->BuildLinks();

  // Allocate storage for lines/points (arbitrary allocation sizes)
  //
  newPts = vtkPoints::New();

  // Set the desired precision for the points in the output.
  if (this->OutputPointsPrecision == vtkAlgorithm::DEFAULT_PRECISION)
  {
    newPts->SetDataType(inPts->GetDataType());
  }
  else if (this->OutputPointsPrecision == vtkAlgorithm::SINGLE_PRECISION)
  {
    newPts->SetDataType(VTK_FLOAT);
  }
  else if (this->OutputPointsPrecision == vtkAlgorithm::DOUBLE_PRECISION)
  {
    newPts->SetDataType(VTK_DOUBLE);
  }

  newPts->Allocate(numPts / 10, numPts);
  newLines = vtkCellArray::New();
  newLines->AllocateEstimate(numPts / 20, 2);
  if (this->Coloring)
  {
    newScalars->SetName("Edge Types");
    newScalars->Allocate(numCells / 10, numCells);
  }

  outPD->CopyGlobalIdsOn();
  outPD->CopyAllocate(pd, numPts);
  outCD->CopyAllocate(cd, numCells);

  // Get our locator for merging points
  //
  if (this->Locator == nullptr)
  {
    this->CreateDefaultLocator();
  }
  this->Locator->InitPointInsertion(newPts, input->GetBounds());

  // Loop over all polygons generating boundary, non-manifold,
  // and feature edges
  //
  if (this->FeatureEdges)
  {
    polyNormals->SetNumberOfComponents(3);
    polyNormals->Allocate(3 * newPolys->GetNumberOfCells());

    for (cellId = 0, newPolys->InitTraversal(); newPolys->GetNextCell(npts, pts); cellId++)
    {
      vtkPolygon::ComputeNormal(inPts, npts, pts, n);
      polyNormals->InsertTuple(cellId, n);
    }

    cosAngle = cos(vtkMath::RadiansFromDegrees(this->FeatureAngle));
  }

  neighbors = vtkIdList::New();
  neighbors->Allocate(VTK_CELL_SIZE);

  int abort = 0;
  vtkIdType progressInterval = numCells / 20 + 1;

  vtkIdType numBEdges = numNonManifoldEdges = numFedges = numManifoldEdges = 0;
  vtkSmartPointer<vtkIdTypeArray> pedigree;
  if (this->GeneratePedigreeIds)
  {
    pedigree = vtkSmartPointer<vtkIdTypeArray>::New();
    pedigree->SetName("Pedigree ID");
  }
  for (cellId = 0, newPolys->InitTraversal(); newPolys->GetNextCell(npts, pts) && !abort; cellId++)
  {
    if (!(cellId % progressInterval)) // manage progress / early abort
    {
      this->UpdateProgress(static_cast<double>(cellId) / numCells);
      abort = this->GetAbortExecute();
    }

    for (i = 0; i < npts; i++)
    {
      p1 = pts[i];
      p2 = pts[(i + 1) % npts];

      Mesh->GetCellEdgeNeighbors(cellId, p1, p2, neighbors);
      numNei = neighbors->GetNumberOfIds();

      if (this->BoundaryEdges && numNei < 1)
      {
        if (ghosts && ghosts[cellId] & vtkDataSetAttributes::DUPLICATECELL)
        {
          continue;
        }
        numBEdges++;
        scalar = 0.0;
      }

      else if (this->NonManifoldEdges && numNei > 1)
      {
        // check to make sure that this edge hasn't been created before
        for (j = 0; j < numNei; j++)
        {
          if (neighbors->GetId(j) < cellId)
          {
            break;
          }
        }
        if (j >= numNei)
        {
          if (ghosts && ghosts[cellId] & vtkDataSetAttributes::DUPLICATECELL)
          {
            continue;
          }
          numNonManifoldEdges++;
          scalar = 0.222222;
        }
        else
        {
          continue;
        }
      }
      else if (this->FeatureEdges && numNei == 1 && (nei = neighbors->GetId(0)) > cellId)
      {
        double neiTuple[3];
        double cellTuple[3];
        polyNormals->GetTuple(nei, neiTuple);
        polyNormals->GetTuple(cellId, cellTuple);
        if (vtkMath::Dot(neiTuple, cellTuple) <= cosAngle)
        {
          if (ghosts && ghosts[cellId] & vtkDataSetAttributes::DUPLICATECELL)
          {
            continue;
          }

          numFedges++;
          scalar = 0.444444;
        }
        else
        {
          continue;
        }
      }
      else if (this->ManifoldEdges && numNei == 1 && neighbors->GetId(0) > cellId)
      {
        if (ghosts && ghosts[cellId] & vtkDataSetAttributes::DUPLICATECELL)
        {
          continue;
        }

        numManifoldEdges++;
        scalar = 0.666667;
      }
      else
      {
        continue;
      }

      // Add edge to output
      Mesh->GetPoint(p1, x1);
      Mesh->GetPoint(p2, x2);

      if (this->Locator->InsertUniquePoint(x1, lineIds[0]))
      {
        outPD->CopyData(pd, p1, lineIds[0]);
        if (pedigree)
        {
          pedigree->InsertNextValue(p1);
        }
      }

      if (this->Locator->InsertUniquePoint(x2, lineIds[1]))
      {
        outPD->CopyData(pd, p2, lineIds[1]);
        if (pedigree)
        {
          pedigree->InsertNextValue(p2);
        }
      }

      newId = newLines->InsertNextCell(2, lineIds);
      outCD->CopyData(cd, cellId, newId);
      if (this->Coloring)
      {
        newScalars->InsertTuple(newId, &scalar);
      }
    }
  }
  if (pedigree)
  {
    outPD->SetPedigreeIds(pedigree);
  }

  vtkDebugMacro(<< "Created " << numBEdges << " boundary edges, " << numNonManifoldEdges
                << " non-manifold edges, " << numFedges << " feature edges, " << numManifoldEdges
                << " manifold edges");

  //  Update ourselves.
  //

  Mesh->Delete();

  output->SetPoints(newPts);
  newPts->Delete();
  neighbors->Delete();

  output->SetLines(newLines);
  newLines->Delete();
  this->Locator->Initialize(); // release any extra memory
  if (this->Coloring)
  {
    int idx = outCD->AddArray(newScalars);
    outCD->SetActiveAttribute(idx, vtkDataSetAttributes::SCALARS);
  }

  return 1;
}

//------------------------------------------------------------------------------
void vtkNewFeatureEdges::CreateDefaultLocator()
{
  if (this->Locator == nullptr)
  {
    this->Locator = vtkMergePoints::New();
  }
}

//------------------------------------------------------------------------------
// Specify a spatial locator for merging points. By
// default an instance of vtkMergePoints is used.
void vtkNewFeatureEdges::SetLocator(vtkIncrementalPointLocator* locator)
{
  if (this->Locator == locator)
  {
    return;
  }
  if (this->Locator)
  {
    this->Locator->UnRegister(this);
    this->Locator = nullptr;
  }
  if (locator)
  {
    locator->Register(this);
  }
  this->Locator = locator;
  this->Modified();
}

//------------------------------------------------------------------------------
vtkMTimeType vtkNewFeatureEdges::GetMTime()
{
  vtkMTimeType mTime = this->Superclass::GetMTime();
  vtkMTimeType time;

  if (this->Locator != nullptr)
  {
    time = this->Locator->GetMTime();
    mTime = (time > mTime ? time : mTime);
  }
  return mTime;
}

//------------------------------------------------------------------------------
int vtkNewFeatureEdges::RequestUpdateExtent(vtkInformation* /*request*/,
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // get the info objects
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  int numPieces;
  int ghostLevel;

  numPieces = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES());
  ghostLevel = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_GHOST_LEVELS());

  if (numPieces > 1)
  {
    inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_GHOST_LEVELS(), ghostLevel + 1);
  }

  return 1;
}

//------------------------------------------------------------------------------
void vtkNewFeatureEdges::ExtractAllEdgeTypesOn()
{
  this->BoundaryEdgesOn();
  this->FeatureEdgesOn();
  this->NonManifoldEdgesOn();
  this->ManifoldEdgesOn();
}

//------------------------------------------------------------------------------
void vtkNewFeatureEdges::ExtractAllEdgeTypesOff()
{
  this->BoundaryEdgesOff();
  this->FeatureEdgesOff();
  this->NonManifoldEdgesOff();
  this->ManifoldEdgesOff();
}

//------------------------------------------------------------------------------
void vtkNewFeatureEdges::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Feature Angle: " << this->FeatureAngle << "\n";
  os << indent << "Boundary Edges: " << (this->BoundaryEdges ? "On\n" : "Off\n");
  os << indent << "Feature Edges: " << (this->FeatureEdges ? "On\n" : "Off\n");
  os << indent << "Non-Manifold Edges: " << (this->NonManifoldEdges ? "On\n" : "Off\n");
  os << indent << "Manifold Edges: " << (this->ManifoldEdges ? "On\n" : "Off\n");
  os << indent << "Coloring: " << (this->Coloring ? "On\n" : "Off\n");

  if (this->Locator)
  {
    os << indent << "Locator: " << this->Locator << "\n";
  }
  else
  {
    os << indent << "Locator: (none)\n";
  }

  os << indent << "Output Points Precision: " << this->OutputPointsPrecision << "\n";
}
VTK_ABI_NAMESPACE_END
