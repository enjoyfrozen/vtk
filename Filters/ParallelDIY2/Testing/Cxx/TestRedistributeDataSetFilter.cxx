#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkCompositePolyDataMapper.h"
#include "vtkDataSetSurfaceFilter.h"
#include "vtkExodusIIReader.h"
#include "vtkLogger.h"
#include "vtkMPIController.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkProperty.h"
#include "vtkRandomAttributeGenerator.h"
#include "vtkRedistributeDataSetFilter.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkTestUtilities.h"
#include "vtkUnstructuredGrid.h"

// clang-format off
#include "vtk_diy2.h"
#include VTK_DIY2(diy/mpi.hpp)
#include VTK_DIY2(diy/master.hpp)
// clang-format on

#include <algorithm>
int TestRedistributeDataSetFilter(int argc, char* argv[])
{
  diy::mpi::environment mpienv(argc, argv);
  diy::mpi::communicator world;
  vtkLogger::SetThreadName("rank:" + std::to_string(world.rank()));

  vtkNew<vtkMPIController> controller;
  controller->Initialize(&argc, &argv, /*initializedExternally=*/1);
  vtkMPIController::SetGlobalController(controller);

  if (world.size() != 1)
  {
    vtkLogF(ERROR, "this test is not expected to run on more than 1 rank.");
    return EXIT_FAILURE;
  }

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

  vtkUnstructuredGrid* data = vtkUnstructuredGrid::SafeDownCast(
    vtkMultiBlockDataSet::SafeDownCast(rdr->GetOutput()->GetBlock(0))->GetBlock(0));

  vtkNew<vtkRedistributeDataSetFilter> rdsf;
  rdsf->SetInputDataObject(data);
  rdsf->SetNumberOfPartitions(16);
  rdsf->GenerateGlobalCellIdsOn();
  rdsf->GenerateGlobalPointIdsOn();
  rdsf->PreservePartitionsInOutputOn();

  vtkNew<vtkDataSetSurfaceFilter> dsf;
  dsf->SetInputConnection(rdsf->GetOutputPort());

  vtkNew<vtkRandomAttributeGenerator> rag;
  rag->SetDataTypeToDouble();
  rag->SetNumberOfComponents(1);
  rag->SetComponentRange(0, 1.0);
  rag->GenerateCellScalarsOn();
  rag->AttributesConstantPerBlockOn();
  rag->SetInputConnection(dsf->GetOutputPort());

  vtkNew<vtkCompositePolyDataMapper> mapper;
  mapper->SetInputConnection(rag->GetOutputPort());

  vtkNew<vtkRenderWindow> renWin;
  renWin->SetSize(400, 400);

  vtkNew<vtkRenderer> renderer;
  renWin->AddRenderer(renderer);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  renderer->AddActor(actor);
  if (auto camera = renderer->GetActiveCamera())
  {
    camera->SetFocalPoint(-0.531007, -1.16954, -1.12284);
    camera->SetPosition(8.62765, 28.0586, -33.585);
    camera->SetViewUp(-0.373065, 0.739388, 0.560472);
  }

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);
  iren->Initialize();

  renWin->Render();

  int retVal = vtkRegressionTestImage(renWin);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    iren->Start();
  }
  controller->Finalize(/*finalizedExternally=*/1);
  vtkMPIController::SetGlobalController(nullptr);
  return !retVal;
}
