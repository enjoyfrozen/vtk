/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include <vtkEllipseArcSource.h>
#include <vtkPolyData.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

int TestEllipseArcSource(int vtkNotUsed(argc), char * vtkNotUsed(argv)[])
{
  vtkNew<vtkEllipseArcSource> source;
  source->SetCenter(0.0, 0.0, 0.0);
  source->SetRatio(0.25);
  source->SetNormal(0., 0., 1.);
  source->SetMajorRadiusVector(10, 0., 0.);
  source->SetStartAngle(20);
  source->SetSegmentAngle(250);
  source->SetResolution(80);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(source->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(.3, .6, .3);

  renderWindow->SetMultiSamples(0);
  renderWindow->Render();
  renderWindowInteractor->Start();
  return EXIT_SUCCESS;
}
