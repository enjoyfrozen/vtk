#include "vtkTestUtilities.h"

#include "vtkCamera.h"
#include "vtkDepthImageToPointCloud.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"
#include "vtkWindowToImageFilter.h"

int TestDepthImageToPointCloud(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  // Simple test for rendering a sphere and converting it to a point cloud
  const int width = 300, height = 300;
  const double radius = 3.0;

  // Create renderers and windows for offscreen rendering
  vtkNew<vtkRenderWindow> renWinColors;
  renWinColors->SetSize(width, height);
  renWinColors->SetOffScreenRendering(true);

  vtkNew<vtkRenderWindow> renWinDepth;
  renWinDepth->SetSize(width, height);
  renWinDepth->SetOffScreenRendering(true);

  vtkNew<vtkRenderer> rendererColor;
  vtkNew<vtkRenderer> rendererDepth;

  renWinColors->AddRenderer(rendererColor);
  renWinDepth->AddRenderer(rendererDepth);

  // Create sphere with fixed radius
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0, 0, 5.0);
  sphereSource->SetRadius(radius);
  sphereSource->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(mapper);

  // Add actor to the renderers
  rendererColor->AddActor(sphereActor);
  rendererDepth->AddActor(sphereActor);

  // Enable parallel projection and share the camera between the renderers
  rendererColor->GetActiveCamera()->SetParallelProjection(true);
  rendererColor->ResetCamera();
  rendererDepth->SetActiveCamera(rendererColor->GetActiveCamera());

  // Render into offscreen windows
  renWinDepth->Render();
  renWinColors->Render();

  // Establish window-to-image filters
  vtkNew<vtkWindowToImageFilter> w2iColors;
  w2iColors->SetInput(renWinColors);
  w2iColors->SetInputBufferTypeToRGBA();
  w2iColors->Update();

  vtkNew<vtkWindowToImageFilter> w2iDepth;
  w2iDepth->SetInput(renWinDepth);
  w2iDepth->SetInputBufferTypeToZBuffer();
  w2iDepth->Update();

  // Create depth image to point cloud instance
  vtkNew<vtkDepthImageToPointCloud> depthImageToPointCloud;
  depthImageToPointCloud->SetInputConnection(0, w2iDepth->GetOutputPort());
  depthImageToPointCloud->SetInputConnection(1, w2iColors->GetOutputPort());
  depthImageToPointCloud->SetCamera(rendererColor->GetActiveCamera());
  depthImageToPointCloud->Update();

  // Get the output point cloud
  vtkPolyData* pointCloud = depthImageToPointCloud->GetOutput();

  double bounds[6];
  pointCloud->GetBounds(bounds);

  double xRange = bounds[1] - bounds[0];
  double yRange = bounds[3] - bounds[2];
  double zRange = bounds[5] - bounds[4];

  int retval = !(xRange > 4.0 && yRange > 4.0 && zRange > 0.0);

  // Now test using View coordinates
  depthImageToPointCloud->SetOutputCoordinateSystem(vtkDepthImageToPointCloud::View);
  depthImageToPointCloud->Update();

  pointCloud = depthImageToPointCloud->GetOutput();
  pointCloud->GetBounds(bounds);

  xRange = bounds[1] - bounds[0];
  yRange = bounds[3] - bounds[2];
  zRange = bounds[5] - bounds[4];

  retval = retval & !(xRange < 1.0 && yRange < 1.0 && zRange > 0.0);

  return retval;
}
