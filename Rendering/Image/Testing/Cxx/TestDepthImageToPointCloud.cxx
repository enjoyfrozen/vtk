/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestImageResliceMapperAlpha.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// Test vtkDepthImageToPointCloud using a scene containing a sphere.

#include "vtkCamera.h"
#include "vtkDepthImageToPointCloud.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"
#include "vtkTestUtilities.h"
#include "vtkWindowToImageFilter.h"

int TestDepthImageToPointCloud(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  const int width = 300, height = 300;
  const double radius = 3.0;

  // Create renderer and render window for offscreen rendering. A render window
  // has both a ZBuffer and an RGBA buffer, so one window is sufficient.
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(width, height);
  renderWindow->SetOffScreenRendering(true);

  vtkNew<vtkRenderer> renderer;
  renderWindow->AddRenderer(renderer);

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0, 0, 5.0);
  sphereSource->SetRadius(radius);
  sphereSource->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(mapper);

  renderer->AddActor(sphereActor);

  // Use parallel projection - then we can easily deduce the
  // dimensions of the output
  renderer->GetActiveCamera()->SetParallelProjection(true);
  renderer->ResetCamera();

  // Establish window-to-image filters
  vtkNew<vtkWindowToImageFilter> windowToImageColors;
  windowToImageColors->SetInput(renderWindow);
  windowToImageColors->SetInputBufferTypeToRGBA();

  vtkNew<vtkWindowToImageFilter> windowToImageDepth;
  windowToImageDepth->SetInput(renderWindow);
  windowToImageDepth->SetInputBufferTypeToZBuffer();

  // Render into offscreen windows
  renderWindow->Render();

  windowToImageColors->Update();
  windowToImageDepth->Update();

  // Create depth image to point cloud instance
  vtkNew<vtkDepthImageToPointCloud> depthImageToPointCloud;
  depthImageToPointCloud->SetInputConnection(0, windowToImageDepth->GetOutputPort());
  depthImageToPointCloud->SetInputConnection(1, windowToImageColors->GetOutputPort());
  depthImageToPointCloud->SetCamera(renderer->GetActiveCamera());
  depthImageToPointCloud->Update();

  // Get the output point cloud
  vtkPolyData* pointCloud = depthImageToPointCloud->GetOutput();

  double bounds[6];
  pointCloud->GetBounds(bounds);

  double xRange = bounds[1] - bounds[0];
  double yRange = bounds[3] - bounds[2];
  double zRange = bounds[5] - bounds[4];

  // Even for a sphere with few facets, the extent should be larger than this
  bool success = true;
  success &= (xRange > 4.0 && yRange > 4.0 && zRange > 0.0);

  // Now test using View coordinates
  depthImageToPointCloud->SetOutputCoordinateSystem(vtkDepthImageToPointCloud::View);
  depthImageToPointCloud->Update();

  pointCloud = depthImageToPointCloud->GetOutput();
  pointCloud->GetBounds(bounds);

  xRange = bounds[1] - bounds[0];
  yRange = bounds[3] - bounds[2];
  zRange = bounds[5] - bounds[4];

  // Using view-corodinates, we must stay within [-1, 1]x[-1, 1]x[-1,1]
  success &= (xRange < 2.0 && yRange < 2.0 && zRange > 0.0);

  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
