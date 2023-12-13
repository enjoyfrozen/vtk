// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkPHyperTreeGridProbeFilter.h"

#include "vtkAbstractArray.h"
#include "vtkCellData.h"
#include "vtkCharArray.h"
#include "vtkDataArray.h"
#include "vtkDataArrayRange.h"
#include "vtkDataSet.h"
#include "vtkFieldData.h"
#include "vtkHyperTreeGrid.h"
#include "vtkHyperTreeGridProbeFilterUtilities.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkMultiProcessController.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkSMPTools.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkStringArray.h"

#include <numeric>
#include <vector>

VTK_ABI_NAMESPACE_BEGIN

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkPHyperTreeGridProbeFilter);

//------------------------------------------------------------------------------
vtkCxxSetObjectMacro(vtkPHyperTreeGridProbeFilter, Controller, vtkMultiProcessController);

//------------------------------------------------------------------------------
vtkPHyperTreeGridProbeFilter::vtkPHyperTreeGridProbeFilter()
  : Controller(nullptr)
{
  this->SetController(vtkMultiProcessController::GetGlobalController());
}

//------------------------------------------------------------------------------
vtkPHyperTreeGridProbeFilter::~vtkPHyperTreeGridProbeFilter()
{
  this->SetController(nullptr);
}

//------------------------------------------------------------------------------
void vtkPHyperTreeGridProbeFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  this->GetController()->PrintSelf(os, indent.GetNextIndent());
}

//------------------------------------------------------------------------------
int vtkPHyperTreeGridProbeFilter::RequestUpdateExtent(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* vtkNotUsed(outputVector))
{
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* sourceInfo = inputVector[1]->GetInformationObject(0);

  inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER(), 0);
  inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES(), 1);
  inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_GHOST_LEVELS(), 0);

  sourceInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),
    sourceInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()), 6);
  return 1;
}

//------------------------------------------------------------------------------
int vtkHyperTreeGridProbeFilter::RequestData(
  vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  if (this->UseImplicitArrays)
  {
    vtkWarningMacro("UseImplicitArrays option is restricted for sequential version of the "
      << "vtkHyperTreeGridProbeFilter. For now, this option will be forced to false in the "
      << "case of a vtkPHyperTreeGridProbeFilter instance.");
    this->SetUseImplicitArrays(false);
  }

  this->Superclass::RequestData(request, inputVector, outputVector);
}

//------------------------------------------------------------------------------
bool vtkPHyperTreeGridProbeFilter::Reduce(
  vtkHyperTreeGrid* source, vtkDataSet* output, vtkIdList* localPointIds)
{
  int procId = 0;
  int numProcs = 1;
  if (this->Controller)
  {
    procId = this->Controller->GetLocalProcessId();
    numProcs = this->Controller->GetNumberOfProcesses();
  }

  vtkIdType numPointsFound = localPointIds->GetNumberOfIds();
  if (procId != 0)
  {
    this->Controller->Send(&numPointsFound, 1, 0, HYPERTREEGRID_PROBE_COMMUNICATION_TAG);
    if (this->UseImplicitArrays)
    {
      // Send size of the local source (HTG sub-extent)
      vtkIdType localSourceNbOfCells = source->GetNumberOfCells();
      this->Controller->Send(&localSourceNbOfCells, 1, 0, HYPERTREEGRID_PROBE_COMMUNICATION_TAG);
    }
    if (numPointsFound > 0)
    {
      if (this->UseImplicitArrays)
      {
        // Send handles + handles size
        this->Controller->Send(
          this->Handles->GetPointer(0), numPointsFound, 0, HYPERTREEGRID_PROBE_COMMUNICATION_TAG);
        this->Handles->Initialize();
      }
      else
      {
        // Send the whole output (carrying all point data arrays)
        this->Controller->Send(output, 0, HYPERTREEGRID_PROBE_COMMUNICATION_TAG);
      }
      this->Controller->Send(
        localPointIds->GetPointer(0), numPointsFound, 0, HYPERTREEGRID_PROBE_COMMUNICATION_TAG);
    }
    output->ReleaseData();
    localPointIds->Initialize();
  }
  else
  {
    if (this->UseImplicitArrays)
    {
      // Firsthand, cache the "final" handles and initialize them with default index
      // (default index = nb of cells in the source)
      // Secondhand, fill them with values at correct point ids using local copy and remote handles
      auto remoteHandles = vtk::TakeSmartPointer(this->Handles->NewInstance());
      remoteHandles->DeepCopy(this->Handles);
      this->Handles->SetNumberOfIds(output->GetNumberOfPoints());
      this->Handles->Fill(vtkHyperTreeGridProbeFilterUtilities::HANDLES_INVALID_ID);
      this->DealWithRemoteHandles(localPointIds, remoteHandles, this->Handles);

      // Gather the total number of cells in the source.
      // This will be used as index in the indexed array to point to the
      // NaN value, that should be necessaritly greater than the number of
      // cells in the total input source (to not point to an existing cell id)
      vtkIdType totalSourceNbOfCells = source->GetNumberOfCells();

      // Retrieve remote handles and deal with it
      vtkIdType numRemotePoints = 0;
      vtkNew<vtkIdList> remotePointIds;
      if (numProcs > 1)
      {
        for (int iProc = 1; iProc < numProcs; iProc++)
        {
          this->Controller->Receive(
            &numRemotePoints, 1, iProc, HYPERTREEGRID_PROBE_COMMUNICATION_TAG);

          // Retrieve number of cells in the remote source (HTG sub-extent on other nodes)
          vtkIdType remoteSourceNbOfCells = 0;
          this->Controller->Receive(
            &remoteSourceNbOfCells, 1, iProc, HYPERTREEGRID_PROBE_COMMUNICATION_TAG);

          totalSourceNbOfCells += remoteSourceNbOfCells;

          if (numRemotePoints > 0)
          {
            remoteHandles->Initialize();
            remoteHandles->SetNumberOfIds(numRemotePoints);
            this->Controller->Receive(remoteHandles->GetPointer(0), numRemotePoints, iProc,
              HYPERTREEGRID_PROBE_COMMUNICATION_TAG);

            remotePointIds->Initialize();
            remotePointIds->SetNumberOfIds(numRemotePoints);
            this->Controller->Receive(remotePointIds->GetPointer(0), numRemotePoints, iProc,
              HYPERTREEGRID_PROBE_COMMUNICATION_TAG);

            this->DealWithRemoteHandles(remotePointIds, remoteHandles, this->Handles);
          }
        }
      }
      this->Finalize(source, output, totalSourceNbOfCells);
    }
    else
    {
      // Cache each final output arrays, and initialize them with default values
      // Then fill them with values at correct point ids using local and remote data arrays
      vtkIdType numRemotePoints = 0;
      vtkSmartPointer<vtkDataSet> remoteOutput = vtk::TakeSmartPointer(output->NewInstance());
      vtkNew<vtkIdList> remotePointIds;
      // deal with master process
      remoteOutput->CopyStructure(output);
      unsigned int numArrays = source->GetCellData()->GetNumberOfArrays();
      for (unsigned int iA = 0; iA < numArrays; iA++)
      {
        vtkDataArray* da =
          output->GetPointData()->GetArray(source->GetCellData()->GetArray(iA)->GetName());
        auto localInstance = vtk::TakeSmartPointer(da->NewInstance());
        localInstance->DeepCopy(da);
        remoteOutput->GetPointData()->AddArray(localInstance);
        da->SetNumberOfTuples(output->GetNumberOfPoints());
        vtkHyperTreeGridProbeFilterUtilities::FillDefaultArray(da);
      }
      this->DealWithRemote(localPointIds, remoteOutput, source, output);
      remoteOutput->Initialize();

      // deal with other processes
      if (numProcs > 1)
      {
        for (int iProc = 1; iProc < numProcs; iProc++)
        {
          this->Controller->Receive(
            &numRemotePoints, 1, iProc, HYPERTREEGRID_PROBE_COMMUNICATION_TAG);
          remotePointIds->SetNumberOfIds(numRemotePoints);
          if (numRemotePoints > 0)
          {
            this->Controller->Receive(remoteOutput, iProc, HYPERTREEGRID_PROBE_COMMUNICATION_TAG);
            remotePointIds->Initialize();
            remotePointIds->SetNumberOfIds(numRemotePoints);
            this->Controller->Receive(remotePointIds->GetPointer(0), numRemotePoints, iProc,
              HYPERTREEGRID_PROBE_COMMUNICATION_TAG);
            this->DealWithRemote(remotePointIds, remoteOutput, source, output);
            remoteOutput->Initialize();
          }
        }
      }
    }
  }
  return true;
}

//------------------------------------------------------------------------------
void vtkPHyperTreeGridProbeFilter::SetUseImplicitArrays(bool vtkNotUsed(useImplicitArrays))
{
  vtkWarningMacro("UseImplicitArrays option is restricted for sequential version of the "
    << "vtkHyperTreeGridProbeFilter. For now, this option has no effect in the case of a"
    << "vtkPHyperTreeGridProbeFilter instance.");
}
VTK_ABI_NAMESPACE_END
