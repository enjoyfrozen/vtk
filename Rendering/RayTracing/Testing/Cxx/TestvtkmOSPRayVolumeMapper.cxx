/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestvtkmOSPRayVolumeMapper.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// Description
// This is a test for the point gaussian mapper in the ospray backend

#include "vtkColorTransferFunction.h"
#include "vtkNew.h"
#include "vtkOSPRayPass.h"
#include "vtkOSPRayRendererNode.h"
#include "vtkPiecewiseFunction.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartVolumeMapper.h"
#include "vtkTesting.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkmDataSet.h"

// vtk-m includes
#include <vtkm/cont/Initialize.h>
#include <vtkm/cont/testing/MakeTestDataSet.h>

#include "vtkOSPRayTestInteractor.h"

int TestvtkmOSPRayVolumeMapper(int argc, char* argv[])
{
  cout << "CTEST_FULL_OUTPUT (Avoid ctest truncation of output)" << endl;

  // Create the RenderWindow, Renderer and both Actors
  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(0.1, 0.2, 0.2);
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(300, 300);
  renderWindow->SetMultiSamples(0);
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renderWindow);
  vtkOSPRayRendererNode::SetSamplesPerPixel(16, renderer);

  // Initialize vtk-m backends
  vtkm::cont::Initialize(argc, argv);

  // Create the vtkmDataSet
  vtkm::cont::testing::MakeTestDataSet Maker;
  auto dataset = Maker.Make3DUniformDataSet1();
  vtkNew<vtkmDataSet> dsVtkm;
  dsVtkm->SetUseVtkmArrays(false);
  dsVtkm->SetVtkmDataSet(dataset);

  // Create the volume rendering pipeline
  vtkNew<vtkVolume> volume;
  vtkNew<vtkSmartVolumeMapper> volumeMapper;
  vtkNew<vtkVolumeProperty> volumeProperty;
  volumeMapper->SetInputData(dsVtkm);
  volume->SetProperty(volumeProperty);
  volumeMapper->SetBlendModeToComposite();
  volume->SetMapper(volumeMapper);

  vtkNew<vtkPiecewiseFunction> scalarOpacity;
  scalarOpacity->AddPoint(0, 0.1);
  scalarOpacity->AddPoint(90, 1.0);

  volumeProperty->ShadeOff();
  volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

  volumeProperty->SetScalarOpacity(scalarOpacity);

  vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction =
    volumeProperty->GetRGBTransferFunction(0);
  colorTransferFunction->RemoveAllPoints();
  colorTransferFunction->AddRGBPoint(0.0, 1.0, 0.0, 0.1);
  colorTransferFunction->AddRGBPoint(45, 0.0, 0.8, 0.1);
  colorTransferFunction->AddRGBPoint(90, 1.0, 0.8, 0.1);
  renderer->AddViewProp(volume);

  vtkNew<vtkOSPRayPass> ospray;
  renderer->SetPass(ospray);
  vtkNew<vtkOSPRayTestInteractor> style;
  style->SetPipelineControlPoints(renderer, ospray, nullptr);
  iren->SetInteractorStyle(style);
  style->SetCurrentRenderer(renderer);

  renderer->ResetCamera();
  renderWindow->Render();

  int retVal = vtkRegressionTestImage(renderWindow);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    iren->Start();
  }

  return !retVal;
}
