/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
//
// This example...
//

#include "vtkGraphLayoutView.h"
#include "vtkRandomGraphSource.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

int main(int, char*[])
{
  vtkRandomGraphSource* source = vtkRandomGraphSource::New();

  vtkGraphLayoutView* view = vtkGraphLayoutView::New();
  view->SetRepresentationFromInputConnection(
    source->GetOutputPort());

  view->ResetCamera();
  view->Render();
  view->GetInteractor()->Start();

  source->Delete();
  view->Delete();

  return 0;
}
