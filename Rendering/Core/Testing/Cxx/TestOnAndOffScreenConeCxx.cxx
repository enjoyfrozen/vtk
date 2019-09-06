/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
// This test covers offscreen rendering.
//
// The command line arguments are:
// -I        => run in interactive mode; unless this is used, the program will
//              not allow interaction and exit

#include "vtkTestUtilities.h"
#include "vtkRegressionTestImage.h"

#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkConeSource.h"

int TestOnAndOffScreenConeCxx(int argc, char* argv[])
{
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  //renWin->SetShowWindow(false);
  //renWin->OffScreenRenderingOn();
  renWin->SetMultiSamples(0);

  vtkRenderer *renderer = vtkRenderer::New();
  renWin->AddRenderer(renderer);
  renderer->Delete();

  vtkConeSource *cone=vtkConeSource::New();
  vtkPolyDataMapper *mapper=vtkPolyDataMapper::New();
  mapper->SetInputConnection(cone->GetOutputPort());
  cone->Delete();

  vtkActor *actor=vtkActor::New();
  actor->SetMapper(mapper);
  mapper->Delete();

  renderer->AddActor(actor);
  actor->Delete();

  renderer->SetBackground(0.2,0.3,0.4);
  renWin->Render();
  renWin->SetShowWindow(false);
  renWin->SetUseOffScreenBuffers(true);
  renderer->SetBackground(0,0,0);

  renWin->Render();
  renWin->Render();
  renWin->Render();
  renWin->Render();

#if 0
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);
  renWin->Delete();

  renWin->Render();

  int retVal = vtkRegressionTestImage( renWin );
  if ( retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    iren->Start();
  }

  // Cleanup
  iren->Delete();
#else // the interactor version fails with OSMesa.
  renWin->Render();
  int retVal = vtkRegressionTestImage( renWin );
  renWin->Delete();
#endif
  return !retVal;
}
