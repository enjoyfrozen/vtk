/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestvtkmOSPRayDataSetMapper.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// Description
// This is a test for the point gaussian mapper in the ospray backend

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkColorTransferFunction.h"
#include "vtkFloatArray.h"
#include "vtkLookupTable.h"
#include "vtkNew.h"
#include "vtkOSPRayPass.h"
#include "vtkOSPRayRendererNode.h"
#include "vtkPiecewiseFunction.h"
#include "vtkPointData.h"
#include "vtkPointGaussianMapper.h"
#include "vtkPointSource.h"
#include "vtkProperty.h"
#include "vtkRandomAttributeGenerator.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartVolumeMapper.h"
#include "vtkTesting.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkmDataSet.h"
#include "vtkmDataSetMapper.h"

#include <vtkm/cont/testing/MakeTestDataSet.h>

#include "vtkOSPRayTestInteractor.h"

int TestvtkmOSPRayDataSetMapper(int argc, char* argv[])
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

  // Create the vtkmDataSet
  vtkm::cont::testing::MakeTestDataSet Maker;
  auto dataset = Maker.Make3DUniformDataSet1();
  // vtkNew<vtkFloatArray> pointField;
  // auto srcScalars =
  // dataset.GetField("pointVar").GetData().AsArrayHandle<vtkm::cont::ArrayHandle<float>>(); auto
  // portal = srcScalars.ReadPortal(); vtkm::Id length = portal.GetNumberOfValues();

  // pointField->SetName("pointVar");
  // pointField->SetNumberOfComponents(1);
  // pointField->SetNumberOfTuples(length);
  // for (vtkm::Id i = 0; i < length; ++i)
  // {
  //   pointField->SetValue(i, portal.Get(i));
  //   std::cout << i << " " << portal.Get(i) << std::endl;
  // }
  vtkNew<vtkmDataSet> dsVtkm;
  dsVtkm->SetUseVtkmArrays(false);
  dsVtkm->SetVtkmDataSet(dataset);
  // imageData->GetPointData()->AddArray(pointField);

  // Create the rendering pipeline
  vtkNew<vtkActor> actor;
  actor->GetProperty()->SetPointSize(5);
  vtkNew<vtkmDataSetMapper> mapper;
  mapper->SetInputData(dsVtkm);
  actor->SetMapper(mapper);
  // renderer->AddActor(actor);

  // Create the volume rendering pipeline
  vtkNew<vtkVolume> volume;
  vtkNew<vtkSmartVolumeMapper> volumeMapper;
  vtkNew<vtkVolumeProperty> volumeProperty;
  volumeMapper->SetInputData(dsVtkm);
  volume->SetProperty(volumeProperty);
  volumeMapper->SetBlendModeToComposite();
  volume->SetMapper(volumeMapper);

  vtkNew<vtkPiecewiseFunction> scalarOpacity;
  scalarOpacity->AddPoint(0, 1.0);
  scalarOpacity->AddPoint(90, 1.0);

  volumeProperty->ShadeOff();
  volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

  volumeProperty->SetScalarOpacity(scalarOpacity);

  vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction =
    volumeProperty->GetRGBTransferFunction(0);
  colorTransferFunction->RemoveAllPoints();
  colorTransferFunction->AddRGBPoint(0.0, 1.0, 0.8, 0.1);
  colorTransferFunction->AddRGBPoint(90, 1.0, 0.8, 0.1);
  renderer->AddViewProp(volume);

  vtkNew<vtkOSPRayPass> ospray;
  renderer->SetPass(ospray);
  vtkNew<vtkOSPRayTestInteractor> style;
  style->SetPipelineControlPoints(renderer, ospray, nullptr);
  iren->SetInteractorStyle(style);
  style->SetCurrentRenderer(renderer);

  renderer->ResetCamera();
  // renderer->GetActiveCamera()->Zoom(2.0);
  renderWindow->Render();

  int retVal = vtkRegressionTestImage(renderWindow);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    iren->Start();
  }

  return !retVal;
}
