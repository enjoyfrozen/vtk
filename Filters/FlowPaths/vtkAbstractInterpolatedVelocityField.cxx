/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkAbstractInterpolatedVelocityField.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkAbstractInterpolatedVelocityField.h"

#include "vtkClosestPointStrategy.h"
#include "vtkCompositeDataIterator.h"
#include "vtkCompositeDataSet.h"
#include "vtkCompositeInterpolatedVelocityField.h"
#include "vtkDataArray.h"
#include "vtkDataObject.h"
#include "vtkDataSet.h"
#include "vtkGenericCell.h"
#include "vtkMath.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"

#include <map>
#include <utility> //make_pair

//------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_BEGIN
vtkCxxSetObjectMacro(vtkAbstractInterpolatedVelocityField, FindCellStrategy, vtkFindCellStrategy);

//------------------------------------------------------------------------------
const double vtkAbstractInterpolatedVelocityField::TOLERANCE_SCALE = 1.0E-8;
const double vtkAbstractInterpolatedVelocityField::SURFACE_TOLERANCE_SCALE = 1.0E-5;

namespace
{ // anonymous

// This is used to keep track of the find cell strategy and vector array
// associated with each dataset forming the velocity field. Note that the
// find cells strategy can be null, this means the find cell is invoked
// using the dataset's FindCell() method.
struct vtkFunctionCache
{
  vtkFindCellStrategy* Strategy;
  vtkDataArray* Vectors;

  vtkFunctionCache(vtkFindCellStrategy* strategy, vtkDataArray* vectors)
    : Strategy(strategy)
    , Vectors(vectors)
  {
  }
};

// Cache information relative to each input dataset defining the velocity
// field. This is done for performance and to mange the different find cell
} // anonymous

// strategies associated with each dataset.
struct vtkFunctionCacheMap : public std::map<vtkDataObject*, vtkFunctionCache>
{
};

//------------------------------------------------------------------------------
vtkAbstractInterpolatedVelocityField::vtkAbstractInterpolatedVelocityField()
{
  this->NumFuncs = 3;     // u, v, w
  this->NumIndepVars = 4; // x, y, z, t
  this->Weights = nullptr;
  this->WeightsSize = 0;

  this->Caching = true; // Caching on by default
  this->CacheHit = 0;
  this->CacheMiss = 0;

  this->LastCellId = -1;
  this->LastDataSet = nullptr;
  this->LastPCoords[0] = 0.0;
  this->LastPCoords[1] = 0.0;
  this->LastPCoords[2] = 0.0;

  this->VectorsType = 0;
  this->VectorsSelection = nullptr;
  this->NormalizeVector = false;
  this->ForceSurfaceTangentVector = false;
  this->SurfaceDataset = false;

  this->Cell = vtkGenericCell::New();
  this->GenCell = vtkGenericCell::New();

  this->InitializationState = NOT_INITIALIZED;
  this->FindCellStrategy = nullptr;
  this->FunctionCacheMap = new vtkFunctionCacheMap;
}

//------------------------------------------------------------------------------
vtkAbstractInterpolatedVelocityField::~vtkAbstractInterpolatedVelocityField()
{
  this->NumFuncs = 0;
  this->NumIndepVars = 0;

  this->LastDataSet = nullptr;
  this->SetVectorsSelection(nullptr);

  delete[] this->Weights;
  this->Weights = nullptr;

  if (this->Cell)
  {
    this->Cell->Delete();
    this->Cell = nullptr;
  }

  if (this->GenCell)
  {
    this->GenCell->Delete();
    this->GenCell = nullptr;
  }

  // Need to free strategies and other information associated with each
  // dataset. There is a special case where the strategy cannot be deleted
  // because is has been specified by the user.
  vtkFindCellStrategy* strat;
  for (auto iter = this->FunctionCacheMap->begin(); iter != this->FunctionCacheMap->end(); ++iter)
  {
    strat = iter->second.Strategy;
    if (strat != nullptr)
    {
      strat->Delete();
    }
  }
  delete this->FunctionCacheMap;

  this->SetFindCellStrategy(nullptr);
}

//------------------------------------------------------------------------------
void vtkAbstractInterpolatedVelocityField::Initialize(vtkCompositeDataSet* compDS, int initStrategy)
{
  // Clear the function cache, subclasses may want to put stuff into it.
  this->FunctionCacheMap->clear();

  // See whether the subclass should take over the initialization process.
  if (this->SelfInitialize())
  {
    return;
  }

  // Proceed to initialize the composite dataset
  this->InitializationState = initStrategy;

  // Obtain this find cell strategy or create the default one as necessary
  vtkSmartPointer<vtkFindCellStrategy> strategy = this->FindCellStrategy;
  vtkFindCellStrategy* strategyClone;
  if (strategy == nullptr)
  {
    strategy = vtkSmartPointer<vtkClosestPointStrategy>::New(); // default strategy if not provided
  }

  // These are the datasets to process from the input to the filter.
  auto datasets = vtkCompositeDataSet::GetDataSets(compDS);

  // For each dataset in the list of datasets, make sure a FindCell
  // strategy has been defined and initialized. The potential for composite
  // datasets which may contain instances of (vtkPointSet) make the process
  // more complex. We only care about find cell strategies if the dataset is
  // a vtkPointSet because the other dataset types (e.g., volumes) have their
  // own built-in FindCell() methods.
  vtkDataArray* vectors;
  for (size_t cc = 0; cc < datasets.size(); ++cc)
  {
    if (!this->VectorsSelection) // if a selection is not specified,
    {
      // use the first one in the point set (this is a behavior for backward compatibility)
      vectors = datasets[cc]->GetPointData()->GetVectors(nullptr);
    }
    else
    {
      vectors =
        datasets[cc]->GetAttributesAsFieldData(this->VectorsType)->GetArray(this->VectorsSelection);
    }

    vtkPointSet* ps = vtkPointSet::SafeDownCast(datasets[cc]);
    strategyClone = nullptr;
    if (ps != nullptr)
    {
      strategyClone = strategy->NewInstance();
    }

    this->FunctionCacheMap->insert(
      std::make_pair(datasets[cc], vtkFunctionCache(strategyClone, vectors)));
  } // for all datasets of composite dataset

  // Now initialize the new strategies
  for (size_t cc = 0; cc < datasets.size(); ++cc)
  {
    vtkPointSet* ps = vtkPointSet::SafeDownCast(datasets[cc]);
    if (ps != nullptr)
    {
      vtkFunctionCacheMap::iterator sIter = this->FunctionCacheMap->find(datasets[cc]);
      strategyClone = sIter->second.Strategy;
      strategyClone->CopyParameters(strategy);
      strategyClone->Initialize(ps);
    }
  }

  // Now perform initialization on certain data sets - a nasty hack.
  // Closest point traversal requires cell links to be built. Only build
  // links if necessary.
  for (size_t cc = 0; cc < datasets.size(); ++cc)
  {
    vtkFunctionCacheMap::iterator sIter = this->FunctionCacheMap->find(datasets[cc]);
    if (sIter->second.Strategy != nullptr &&
      vtkClosestPointStrategy::SafeDownCast(sIter->second.Strategy) != nullptr)
    {
      vtkUnstructuredGrid* ug = vtkUnstructuredGrid::SafeDownCast(datasets[cc]);
      vtkPolyData* pd = vtkPolyData::SafeDownCast(datasets[cc]);

      if (ug != nullptr)
      {
        ug->BuildLinks();
      }
      else if (pd != nullptr)
      {
        pd->BuildLinks();
      }
    }
  }
}

//------------------------------------------------------------------------------
int vtkAbstractInterpolatedVelocityField::FunctionValues(vtkDataSet* dataset, double* x, double* f)
{
  // Make sure the velocity field has been initialized. If not initialized,
  // then self initialization is invoked which may not be thead safe, and
  // data races may result. Meant to support backward legacy in serial
  // execution.
  if (this->InitializationState == NOT_INITIALIZED)
  {
    vtkWarningMacro(<< "Velocity field not initialized for threading!");
    this->SelfInitialize();
  }

  // See if a dataset has been specified and if there are input vectors
  if (!dataset)
  {
    vtkErrorMacro(<< "Can't evaluate dataset!");
    return 0;
  }

  // Retrieve cached function array
  vtkDataArray* vectors = nullptr;
  vtkFunctionCacheMap::iterator sIter = this->FunctionCacheMap->find(dataset);
  if (sIter != this->FunctionCacheMap->end())
  {
    vectors = sIter->second.Vectors;
  }

  if (!vectors)
  {
    vtkErrorMacro(<< "No vectors for dataset!");
    return 0;
  }

  // Compute function values for the dataset
  int i, j, numPts, id;
  double vec[3];
  f[0] = f[1] = f[2] = 0.0;

  if (!this->FindAndUpdateCell(dataset, x))
  {
    vectors = nullptr;
    return 0;
  }

  // if the cell is valid
  if (this->LastCellId >= 0)
  {
    numPts = this->GenCell->GetNumberOfPoints();

    // interpolate the vectors
    if (this->VectorsType == vtkDataObject::POINT)
    {
      for (j = 0; j < numPts; j++)
      {
        id = this->GenCell->PointIds->GetId(j);
        vectors->GetTuple(id, vec);
        for (i = 0; i < 3; i++)
        {
          f[i] += vec[i] * this->Weights[j];
        }
      }
    }
    else
    {
      vectors->GetTuple(this->LastCellId, f);
    }

    if (this->ForceSurfaceTangentVector)
    {
      vtkNew<vtkIdList> ptIds;
      dataset->GetCellPoints(this->LastCellId, ptIds);
      if (ptIds->GetNumberOfIds() < 3)
      {
        vtkErrorMacro(<< "Cannot compute normal on cells with less than 3 points");
      }
      else
      {
        double p1[3];
        double p2[3];
        double p3[3];
        double normal[3];
        double v1[3], v2[3];
        double k;

        dataset->GetPoint(ptIds->GetId(0), p1);
        dataset->GetPoint(ptIds->GetId(1), p2);
        dataset->GetPoint(ptIds->GetId(2), p3);

        // Compute othogonal component
        v1[0] = p2[0] - p1[0];
        v1[1] = p2[1] - p1[1];
        v1[2] = p2[2] - p1[2];
        v2[0] = p3[0] - p1[0];
        v2[1] = p3[1] - p1[1];
        v2[2] = p3[2] - p1[2];

        vtkMath::Cross(v1, v2, normal);
        vtkMath::Normalize(normal);
        k = vtkMath::Dot(normal, f);

        // Remove non orthogonal component.
        f[0] = f[0] - (normal[0] * k);
        f[1] = f[1] - (normal[1] * k);
        f[2] = f[2] - (normal[2] * k);
      }
    }

    if (this->NormalizeVector)
    {
      vtkMath::Normalize(f);
    }
  }
  // if not, return false
  else
  {
    vectors = nullptr;
    return 0;
  }

  vectors = nullptr;
  return 1;
}

//------------------------------------------------------------------------------
bool vtkAbstractInterpolatedVelocityField::CheckPCoords(double pcoords[3])
{
  for (int i = 0; i < 3; i++)
  {
    if (pcoords[i] < 0 || pcoords[i] > 1)
    {
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------------
bool vtkAbstractInterpolatedVelocityField::FindAndUpdateCell(vtkDataSet* dataset, double* x)
{
  double tol2, dist2;
  if (this->SurfaceDataset)
  {
    tol2 = dataset->GetLength() * dataset->GetLength() *
      vtkAbstractInterpolatedVelocityField::SURFACE_TOLERANCE_SCALE;
  }
  else
  {
    tol2 = dataset->GetLength() * dataset->GetLength() *
      vtkAbstractInterpolatedVelocityField::TOLERANCE_SCALE;
  }

  double closest[3];
  bool found = false;
  if (this->Caching)
  {
    bool out = false;

    // See if the point is in the cached cell
    if (this->LastCellId != -1)
    {
      // Use cache cell only if point is inside
      // or , with surface , not far and in pccords
      int ret = this->GenCell->EvaluatePosition(
        x, closest, this->LastSubId, this->LastPCoords, dist2, this->Weights);
      if (ret == -1 || (ret == 0 && !this->SurfaceDataset) ||
        (this->SurfaceDataset && (dist2 > tol2 || !this->CheckPCoords(this->LastPCoords))))
      {
        out = true;
      }

      if (out)
      {
        this->CacheMiss++;

        dataset->GetCell(this->LastCellId, this->Cell);

        // Search around current cached cell to see if there is a cell within tolerance
        vtkFindCellStrategy* strategy = nullptr;
        vtkFunctionCacheMap::iterator sIter = this->FunctionCacheMap->find(dataset);
        strategy = (sIter != this->FunctionCacheMap->end() ? sIter->second.Strategy : nullptr);

        this->LastCellId = ((strategy == nullptr)
            ? dataset->FindCell(x, this->Cell, this->GenCell, this->LastCellId, tol2,
                this->LastSubId, this->LastPCoords, this->Weights)
            : strategy->FindCell(x, this->Cell, this->GenCell, this->LastCellId, tol2,
                this->LastSubId, this->LastPCoords, this->Weights));

        if (this->LastCellId != -1 &&
          (!this->SurfaceDataset || this->CheckPCoords(this->LastPCoords)))
        {
          dataset->GetCell(this->LastCellId, this->GenCell);
          found = true;
        }
      }
      else
      {
        this->CacheHit++;
        found = true;
      }
    }
  } // if caching

  if (!found)
  {
    // if the cell is not found in cache, do a global search (ignore initial
    // cell if there is one)
    vtkFindCellStrategy* strategy = nullptr;
    vtkFunctionCacheMap::iterator sIter = this->FunctionCacheMap->find(dataset);
    strategy = (sIter != this->FunctionCacheMap->end() ? sIter->second.Strategy : nullptr);

    this->LastCellId =
      ((strategy == nullptr) ? dataset->FindCell(x, nullptr, this->GenCell, -1, tol2,
                                 this->LastSubId, this->LastPCoords, this->Weights)
                             : strategy->FindCell(x, nullptr, this->GenCell, -1, tol2,
                                 this->LastSubId, this->LastPCoords, this->Weights));

    if (this->LastCellId != -1 && (!this->SurfaceDataset || this->CheckPCoords(this->LastPCoords)))
    {
      dataset->GetCell(this->LastCellId, this->GenCell);
    }
    else
    {
      if (this->SurfaceDataset)
      {
        // Still cannot find cell, use a locator to find a (arbitrary) cell, for 2D surface
        vtkIdType idPoint = dataset->FindPoint(x);
        if (idPoint < 0)
        {
          this->LastCellId = -1;
          return false;
        }

        vtkNew<vtkIdList> cellList;
        dataset->GetPointCells(idPoint, cellList);
        double minDist2 = dataset->GetLength() * dataset->GetLength();
        vtkIdType minDistId = -1;
        for (vtkIdType idCell = 0; idCell < cellList->GetNumberOfIds(); idCell++)
        {
          this->LastCellId = cellList->GetId(idCell);
          dataset->GetCell(this->LastCellId, this->GenCell);
          int ret = this->GenCell->EvaluatePosition(
            x, closest, this->LastSubId, this->LastPCoords, dist2, this->Weights);
          if (ret != -1 && dist2 < minDist2)
          {
            minDistId = this->LastCellId;
            minDist2 = dist2;
          }
        }

        if (minDistId == -1)
        {
          this->LastCellId = -1;
          return false;
        }

        // Recover closest cell info
        this->LastCellId = minDistId;
        dataset->GetCell(this->LastCellId, this->GenCell);
        int ret = this->GenCell->EvaluatePosition(
          x, closest, this->LastSubId, this->LastPCoords, dist2, this->Weights);

        // Find Point being not perfect to find cell, check for closer cells
        vtkNew<vtkIdList> boundaryPoints;
        vtkNew<vtkIdList> neighCells;
        bool edge = false;
        bool closer;
        while (true)
        {
          this->GenCell->CellBoundary(this->LastSubId, this->LastPCoords, boundaryPoints);
          dataset->GetCellNeighbors(this->LastCellId, boundaryPoints, neighCells);
          if (neighCells->GetNumberOfIds() == 0)
          {
            edge = true;
            break;
          }
          closer = false;
          for (vtkIdType neighCellId = 0; neighCellId < neighCells->GetNumberOfIds(); neighCellId++)
          {
            this->LastCellId = neighCells->GetId(neighCellId);
            dataset->GetCell(this->LastCellId, this->GenCell);
            ret = this->GenCell->EvaluatePosition(
              x, closest, this->LastSubId, this->LastPCoords, dist2, this->Weights);
            if (ret != -1 && dist2 < minDist2)
            {
              minDistId = this->LastCellId;
              minDist2 = dist2;
              closer = true;
            }
          }
          if (!closer)
          {
            break;
          }
        }

        // Recover closest cell info
        if (!edge)
        {
          this->LastCellId = minDistId;
          dataset->GetCell(this->LastCellId, this->GenCell);
          this->GenCell->EvaluatePosition(
            x, closest, this->LastSubId, this->LastPCoords, dist2, this->Weights);
        }
        if (minDist2 > tol2 || (!this->CheckPCoords(this->LastPCoords) && edge))
        {
          this->LastCellId = -1;
          return false;
        }
      }
      else
      {
        this->LastCellId = -1;
        return false;
      }
    }
  }
  return true;
}
//------------------------------------------------------------------------------
int vtkAbstractInterpolatedVelocityField::GetLastWeights(double* w)
{
  if (this->LastCellId < 0)
  {
    return 0;
  }

  int numPts = this->GenCell->GetNumberOfPoints();
  for (int i = 0; i < numPts; i++)
  {
    w[i] = this->Weights[i];
  }

  return 1;
}

//------------------------------------------------------------------------------
int vtkAbstractInterpolatedVelocityField::GetLastLocalCoordinates(double pcoords[3])
{
  if (this->LastCellId < 0)
  {
    return 0;
  }

  pcoords[0] = this->LastPCoords[0];
  pcoords[1] = this->LastPCoords[1];
  pcoords[2] = this->LastPCoords[2];

  return 1;
}

//------------------------------------------------------------------------------
void vtkAbstractInterpolatedVelocityField::FastCompute(vtkDataArray* vectors, double f[3])
{
  int pntIdx;
  int numPts = this->GenCell->GetNumberOfPoints();
  double vector[3];
  f[0] = f[1] = f[2] = 0.0;

  for (int i = 0; i < numPts; i++)
  {
    pntIdx = this->GenCell->PointIds->GetId(i);
    vectors->GetTuple(pntIdx, vector);
    f[0] += vector[0] * this->Weights[i];
    f[1] += vector[1] * this->Weights[i];
    f[2] += vector[2] * this->Weights[i];
  }
}

//------------------------------------------------------------------------------
bool vtkAbstractInterpolatedVelocityField::InterpolatePoint(vtkPointData* outPD, vtkIdType outIndex)
{
  if (!this->LastDataSet)
  {
    return false;
  }

  outPD->InterpolatePoint(
    this->LastDataSet->GetPointData(), outIndex, this->GenCell->PointIds, this->Weights);
  return true;
}

//------------------------------------------------------------------------------
void vtkAbstractInterpolatedVelocityField::CopyParameters(
  vtkAbstractInterpolatedVelocityField* from)
{
  this->Caching = from->Caching;
  this->SetFindCellStrategy(from->GetFindCellStrategy());
  this->NormalizeVector = from->NormalizeVector;
  this->ForceSurfaceTangentVector = from->ForceSurfaceTangentVector;
  this->SurfaceDataset = from->SurfaceDataset;
  this->VectorsType = from->VectorsType;
  this->SetVectorsSelection(from->VectorsSelection);

  // Copy the function cache, including possibly strategies, from the
  // prototype. In a threaded situation, there must be separate strategies
  // for each interpolated velocity field.
  this->InitializationState = from->InitializationState;
  this->FunctionCacheMap->clear();
  vtkFunctionCacheMap::iterator cIter = from->FunctionCacheMap->begin();
  for (; cIter != from->FunctionCacheMap->end(); ++cIter)
  {
    vtkFindCellStrategy* strategy = nullptr;
    if (cIter->second.Strategy != nullptr)
    {
      strategy = cIter->second.Strategy->NewInstance();
      strategy->CopyParameters(cIter->second.Strategy);
      strategy->Initialize(static_cast<vtkPointSet*>(cIter->first));
    }
    vtkDataArray* vectors = cIter->second.Vectors;
    this->FunctionCacheMap->insert(
      std::make_pair(cIter->first, vtkFunctionCache(strategy, vectors)));
  }
}

//------------------------------------------------------------------------------
void vtkAbstractInterpolatedVelocityField::AddToFunctionCache(
  vtkDataObject* ds, vtkFindCellStrategy* s, vtkDataArray* vectors)
{
  this->FunctionCacheMap->insert(std::make_pair(ds, vtkFunctionCache(s, vectors)));
}

//------------------------------------------------------------------------------
size_t vtkAbstractInterpolatedVelocityField::GetFunctionCacheSize()
{
  return this->FunctionCacheMap->size();
}

//------------------------------------------------------------------------------
void vtkAbstractInterpolatedVelocityField::SelectVectors(int associationType, const char* fieldName)
{
  this->VectorsType = associationType;
  this->SetVectorsSelection(fieldName);
}

//------------------------------------------------------------------------------
void vtkAbstractInterpolatedVelocityField::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent
     << "VectorsSelection: " << (this->VectorsSelection ? this->VectorsSelection : "(none)")
     << endl;
  os << indent << "NormalizeVector: " << (this->NormalizeVector ? "on." : "off.") << endl;
  os << indent
     << "ForceSurfaceTangentVector: " << (this->ForceSurfaceTangentVector ? "on." : "off.") << endl;
  os << indent << "SurfaceDataset: " << (this->SurfaceDataset ? "on." : "off.") << endl;

  os << indent << "Caching Status: " << (this->Caching ? "on." : "off.") << endl;
  os << indent << "Cache Hit: " << this->CacheHit << endl;
  os << indent << "Cache Miss: " << this->CacheMiss << endl;
  os << indent << "Weights Size: " << this->WeightsSize << endl;

  os << indent << "Last Dataset: " << this->LastDataSet << endl;
  os << indent << "Last Cell Id: " << this->LastCellId << endl;
  os << indent << "Last Cell: " << this->Cell << endl;
  os << indent << "Current Cell: " << this->GenCell << endl;
  os << indent << "Last P-Coords: " << this->LastPCoords[0] << ", " << this->LastPCoords[1] << ", "
     << this->LastPCoords[2] << endl;
  os << indent << "Last Weights: " << this->Weights << endl;

  os << indent << "FindCell Strategy: " << this->FindCellStrategy << endl;
}
VTK_ABI_NAMESPACE_END
