/*=========================================================================
      </ProxyProperty>

  Program:   Visualization Toolkit
  Module:    vtkPartitionBalancer.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkPartitionBalancer.h"

#include "vtkCommunicator.h"
#include "vtkCompositeDataSet.h"
#include "vtkDataObject.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMultiProcessController.h"
#include "vtkMultiProcessStream.h"
#include "vtkObjectFactory.h"
#include "vtkPartitionedDataSet.h"
#include "vtkPartitionedDataSetCollection.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include <vector>

vtkStandardNewMacro(vtkPartitionBalancer);
vtkCxxSetObjectMacro(vtkPartitionBalancer, Controller, vtkMultiProcessController);

namespace
{
//------------------------------------------------------------------------------
void ShallowCopy(vtkPartitionedDataSet* inPDS, vtkPartitionedDataSet* outPDS,
  int numberOfNonNullPartitionsInInput, int offset = 0)
{
  for (int outPartitionId = 0, inPartitionId = 0; outPartitionId < numberOfNonNullPartitionsInInput;
       ++inPartitionId, ++outPartitionId)
  {
    vtkDataObject* inputDO = inPDS->GetPartitionAsDataObject(inPartitionId);

    while (!inputDO)
    {
      ++inPartitionId;
      inputDO = inPDS->GetPartitionAsDataObject(inPartitionId);
    }

    vtkDataObject* outputDO = inputDO->NewInstance();
    outputDO->ShallowCopy(inputDO);
    outPDS->SetPartition(outPartitionId + offset, outputDO);
    outputDO->FastDelete();
  }
}
} // anonymous namespace

//----------------------------------------------------------------------------
vtkPartitionBalancer::vtkPartitionBalancer()
  : Controller(nullptr)
  : Mode(Mode::Squash)
{
  this->SetController(vtkMultiProcessController::GetGlobalController());
}

//----------------------------------------------------------------------------
vtkPartitionBalancer::~vtkPartitionBalancer()
{
  this->SetController(nullptr);
}

//------------------------------------------------------------------------------
int vtkPartitionBalancer::FillInputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPartitionedDataSet");
  info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPartitionedDataSetCollection");
  return 1;
}

//----------------------------------------------------------------------------
int vtkPartitionBalancer::RequestData(
  vtkInformation*, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkDataObject* inputDO = vtkDataObject::GetData(inputVector[0], 0);
  vtkDataObject* outputDO = vtkDataObject::GetData(outputVector, 0);

  std::vector<vtkPartitionedDataSet*> inputPDSs, outputPDSs;

  if (auto inputPDSC = vtkPartitionedDataSetCollection::SafeDownCast(inputDO))
  {
    auto outputPDSC = vtkPartitionedDataSetCollection::SafeDownCast(outputDO);
    outputPDSC->CopyStructure(inputPDSC);

    for (unsigned int pdsId = 0; pdsId < inputPDSC->GetNumberOfPartitionedDataSets(); ++pdsId)
    {
      inputPDSs.emplace_back(inputPDSC->GetPartitionedDataSet(pdsId));
      outputPDSs.emplace_back(outputPDSC->GetPartitionedDataSet(pdsId));
    }
  }
  else if (auto inPDS = vtkPartitionedDataSet::SafeDownCast(inputDO))
  {
    inputPDSs.emplace_back(inPDS);
    outputPDSs.emplace_back(vtkPartitionedDataSet::SafeDownCast(outputDO));
  }
  else
  {
    vtkErrorMacro("Wrong input type. vtkPartitionBalancer only accepts vtkPartitionedDataSet and"
      << " vtkPartitionedDataSetCollection as inputs. Aborting...");
    return 0;
  }

  std::vector<int> numberOfNonNullPartitionsInInput(inputPDSs.size(), 0);
  for (int pdsId = 0; pdsId < static_cast<int>(numberOfNonNullPartitionsInInput.size()); ++pdsId)
  {
    vtkPartitionedDataSet* inPDS = inputPDSs[pdsId];
    for (unsigned int partitionId = 0; partitionId < inPDS->GetNumberOfPartitions(); ++partitionId)
    {
      numberOfNonNullPartitionsInInput[pdsId] += (inPDS->GetPartition(partitionId) != nullptr);
    }
  }

  vtkMultiProcessStream sendBuf;
  for (int pdsId = 0; pdsId < static_cast<int>(inputPDSs.size()); ++pdsId)
  {
    int numberOfPartitions = 0;
    vtkPartitionedDataSet* pds = inputPDSs[pdsId];
    for (unsigned int partitionId = 0; partitionId < pds->GetNumberOfPartitions(); ++partitionId)
    {
      numberOfPartitions += (pds->GetPartition(partitionId) != nullptr);
    }
    sendBuf << numberOfNonNullPartitionsInInput[pdsId];
  }

  std::vector<vtkMultiProcessStream> recvBuf;

  this->Controller->AllGather(sendBuf, recvBuf);

  int localProcessId = this->Controller->GetLocalProcessId();

  if (this->Mode == Mode::Expand)
  {
    std::vector<int> numberOfPartitions(inputPDSs.size(), 0);
    std::vector<int> offset(inputPDSs.size());
    for (int processId = 0; processId < static_cast<int>(recvBuf.size()); ++processId)
    {
      vtkMultiProcessStream& buf = recvBuf[processId];

      for (int pdsId = 0; pdsId < static_cast<int>(inputPDSs.size()); ++pdsId)
      {
        int n;
        buf >> n;

        if (processId == localProcessId)
        {
          offset[pdsId] = numberOfPartitions[pdsId];
        }
        numberOfPartitions[pdsId] += n;
      }
    }

    for (int pdsId = 0; pdsId < static_cast<int>(inputPDSs.size()); ++pdsId)
    {
      vtkPartitionedDataSet* inPDS = inputPDSs[pdsId];
      vtkPartitionedDataSet* outPDS = outputPDSs[pdsId];

      outPDS->SetNumberOfPartitions(numberOfPartitions[pdsId]);

      for (unsigned int partitionId = 0; partitionId < outPDS->GetNumberOfPartitions();
           ++partitionId)
      {
        outPDS->SetPartition(partitionId, nullptr);
      }

      ShallowCopy(inPDS, outPDS, numberOfNonNullPartitionsInInput[pdsId], offset[pdsId]);
    }
  }
  else if (this->Mode == Mode::Squash)
  {
    std::vector<int> numberOfPartitions(inputPDSs.size(), 0);

    for (int processId = 0; processId < static_cast<int>(recvBuf.size()); ++processId)
    {
      vtkMultiProcessStream& buf = recvBuf[processId];

      for (int pdsId = 0; pdsId < static_cast<int>(inputPDSs.size()); ++pdsId)
      {
        int n;
        buf >> n;

        numberOfPartitions[pdsId] = std::max(numberOfPartitions[pdsId], n);
      }
    }

    for (int pdsId = 0; pdsId < static_cast<int>(inputPDSs.size()); ++pdsId)
    {
      vtkPartitionedDataSet* inPDS = inputPDSs[pdsId];
      vtkPartitionedDataSet* outPDS = outputPDSs[pdsId];

      outPDS->SetNumberOfPartitions(numberOfPartitions[pdsId]);

      for (unsigned int partitionId = numberOfNonNullPartitionsInInput[pdsId];
           partitionId < outPDS->GetNumberOfPartitions(); ++partitionId)
      {
        outPDS->SetPartition(partitionId, nullptr);
      }

      ShallowCopy(inPDS, outPDS, numberOfNonNullPartitionsInInput[pdsId]);
    }
  }
  else
  {
    vtkErrorMacro(<< "Wrong value for Mode. It should be set to Mode::Expand or Mode::Squash."
                  << " Aborting...");
    return 0;
  }

  return 1;
}

//----------------------------------------------------------------------------
void vtkPartitionBalancer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Controller: " << this->Controller << endl;
  switch (this->Mode)
  {
    case Mode::Expand:
      os << indent << "Mode: Expand" << std::endl;
      break;
    case Mode::Squash:
      os << indent << "Mode: Squash" << std::endl;
      break;
    default:
      os << indent << "Mode: Wrong value" << std::endl;
      break;
  }
}
