/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
#include "vtkmCleanGrid.h"

#include "vtkActor.h"
#include "vtkGeometryFilter.h"
#include "vtkNew.h"
#include "vtkPolyDataMapper.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRTAnalyticSource.h"


int TestVTKMCleanGrid(int argc, char *argv[])
{
  vtkNew<vtkRTAnalyticSource> wavelet;
  wavelet->SetWholeExtent(-10, 10, -10, 10, -10, 10);
  wavelet->SetCenter(0, 0, 0);

  vtkNew<vtkmCleanGrid> cleanGrid;
  cleanGrid->SetInputConnection(wavelet->GetOutputPort());

  vtkNew<vtkGeometryFilter> geometry;
  geometry->SetInputConnection(cleanGrid->GetOutputPort());

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(geometry->GetOutputPort());
  mapper->SetScalarRange(37, 277);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->ResetCamera();

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);
  iren->Initialize();

  renWin->Render();
  int retVal = vtkRegressionTestImage(renWin);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    iren->Start();
  }

  return !retVal;
}
