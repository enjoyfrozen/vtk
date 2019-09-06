/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include <mpi.h>

#include "vtkEdgeListIterator.h"
#include "vtkGraphLayoutView.h"
#include "vtkInEdgeIterator.h"
#include "vtkInformation.h"
#include "vtkMPIController.h"
#include "vtkPBGLCollectGraph.h"
#include "vtkPBGLDistributedGraphHelper.h"
#include "vtkPBGLBreadthFirstSearch.h"
#include "vtkPBGLRandomGraphSource.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkUndirectedGraph.h"
#include "vtkViewTheme.h"

int main(int argc, char** argv)
{
  MPI_Init(&argc, &argv);

  vtkSmartPointer<vtkPBGLRandomGraphSource> source =
    vtkSmartPointer<vtkPBGLRandomGraphSource>::New();
  source->DirectedOff();
  source->SetNumberOfVertices(100000);
  source->SetNumberOfEdges(10000);
  source->StartWithTreeOn();
  vtkSmartPointer<vtkPBGLBreadthFirstSearch> bfs =
    vtkSmartPointer<vtkPBGLBreadthFirstSearch>::New();
  bfs->SetInputConnection(source->GetOutputPort());
  vtkSmartPointer<vtkPBGLCollectGraph> collect =
    vtkSmartPointer<vtkPBGLCollectGraph>::New();
  collect->SetInputConnection(bfs->GetOutputPort());

  // Setup pipeline request
  vtkSmartPointer<vtkMPIController> controller =
    vtkSmartPointer<vtkMPIController>::New();
  controller->Initialize(&argc, &argv, 1);
  int rank = controller->GetLocalProcessId();
  int procs = controller->GetNumberOfProcesses();
  collect->Update(rank, procs, 0);

  if (rank == 0)
  {
    vtkSmartPointer<vtkUndirectedGraph> g =
      vtkSmartPointer<vtkUndirectedGraph>::New();
    g->ShallowCopy(collect->GetOutput());
    vtkSmartPointer<vtkGraphLayoutView> view =
      vtkSmartPointer<vtkGraphLayoutView>::New();
    vtkSmartPointer<vtkViewTheme> theme;
    theme.TakeReference(vtkViewTheme::CreateMellowTheme());
    view->ApplyViewTheme(theme);
    view->SetRepresentationFromInput(g);
    view->SetVertexColorArrayName("BFS");
    view->ColorVerticesOn();
    vtkRenderWindow* win = view->GetRenderWindow();
    view->Update();
    view->GetRenderer()->ResetCamera();
    win->GetInteractor()->Initialize();
    win->GetInteractor()->Start();
  }

  controller->Finalize();
  return 0;
}
