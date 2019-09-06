/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include <vtkGLTFImporter.h>
#include <vtkRegressionTestImage.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int TestGLTFImporter(int argc, char* argv[])
{
  if (argc <= 1)
  {
    std::cout << "Usage: " << argv[0] << " <gltf file>" << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkGLTFImporter> importer;
  importer->SetFileName(argv[1]);

  vtkNew<vtkRenderWindow> renderWindow;
  importer->SetRenderWindow(renderWindow);

  vtkNew<vtkRenderer> renderer;
  renderWindow->AddRenderer(renderer);
  renderer->SetBackground(.0, .0, .2);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  importer->Update();
  renderWindow->Render();

  int retVal = vtkRegressionTestImage(renderWindow);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    renderWindowInteractor->Start();
  }
  return !retVal;
}
