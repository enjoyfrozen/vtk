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
void ShallowCopy(vtkPartitionedDataSet* inputPDS, vtkPartitionedDataSet* outputPDS,
  int numberOfNonNullPartitionsInInput, int offset = 0)
{
  for (int outPartitionId = 0, inPartitionId = 0; outPartitionId < numberOfNonNullPartitionsInInput;
       ++inPartitionId, ++outPartitionId)
  {
    vtkDataObject* inputDO = inputPDS->GetPartitionAsDataObject(inPartitionId);

    while (!inputDO)
    {
      ++inPartitionId;
      inputDO = inputPDS->GetPartitionAsDataObject(inPartitionId);
    }

    outputPDS->SetPartition(outPartitionId + offset, inputDO);
  }
}
} // anonymous namespace

//----------------------------------------------------------------------------
vtkPartitionBalancer::vtkPartitionBalancer()
  : Controller(nullptr)
  , Mode(Squash)
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
  return 1;
}

//----------------------------------------------------------------------------
int vtkPartitionBalancer::RequestData(
  vtkInformation*, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkDataObject* inputDO = vtkDataObject::GetData(inputVector[0], 0);
  vtkDataObject* outputDO = vtkDataObject::GetData(outputVector, 0);

  auto inputPDS = vtkPartitionedDataSet::SafeDownCast(inputDO);
  auto outputPDS = vtkPartitionedDataSet::SafeDownCast(outputDO);

  int numberOfNonNullPartitionsInInput = 0;

  for (unsigned int partitionId = 0; partitionId < inputPDS->GetNumberOfPartitions(); ++partitionId)
  {
    numberOfNonNullPartitionsInInput += (inputPDS->GetPartition(partitionId) != nullptr);
  }

  int numberOfPartitions = 0;
  for (unsigned int partitionId = 0; partitionId < inputPDS->GetNumberOfPartitions(); ++partitionId)
  {
    numberOfPartitions += (inputPDS->GetPartition(partitionId) != nullptr);
  }

  std::vector<int> recvBuf(this->Controller->GetNumberOfProcesses());

  this->Controller->AllGather(&numberOfNonNullPartitionsInInput, recvBuf.data(), 1);

  int localProcessId = this->Controller->GetLocalProcessId();
  numberOfPartitions = 0;

  if (this->Mode == Mode::Expand)
  {
    int offset = 0;
    for (int processId = 0; processId < static_cast<int>(recvBuf.size()); ++processId)
    {
      int n = recvBuf[processId];

      if (processId == localProcessId)
      {
        offset = numberOfPartitions;
      }
      numberOfPartitions += n;
    }

    outputPDS->SetNumberOfPartitions(numberOfPartitions);

    for (unsigned int partitionId = 0; partitionId < outputPDS->GetNumberOfPartitions();
         ++partitionId)
    {
      outputPDS->SetPartition(partitionId, nullptr);
    }

    ShallowCopy(inputPDS, outputPDS, numberOfNonNullPartitionsInInput, offset);
  }
  else if (this->Mode == Mode::Squash)
  {
    for (int processId = 0; processId < static_cast<int>(recvBuf.size()); ++processId)
    {
      int n = recvBuf[processId];

      numberOfPartitions = std::max(numberOfPartitions, n);
    }

    outputPDS->SetNumberOfPartitions(numberOfPartitions);

    for (unsigned int partitionId = numberOfNonNullPartitionsInInput;
         partitionId < outputPDS->GetNumberOfPartitions(); ++partitionId)
    {
      outputPDS->SetPartition(partitionId, nullptr);
    }

    ShallowCopy(inputPDS, outputPDS, numberOfNonNullPartitionsInInput);
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
