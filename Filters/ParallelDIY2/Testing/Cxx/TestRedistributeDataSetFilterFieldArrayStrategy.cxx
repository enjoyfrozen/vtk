// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkCellData.h"
#include "vtkCompositePolyDataMapper.h"
#include "vtkCompositeRenderManager.h"
#include "vtkDataSetSurfaceFilter.h"
#include "vtkExodusIIReader.h"
#include "vtkFieldData.h"
#include "vtkImageData.h"
#include "vtkLogger.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkNew.h"
#include "vtkPartitionedDataSet.h"
#include "vtkPartitionedDataSetCollection.h"
#include "vtkPointData.h"
#include "vtkProperty.h"
#include "vtkRTAnalyticSource.h"
#include "vtkRandomAttributeGenerator.h"
#include "vtkRedistributeDataSetFilter.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkTestUtilities.h"
#include "vtkUnstructuredGrid.h"
#include "vtkXMLPartitionedDataSetWriter.h"

#if VTK_MODULE_ENABLE_VTK_ParallelMPI
#include "vtkMPIController.h"
#else
#include "vtkDummyController.h"
#endif

// clang-format off
#include "vtk_diy2.h"
#include "vtkFieldPartitioningStrategy.h"
#include VTK_DIY2(diy/mpi.hpp)
// clang-format on

namespace
{
int targetNbOfPartitions = 5;

bool ValidateDataset(
  vtkUnstructuredGrid* input, int nbPartitionTotal, vtkMultiProcessController* controller)
{
  if (nbPartitionTotal != targetNbOfPartitions)
  {
    vtkLogF(ERROR, "Incorrect number of partitions in output expected %d, actual %d",
      targetNbOfPartitions, nbPartitionTotal);
    return false;
  }
  return true;
}
}

int TestRedistributeDataSetFilterFieldArrayStrategy(int argc, char* argv[])
{
#if VTK_MODULE_ENABLE_VTK_ParallelMPI
  vtkNew<vtkMPIController> controller;
#else
  vtkNew<vtkDummyController> controller;
#endif
  controller->Initialize(&argc, &argv);
  vtkMultiProcessController::SetGlobalController(controller);
  const int rank = controller->GetLocalProcessId();
  const int numProcs = controller->GetNumberOfProcesses();
  vtkLogger::SetThreadName("rank:" + std::to_string(rank));

  vtkNew<vtkUnstructuredGrid> data;
  char* fname = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/disk_out_ref.ex2");
  if (!fname)
  {
    vtkLogF(ERROR, "Could not obtain filename for test data.");
    return EXIT_FAILURE;
  }

  vtkNew<vtkExodusIIReader> rdr;
  if (!rdr->CanReadFile(fname))
  {
    vtkLogF(ERROR, "Cannot read `%s`", fname);
    return 1;
  }

  rdr->SetFileName(fname);
  delete[] fname;
  rdr->Update();

  vtkNew<vtkRandomAttributeGenerator> rag;
  rag->SetDataTypeToInt();
  rag->SetNumberOfComponents(1);
  rag->SetComponentRange(0, targetNbOfPartitions);
  rag->GenerateCellScalarsOn();
  /* Data is of the following structure
   * -----------------------------------
   * | Index |     Type    |
   * |-------|-------------|
   * | root  |~Multiblock  |
   * | 0     |~~Multiblock |
   * | 0.0   |~~~PolyData  |
   * | 1     |~~DataSet    |
   * |...    |...          |
   * -----------------------------------
   */
  rag->SetInputData(vtkUnstructuredGrid::SafeDownCast(
    vtkMultiBlockDataSet::SafeDownCast(rdr->GetOutput()->GetBlock(0))->GetBlock(0)));
  rag->Update();

  data->ShallowCopy(rag->GetOutputDataObject(0));

  vtkNew<vtkFieldPartitioningStrategy> fieldStrategy;
  fieldStrategy->SetFieldArrayName("RandomCellScalars");
  vtkNew<vtkRedistributeDataSetFilter> rdsf;
  rdsf->SetStrategy(fieldStrategy);
  rdsf->SetNumberOfPartitions(-1);
  rdsf->SetInputData(data);
  rdsf->GenerateGlobalCellIdsOn();
  rdsf->PreservePartitionsInOutputOn();
  rdsf->UpdatePiece(rank, numProcs, 0);
  int nbParttionLocal =
    vtkPartitionedDataSet::SafeDownCast(rdsf->GetOutputDataObject(0))->GetNumberOfPartitions();
  int nbPartitionTotal = 0;
  // Reduce the total number of partitions accross ranks
  controller->Reduce(
    &nbParttionLocal, &nbPartitionTotal, 1, vtkCommunicator::StandardOperations::SUM_OP, 0);
  int retVal = 1;
  if (rank == 0)
  {
    if (!ValidateDataset(data, nbPartitionTotal, controller))
    {
      retVal = 0;
    }
    controller->TriggerBreakRMIs();
  }
  controller->Broadcast(&retVal, 1, 0);
  controller->Finalize();
  vtkMultiProcessController::SetGlobalController(nullptr);
  return retVal ? EXIT_SUCCESS : EXIT_FAILURE;
}
