/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestDistancePolyDataFilter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

    This software is distributed WITHOUT ANY WARRANTY; without even
    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
    PURPOSE.  See the above copyright notice for more information.
=========================================================================*/

#include "vtkImageData.h"
#include "vtkMPIController.h"
#include "vtkMultiBlockDataGroupFilter.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkNew.h"
#include "vtkPOutlineFilter.h"
#include "vtkRTAnalyticSource.h"

#include <vtk_mpi.h>

#include <iostream>
int TestPOutlineFilter(int argc, char* argv[])
{
  // This is here to avoid false leak messages from vtkDebugLeaks when
  // using mpich. It appears that the root process which spawns all the
  // main processes waits in MPI_Init() and calls exit() when
  // the others are done, causing apparent memory leaks for any objects
  // created before MPI_Init().
  MPI_Init(&argc, &argv);

  // Note that this will create a vtkMPIController if MPI
  // is configured, vtkThreadedController otherwise.
  vtkMPIController* contr = vtkMPIController::New();
  contr->Initialize(&argc, &argv, 1);

  int retVal = EXIT_SUCCESS;

  vtkMultiProcessController::SetGlobalController(contr);

  int me = contr->GetLocalProcessId();

  if (!contr->IsA("vtkMPIController"))
  {
    if (me == 0)
    {
      cout << "PCellDataToPointData test requires MPI" << endl;
    }
    contr->Delete();
    return EXIT_FAILURE;
  }

  int numProcs = contr->GetNumberOfProcesses();

  int localExtent[6] = { (me - 1) * 10, me * 10, -10, 10, -10, 10 };

  vtkNew<vtkRTAnalyticSource> wavelet;
  wavelet->UpdatePiece(me, numProcs, 0, localExtent);

  vtkNew<vtkPOutlineFilter> outlineDS;
  outlineDS->SetController(vtkMultiProcessController::GetGlobalController());
  outlineDS->SetInputConnection(wavelet->GetOutputPort());
  outlineDS->Update();

  vtkNew<vtkMultiBlockDataGroupFilter> grouper;
  grouper->AddInputConnection(wavelet->GetOutputPort());

  vtkNew<vtkPOutlineFilter> outlineMB;
  outlineMB->SetController(vtkMultiProcessController::GetGlobalController());
  outlineMB->SetInputConnection(grouper->GetOutputPort());
  outlineMB->Update();

  if (me == 0)
  {
    int expectedNumberOfPoints = 8;
    int expectedNumberOfCells = 12;
    if (outlineDS->GetOutput()->GetNumberOfPoints() != expectedNumberOfPoints ||
      outlineMB->GetOutput()->GetNumberOfPoints() != expectedNumberOfPoints ||
      outlineDS->GetOutput()->GetNumberOfCells() != expectedNumberOfCells ||
      outlineMB->GetOutput()->GetNumberOfCells() != expectedNumberOfCells)
    {
      std::cerr << "ERROR: Unexpected number of points or cells" << std::endl;
      return EXIT_FAILURE;
    }
  }

  contr->Finalize();
  contr->Delete();

  return EXIT_SUCCESS;
}
