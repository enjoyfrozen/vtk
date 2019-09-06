/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

// This test draws a sphere with the edges shown.  It also turns on coincident
// topology resolution with a z-shift to both make sure the wireframe is
// visible and to exercise that type of coincident topology resolution.

#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSphereSource.h"

#include "vtkSmartPointer.h"
#define VTK_CREATE(type, var) \
  vtkSmartPointer<type> var = vtkSmartPointer<type>::New()

int SurfacePlusEdges(int argc, char *argv[])
{
  vtkMapper::SetResolveCoincidentTopologyToShiftZBuffer();
  vtkMapper::SetResolveCoincidentTopologyZShift(0.1);

  VTK_CREATE(vtkSphereSource, sphere);

  VTK_CREATE(vtkPolyDataMapper, mapper);
  mapper->SetInputConnection(sphere->GetOutputPort());

  VTK_CREATE(vtkActor, actor);
  actor->SetMapper(mapper);
  actor->GetProperty()->EdgeVisibilityOn();
  actor->GetProperty()->SetEdgeColor(1.0, 0.0, 0.0);

  VTK_CREATE(vtkRenderer, renderer);
  renderer->AddActor(actor);
  renderer->ResetCamera();

  VTK_CREATE(vtkRenderWindow, renwin);
  renwin->AddRenderer(renderer);
  renwin->SetSize(250, 250);
  renwin->SetMultiSamples(0);

  int retVal = vtkRegressionTestImage(renwin);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    VTK_CREATE(vtkRenderWindowInteractor, iren);
    iren->SetRenderWindow(renwin);
    iren->Initialize();
    iren->Start();
    retVal = vtkRegressionTester::PASSED;
  }

  return (retVal == vtkRegressionTester::PASSED) ? 0 : 1;
}
