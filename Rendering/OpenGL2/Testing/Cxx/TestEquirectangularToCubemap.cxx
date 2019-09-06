/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkRegressionTestImage.h"
#include "vtkTestUtilities.h"

#include "vtkEquirectangularToCubemapTexture.h"
#include "vtkJPEGReader.h"
#include "vtkOpenGLTexture.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSkybox.h"

int TestEquirectangularToCubemap(int argc, char* argv[])
{
  vtkNew<vtkRenderWindow> renWin;
  renWin->SetSize(400, 400);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkNew<vtkRenderer> renderer;
  renWin->AddRenderer(renderer);

  vtkNew<vtkJPEGReader> reader;
  char * fileName = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/autoshop.jpg");
  reader->SetFileName(fileName);
  delete[] fileName;

  vtkNew<vtkTexture> texture;
  texture->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkEquirectangularToCubemapTexture> cubemap;
  cubemap->SetInputTexture(vtkOpenGLTexture::SafeDownCast(texture));

  vtkNew<vtkSkybox> world;
  world->SetTexture(cubemap);
  renderer->AddActor(world);

  renWin->Render();

  int retVal = vtkRegressionTestImage(renWin);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    iren->Start();
  }

  return !retVal;
}
