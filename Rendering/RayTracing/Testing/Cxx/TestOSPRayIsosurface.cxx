/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkTestUtilities.h"

#include "vtkColorTransferFunction.h"
#include "vtkContourValues.h"
#include "vtkOSPRayPass.h"
#include "vtkOSPRayVolumeMapper.h"
#include "vtkPiecewiseFunction.h"
#include "vtkRTAnalyticSource.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkVolumeProperty.h"

int TestOSPRayIsosurface(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  vtkNew<vtkRenderWindowInteractor> iren;
  vtkNew<vtkRenderWindow> renWin;
  iren->SetRenderWindow(renWin);
  vtkNew<vtkRenderer> renderer;
  renWin->AddRenderer(renderer);

  vtkNew<vtkRTAnalyticSource> wavelet;

  vtkNew<vtkOSPRayVolumeMapper> volumeMapper;
  volumeMapper->SetInputConnection(wavelet->GetOutputPort());
  volumeMapper->SetBlendModeToIsoSurface();

  vtkNew<vtkColorTransferFunction> colorTransferFunction;
  colorTransferFunction->AddRGBPoint(220.0, 0.0, 1.0, 0.0);
  colorTransferFunction->AddRGBPoint(150.0, 1.0, 1.0, 1.0);
  colorTransferFunction->AddRGBPoint(190.0, 0.0, 1.0, 1.0);

  vtkNew<vtkPiecewiseFunction> scalarOpacity;
  scalarOpacity->AddPoint(220.0, 1.0);
  scalarOpacity->AddPoint(150.0, 0.2);
  scalarOpacity->AddPoint(190.0, 0.6);

  vtkNew<vtkVolumeProperty> volumeProperty;
  volumeProperty->ShadeOn();
  volumeProperty->SetInterpolationTypeToLinear();
  volumeProperty->SetColor(colorTransferFunction);
  volumeProperty->SetScalarOpacity(scalarOpacity);
  volumeProperty->GetIsoSurfaceValues()->SetValue(0, 220.0);
  volumeProperty->GetIsoSurfaceValues()->SetValue(1, 150.0);
  volumeProperty->GetIsoSurfaceValues()->SetValue(2, 190.0);

  vtkNew<vtkVolume> volume;
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);

  renderer->AddVolume(volume);
  renWin->SetSize(400, 400);

  vtkNew<vtkOSPRayPass> ospray;
  renderer->SetPass(ospray);

  renWin->Render();

  iren->Start();
  return 0;
}
