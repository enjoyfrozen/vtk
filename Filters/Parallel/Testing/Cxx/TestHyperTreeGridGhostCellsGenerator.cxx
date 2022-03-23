#include "vtkCellData.h"
#include "vtkHyperTreeGrid.h"
#include "vtkHyperTreeGridGhostCellsGenerator.h"
#include "vtkMPIController.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkRandomHyperTreeGridSource.h"
#include "vtkUnsignedCharArray.h"

#include "vtkLogger.h"

// Program main
int TestHyperTreeGridGhostCellsGenerator(int argc, char* argv[])
{
  int ret = EXIT_SUCCESS;
  // Initialize MPI
  vtkNew<vtkMPIController> controller;
  controller->Initialize(&argc, &argv, 0);
  vtkMultiProcessController::SetGlobalController(controller);
  int myRank = controller->GetLocalProcessId();
  int nbRanks = controller->GetNumberOfProcesses();

  // Initialize log
  std::string threadName = "rank #";
  threadName += std::to_string(myRank);
  vtkLogger::SetThreadName(threadName);
  // vtkLogger::SetStderrVerbosity(vtkLogger::VERBOSITY_TRACE);

  // Setup pipeline
  vtkNew<vtkRandomHyperTreeGridSource> htgSource;
  htgSource->SetMaxDepth(3);
  htgSource->SetDimensions(3, 3, 3);
  htgSource->UpdatePiece(myRank, nbRanks, 0);
  vtkLog(TRACE, << "number of cells (before Generator): "
                << htgSource->GetHyperTreeGridOutput()->GetNumberOfCells());

  vtkNew<vtkHyperTreeGridGhostCellsGenerator> generator;
  generator->SetInputConnection(htgSource->GetOutputPort());
  vtkHyperTreeGrid* htg = generator->GetHyperTreeGridOutput();
  generator->UpdatePiece(myRank, nbRanks, 0);
  vtkLog(TRACE, << "number of cells (after Generator): " << htg->GetNumberOfCells());

  // every piece should have some ghosts
  if (!(htg->HasAnyGhostCells() && htg->GetGhostCells()->GetNumberOfTuples() > 1))
  {
    ret = EXIT_FAILURE;
  }

  controller->Finalize();
  return ret;
}
