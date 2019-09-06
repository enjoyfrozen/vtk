/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkTestUtilities.h"
#include "vtkRegressionTestImage.h"

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkPolyDataMapper.h"
#include "vtkNew.h"
#include "vtkProperty.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkProteinRibbonFilter.h"
#include "vtkPDBReader.h"
#include "vtkInteractorStyleSwitch.h"

int TestProteinRibbon(int argc, char *argv[])
{
  char* fileName =
    vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/3GQP.pdb");

  // read protein from pdb
  vtkNew<vtkPDBReader> reader;
  reader->SetFileName(fileName);

  delete [] fileName;

  // setup ribbon filter
  vtkNew<vtkProteinRibbonFilter> ribbonFilter;
  ribbonFilter->SetInputConnection(reader->GetOutputPort());
  ribbonFilter->Update();

  // setup poly data mapper
  vtkNew<vtkPolyDataMapper> polyDataMapper;
  polyDataMapper->SetInputData(ribbonFilter->GetOutput());
  polyDataMapper->Update();

  // setup actor
  vtkNew<vtkActor> actor;
  actor->SetMapper(polyDataMapper);

  // setup render window
  vtkNew<vtkRenderer> ren;
  vtkNew<vtkRenderWindow> win;
  win->AddRenderer(ren);
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(win);
  vtkInteractorStyleSwitch* is = vtkInteractorStyleSwitch::SafeDownCast(iren->GetInteractorStyle());
  if (is)
  {
    is->SetCurrentStyleToTrackballCamera();
  }
  ren->AddActor(actor);
  ren->SetBackground(0,0,0);
  win->SetSize(450, 450);
  ren->ResetCamera();
  ren->GetActiveCamera()->Zoom(1.5);
  ren->ResetCameraClippingRange();
  win->Render();

  // Finally render the scene and compare the image to a reference image
  win->SetMultiSamples(0);
  win->GetInteractor()->Initialize();
  win->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
