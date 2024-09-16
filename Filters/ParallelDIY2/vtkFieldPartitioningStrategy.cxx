// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkFieldPartitioningStrategy.h"

#include "vtkArrayIterator.h"
#include "vtkArrayIteratorIncludes.h "
#include "vtkBoundingBox.h"
#include "vtkCellData.h"
#include "vtkDIYKdTreeUtilities.h"
#include "vtkDIYUtilities.h"
#include "vtkDataSetAttributes.h"
#include "vtkGenericCell.h"
#include "vtkIdTypeArray.h"
#include "vtkKdNode.h"
#include "vtkMultiProcessController.h"
#include "vtkObjectFactory.h"
#include "vtkPartitionedDataSet.h"
#include "vtkPartitionedDataSetCollection.h"
#include "vtkPartitioningStrategy.h"
#include "vtkRedistributeDataSetFilter.h"
#include "vtkSMPThreadLocal.h"
#include "vtkSMPThreadLocalObject.h"
#include "vtkSMPTools.h"
#include "vtkUnsignedCharArray.h"

namespace
{

/*
 * Fill the partition information from the region information
 */
vtkPartitioningStrategy::PartitionInformation RegionToPartition(
  vtkDataSet* dataset, std::map<vtkIdType, int>& regions, vtkDataArray* fieldArray)
{
  if (!dataset || dataset->GetNumberOfCells() == 0)
  {
    vtkWarningWithObjectMacro(nullptr, "Dataset is empty");
    return vtkPartitioningStrategy::PartitionInformation();
  }

  auto ghostCells = vtkUnsignedCharArray::SafeDownCast(
    dataset->GetCellData()->GetArray(vtkDataSetAttributes::GhostArrayName()));
  vtkPartitioningStrategy::PartitionInformation res;
  const auto numCells = dataset->GetNumberOfCells();
  res.TargetPartitions->Resize(numCells);
  vtkSMPTools::For(0, numCells, [&](vtkIdType first, vtkIdType last) {
    for (vtkIdType cellId = first; cellId < last; ++cellId)
    {
      if (ghostCells != nullptr &&
        ((ghostCells->GetTypedComponent(cellId, 0) & vtkDataSetAttributes::DUPLICATECELL) != 0))
      {
        // skip ghost cells, they will not be extracted since they will be
        // extracted on ranks where they are not marked as ghosts.
        continue;
      }
      res.TargetPartitions->SetTuple1(cellId, regions.at(fieldArray->GetTuple1(cellId)));
    }
  });
  res.NumberOfPartitions = regions.size();
  return res;
}

int GetTotalNumberOfPartition(vtkPartitionedDataSetCollection* collection)
{
  int totalNbPartition = 0;
  int nbDataSet = collection->GetNumberOfPartitionedDataSets();
  for (size_t dataSetIndex = 0; dataSetIndex < nbDataSet; dataSetIndex++)
  {
    totalNbPartition += collection->GetNumberOfPartitions(nbDataSet);
  }
  return totalNbPartition;
}
}

VTK_ABI_NAMESPACE_BEGIN
//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkFieldPartitioningStrategy);

//------------------------------------------------------------------------------
void vtkFieldPartitioningStrategy::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------------
std::vector<vtkPartitioningStrategy::PartitionInformation>
vtkFieldPartitioningStrategy::ComputePartition(vtkPartitionedDataSetCollection* collection)
{

  std::vector<PartitionInformation> res;
  if (!collection)
  {
    vtkErrorMacro("Collection is nullptr!");
    return res;
  }
  if (this->FieldArrayName.empty())
  {
    vtkErrorMacro(<< "No field array has been defined for the Field partition strategy, the "
                     "partitioning strategy can't be executed properly.");
  }
  int totalNbOfPartition = ::GetTotalNumberOfPartition(collection);
  res.reserve(totalNbOfPartition);
  for (unsigned int part = 0; part < collection->GetNumberOfPartitionedDataSets(); ++part)
  {

    auto inputPTD = collection->GetPartitionedDataSet(part);
    if (!inputPTD)
    {
      vtkWarningMacro("Found nullptr partitioned data set");
      continue;
    }
    for (unsigned int cc = 0; cc < inputPTD->GetNumberOfPartitions(); ++cc)
    {
      auto ds = inputPTD->GetPartition(cc);
      if (ds && (ds->GetNumberOfPoints() > 0 || ds->GetNumberOfCells() > 0))
      {
        auto fieldArray =
          ds->GetCellData() ? ds->GetCellData()->GetArray(this->FieldArrayName.c_str()) : nullptr;
        if (fieldArray == nullptr)
        {
          res.emplace_back(vtkPartitioningStrategy::PartitionInformation());
        }
        else
        {
          std::map<vtkIdType, int> regions{};
          this->GetFieldKeys(regions, ds);
          res.emplace_back(::RegionToPartition(ds, regions, fieldArray));
        }
      }
      else
      {
        res.emplace_back(vtkPartitioningStrategy::PartitionInformation());
      }
    }
    // Exchange the number of partitions in dataset accross ranks
    auto controller = this->GetController();
    if (controller && controller->GetNumberOfProcesses() > 1)
    {
      vtkIdType locsize = static_cast<vtkIdType>(res.size());
      vtkIdType allsize = 0;
      controller->AllReduce(&locsize, &allsize, 1, vtkCommunicator::MAX_OP);
      res.resize(allsize);
    }
  }
  // Exchange the total number of partitions in collection accross ranks
  auto controller = this->GetController();
  if (controller && controller->GetNumberOfProcesses() > 1)
  {
    vtkIdType allsize = res.size();
    std::vector<vtkIdType> nParts(allsize);
    std::transform(res.begin(), res.end(), nParts.begin(),
      [](PartitionInformation& info) { return info.NumberOfPartitions; });
    std::vector<vtkIdType> globNParts(allsize);
    controller->AllReduce(nParts.data(), globNParts.data(), allsize, vtkCommunicator::MAX_OP);
    for (vtkIdType iP = 0; iP < allsize; ++iP)
    {
      res[iP].NumberOfPartitions = globNParts[iP];
    }
  }

  return res;
}

//------------------------------------------------------------------------------
void vtkFieldPartitioningStrategy::GetFieldKeys(std::map<vtkIdType, int>& fieldKeys, vtkDataSet* dS)
{
  fieldKeys.clear();
  vtkSmartPointer<vtkDataArray> fieldArray =
    dS->GetCellData()->GetArray(this->FieldArrayName.c_str());
  if (fieldArray == nullptr)
  {
    return;
  }
  else
  {
    int nbOfregions = 0;
    auto nbTuple = fieldArray->GetNumberOfTuples();
    for (int index = 0; index < nbTuple; index++)
    {
      if (fieldKeys.find(fieldArray->GetTuple1(index)) == fieldKeys.end())
      {
        fieldKeys.emplace(fieldArray->GetTuple1(index), nbOfregions);
        nbOfregions += 1;
      }
    }
  }
}

//------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
