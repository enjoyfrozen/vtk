/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkImageData.h"
#include "vtkImageViewer.h"
#include "vtkPNGReader.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"

int TestPNGReader(int argc, char* argv[])
{

  if (argc <= 1)
  {
    cout << "Usage: " << argv[0] << " <png file>" << endl;
    return EXIT_FAILURE;
  }

  std::string filename = argv[1];

  vtkNew<vtkPNGReader> pngReader;

  // Check the image can be read
  if (!pngReader->CanReadFile(filename.c_str()))
  {
    cerr << "CanReadFile failed for " << filename.c_str() << "\n";
    return EXIT_FAILURE;
  }

  // Read the input image
  pngReader->SetFileName(filename.c_str());
  pngReader->Update();

  // Read and display the image properties
  const char* fileExtensions = pngReader->GetFileExtensions();
  cout << "File extensions: " << fileExtensions << endl;

  const char* descriptiveName = pngReader->GetDescriptiveName();
  cout << "Descriptive name: " << descriptiveName << endl;

  // Visualize
  vtkNew<vtkImageViewer> imageViewer;
  imageViewer->SetInputConnection(pngReader->GetOutputPort());
  imageViewer->SetColorWindow(256);
  imageViewer->SetColorLevel(127.5);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  imageViewer->SetupInteractor(renderWindowInteractor);
  imageViewer->Render();

  vtkRenderWindow* renWin = imageViewer->GetRenderWindow();
  int retVal = vtkRegressionTestImage(renWin);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    renderWindowInteractor->Start();
  }

  return !retVal;
}
