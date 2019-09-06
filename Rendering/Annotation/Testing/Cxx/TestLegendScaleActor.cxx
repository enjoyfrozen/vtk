/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
// This tests the terrain annotation capabilities in VTK.
#include "vtkLegendScaleActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkCamera.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkTestUtilities.h"
#include "vtkRegressionTestImage.h"

//----------------------------------------------------------------------------
int TestLegendScaleActor( int argc, char * argv [] )
{
  // Create the RenderWindow, Renderer and both Actors
  //
  vtkRenderer *ren1 = vtkRenderer::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->SetMultiSamples(0);
  renWin->AddRenderer(ren1);

  ren1->GetActiveCamera()->ParallelProjectionOn();

  vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);
  iren->SetInteractorStyle(style);

  // Create a test pipeline
  //
  vtkSphereSource *ss = vtkSphereSource::New();
  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInputConnection(ss->GetOutputPort());
  vtkActor *sph = vtkActor::New();
  sph->SetMapper(mapper);

  // Create the actor
  vtkLegendScaleActor *actor = vtkLegendScaleActor::New();
  actor->TopAxisVisibilityOn();

  // Add the actors to the renderer, set the background and size
  ren1->AddActor(sph);
  ren1->AddViewProp(actor);
  ren1->SetBackground(0.1, 0.2, 0.4);
  renWin->SetSize(300, 300);

  // render the image
  //
  iren->Initialize();
  renWin->Render();

  int retVal = vtkRegressionTestImage( renWin );
  if ( retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    iren->Start();
  }

  ss->Delete();
  mapper->Delete();
  sph->Delete();
  actor->Delete();
  style->Delete();
  iren->Delete();
  renWin->Delete();
  ren1->Delete();

  return !retVal;
}
