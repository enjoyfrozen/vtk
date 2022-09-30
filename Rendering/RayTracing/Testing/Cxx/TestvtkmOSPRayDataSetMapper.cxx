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
#include "vtkLookupTable.h"
#include "vtkNew.h"
#include "vtkOSPRayPass.h"
#include "vtkOSPRayRendererNode.h"
#include "vtkPointGaussianMapper.h"
#include "vtkPointSource.h"
#include "vtkRandomAttributeGenerator.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkTesting.h"
#include "vtkmDataSet.h"
#include "vtkmDataSetMapper.h"

#include <vtkm/cont/testing/MakeTestDataSet.h>

#include "vtkOSPRayTestInteractor.h"

int TestvtkmOSPRayDataSetMapper(int argc, char* argv[])
{
  cout << "CTEST_FULL_OUTPUT (Avoid ctest truncation of output)" << endl;

  int desiredPoints = 1.0e3;

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
  vtkNew<vtkmDataSet> dsVtkm;
  dsVtkm->SetVtkmDataSet(dataset);

  // Create the rendering pipeline
  vtkNew<vtkActor> actor;
  vtkNew<vtkmDataSetMapper> mapper;
  mapper->SetInputData(dsVtkm);
  actor->SetMapper(mapper);
  renderer->AddActor(actor);

  vtkNew<vtkOSPRayPass> ospray;
  renderer->SetPass(ospray);
  vtkNew<vtkOSPRayTestInteractor> style;
  style->SetPipelineControlPoints(renderer, ospray, nullptr);
  iren->SetInteractorStyle(style);
  style->SetCurrentRenderer(renderer);

  renderer->ResetCamera();
  renderer->GetActiveCamera()->Zoom(2.0);
  renderWindow->Render();

  int retVal = vtkRegressionTestImage(renderWindow);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    iren->Start();
  }

  return !retVal;
}
