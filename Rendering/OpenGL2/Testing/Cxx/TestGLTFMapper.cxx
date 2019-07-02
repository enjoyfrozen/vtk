/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestGLTFMapper.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCompositePolyDataMapper.h>
#include <vtkGLTFMapper.h>
#include <vtkGLTFReader.h>
#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkRegressionTestImage.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkTexture.h>

int TestGLTFMapper(int argc, char* argv[])
{
  if (argc <= 2)
  {
    std::cout << "Usage: " << argv[0] << "<step> <gltf file>" << std::endl;
    return EXIT_FAILURE;
  }

  const int step = std::atoi(argv[1]);
  vtkNew<vtkGLTFReader> reader;
  reader->SetFileName(argv[2]);
  reader->SetFrameRate(60);
  reader->ApplyDeformationsToGeometryOff();

  reader->UpdateInformation(); // Read model metadata to get the number of animations
  for (vtkIdType i = 0; i < reader->GetNumberOfAnimations(); i++)
  {
    reader->EnableAnimation(i);
  }

  reader->UpdateInformation(); // Update number of time steps now that animations are enabled
  vtkInformation* readerInfo = reader->GetOutputInformation(0);

  int nbSteps = readerInfo->Length(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
  if (nbSteps < step)
  {
    std::cerr << "Invalid number of steps for input argument: " << step << std::endl;
    return EXIT_FAILURE;
  }

  if (step > 0)
  {
    double time = readerInfo->Get(vtkStreamingDemandDrivenPipeline::TIME_STEPS(), step);
    readerInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(), time);
  }
  reader->Update();

  vtkNew<vtkGLTFMapper> mapper;
  mapper->SetInputConnection(reader->GetOutputPort());

  // Get imageData from the reader, and create textures
  std::vector<vtkSmartPointer<vtkTexture> > textures;
  for (int i = 0; i < reader->GetNumberOfTextures(); i++)
  {
    // Create vtk textures
    auto readerTextureObject = reader->GetGLTFTexture(i);
    vtkNew<vtkTexture> tex;
    tex->SetInputData(readerTextureObject.Image);
    tex->InterpolateOn();
    tex->MipmapOn();
    tex->Update();
    textures.push_back(tex);
  }
  mapper->SetTextures(textures);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  // Flip texture coordinates
  if (actor->GetPropertyKeys() == nullptr)
  {
    vtkNew<vtkInformation> info;
    actor->SetPropertyKeys(info);
  }
  double mat[] = { 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
  actor->GetPropertyKeys()->Set(vtkProp::GeneralTextureTransform(), mat, 16);

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->SetBackground(.0, .0, .2);

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderWindow->Render();
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->GetActiveCamera()->SetClippingRange(0.001, 10000);
  int retVal = vtkRegressionTestImage(renderWindow);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    renderWindowInteractor->Start();
  }
  return !retVal;
}
