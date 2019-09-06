/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkTestUtilities.h"
#include "vtkRegressionTestImage.h"

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkCMLMoleculeReader.h"
#include "vtkMolecule.h"
#include "vtkMoleculeMapper.h"
#include "vtkNew.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"

int TestCMLMoleculeReader(int argc, char *argv[])
{
  char* fname = vtkTestUtilities::ExpandDataFileName(
      argc, argv, "Data/porphyrin.cml");

  vtkNew<vtkCMLMoleculeReader> cmlSource;

  cmlSource->SetFileName(fname);

  delete [] fname;

  vtkNew<vtkMoleculeMapper> molmapper;
  molmapper->SetInputConnection(cmlSource->GetOutputPort());

  molmapper->UseBallAndStickSettings();

  vtkNew<vtkActor> actor;
  actor->SetMapper(molmapper);

  vtkNew<vtkRenderer> ren;
  vtkNew<vtkRenderWindow> win;
  win->AddRenderer(ren);
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(win);

  ren->AddActor(actor);

  ren->SetBackground(0.0,0.0,0.0);
  win->SetSize(450,450);
  win->Render();
  ren->GetActiveCamera()->Zoom(2.0);

  // Finally render the scene and compare the image to a reference image
  win->SetMultiSamples(0);
  win->GetInteractor()->Initialize();
  win->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
