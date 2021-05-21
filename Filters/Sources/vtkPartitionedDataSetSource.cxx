/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPartitionedDataSetSource.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPartitionedDataSetSource.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkLogger.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkParametricFunctionSource.h"
#include "vtkPartitionedDataSet.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkStringArray.h"

#include <algorithm>
#include <numeric>
#include <typeinfo>
#include <vector>

namespace
{

enum NUM_PARTITIONS : signed int
{
  MULTIPLE_PARTITIONS = -1,
  NO_PARTITIONS = 0
};

/*
 * Generate allocations for the given ranks taking in consideration that:
 *
 * - Some ranks might not accept any partitions
 * - Some ranks might accept a finite amount of partitions.
 * - Some ranks might accept a any multiplicty of partitions.
 *
 */
std::vector<int> GenerateAllocations(const std::vector<int>& allocs, const int numPartitions)
{
  std::vector<int> partsPerRank(allocs);

  const int partsAllocated = std::accumulate(allocs.begin(), allocs.end(), 0,
    [](int a, int b) { return (b == NUM_PARTITIONS::MULTIPLE_PARTITIONS) ? a : a + b; });

  const int ranksToAlloc =
    std::count(allocs.begin(), allocs.end(), NUM_PARTITIONS::MULTIPLE_PARTITIONS);

  if (ranksToAlloc)
  {
    const int partsToAlloc = std::max(numPartitions - partsAllocated, 0);
    const int partsToAllocPerRank = partsToAlloc / ranksToAlloc;
    const int partsToAllocLastRank = partsToAlloc % ranksToAlloc;
    int lastRankToAlloc = 0;

    for (int i = 0; i < allocs.size(); i++)
    {
      if (allocs[i] == NUM_PARTITIONS::MULTIPLE_PARTITIONS)
      {
        partsPerRank[i] = partsToAllocPerRank;
        lastRankToAlloc = i;
      }
    }

    if (partsToAllocLastRank)
    {
      partsPerRank[lastRankToAlloc] = partsToAllocLastRank;
    }
  }

  assert(std::accumulate(partsPerRank.begin(), partsPerRank.end(), 0) == numPartitions);

  return partsPerRank;
}

// returns [start, end].
std::pair<int, int> GetRange(const int rank, const std::vector<int>& parts)
{
  std::pair<int, int> result(0, parts[0]);
  if (rank == 0)
  {
    return result;
  }
  for (int cc = 1; cc <= rank; ++cc)
  {
    result.first = result.second;
    result.second += parts[cc];
  }
  return result;
}

}

vtkStandardNewMacro(vtkPartitionedDataSetSource);

//----------------------------------------------------------------------------
vtkPartitionedDataSetSource::vtkPartitionedDataSetSource()
{
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
}

//----------------------------------------------------------------------------
vtkPartitionedDataSetSource::~vtkPartitionedDataSetSource() = default;

//----------------------------------------------------------------------------
void vtkPartitionedDataSetSource::SetParametricFunction(
  vtkSmartPointer<vtkParametricFunction> param)
{
  this->ParametricFunction = param;
}

//----------------------------------------------------------------------------
void vtkPartitionedDataSetSource::EnableRank(int rank)
{
  this->Allocations[rank] = NUM_PARTITIONS::MULTIPLE_PARTITIONS;
}

//----------------------------------------------------------------------------
void vtkPartitionedDataSetSource::DisableRank(int rank)
{
  this->Allocations[rank] = NUM_PARTITIONS::NO_PARTITIONS;
}

//----------------------------------------------------------------------------
bool vtkPartitionedDataSetSource::IsEnabledRank(int rank)
{
  // Unless set all ranks are enabled
  if (this->Allocations.find(rank) == this->Allocations.end())
  {
    return true;
  }

  return this->Allocations[rank] == NUM_PARTITIONS::MULTIPLE_PARTITIONS;
}

//----------------------------------------------------------------------------
void vtkPartitionedDataSetSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "ParametricFunction: " << typeid(ParametricFunction).name() << endl;
}

//------------------------------------------------------------------------------
int vtkPartitionedDataSetSource::RequestInformation(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* outputVector)
{
  auto outInfo = outputVector->GetInformationObject(0);
  outInfo->Set(CAN_HANDLE_PIECE_REQUEST(), 1);
  return 1;
}

//----------------------------------------------------------------------------
int vtkPartitionedDataSetSource::RequestData(
  vtkInformation*, vtkInformationVector**, vtkInformationVector* outputVector)
{
  auto outInfo = outputVector->GetInformationObject(0);
  auto pds = vtkPartitionedDataSet::GetData(outInfo);
  const int rank = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER());
  const int numRanks = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES());

  vtkNew<vtkParametricFunctionSource> source;
  source->SetOutputPointsPrecision(vtkAlgorithm::DOUBLE_PRECISION);

  vtkSmartPointer<vtkParametricFunction> function{ this->ParametricFunction };

  function->JoinVOff();
  function->JoinUOff();

  source->SetParametricFunction(function);
  source->SetScalarModeToV();

  // By default we want to split partititions evenly in our ranks
  std::vector<int> allocs(numRanks, NUM_PARTITIONS::MULTIPLE_PARTITIONS);
  for (auto& kv : this->Allocations)
  {
    if (kv.first < allocs.size())
    {
      allocs[kv.first] = kv.second;
    }
  }

  const int numberOfPartitions = (this->NumberOfPartitions > 0)
    ? this->NumberOfPartitions
    : std::count(allocs.begin(), allocs.end(), NUM_PARTITIONS::MULTIPLE_PARTITIONS);

  auto partsPerRank = ::GenerateAllocations(allocs, numberOfPartitions);
  const auto range = ::GetRange(rank, partsPerRank);

  const double deltaV = function->GetMaximumV() / numberOfPartitions;
  for (int idx = 0, partition = range.first; partition < range.second; ++partition, ++idx)
  {
    function->SetMinimumV(partition * deltaV);
    function->SetMaximumV((partition + 1) * deltaV);
    vtkLogF(TRACE, "min=%f max=%f", function->GetMinimumV(), function->GetMaximumV());

    source->Update();

    vtkNew<vtkPolyData> clone;
    clone->ShallowCopy(source->GetOutputDataObject(0));

    vtkNew<vtkIntArray> partId;
    partId->SetName("PartitionId");
    partId->SetNumberOfTuples(clone->GetNumberOfPoints());
    partId->FillValue(partition);
    clone->GetPointData()->AddArray(partId);

    vtkNew<vtkStringArray> objectId;
    objectId->SetName("ObjectId");
    objectId->SetNumberOfTuples(clone->GetNumberOfPoints());
    for (int i = 0; i < clone->GetNumberOfPoints(); i++)
    {
      objectId->InsertNextValue(typeid(ParametricFunction).name());
    }
    clone->GetPointData()->AddArray(objectId);

    pds->SetPartition(idx, clone);
  }

  return 1;
}
