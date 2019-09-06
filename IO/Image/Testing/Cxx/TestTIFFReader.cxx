/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include <vtkSmartPointer.h>

#include <vtkTIFFReader.h>
#include <vtkImageViewer2.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>

int TestTIFFReader(int argc, char* argv[])
{
  //Verify input arguments
  if ( argc < 3 )
  {
    std::cout << "Usage: " << argv[0]
              << " Filename(.tif)" << std::endl;
    return EXIT_FAILURE;
  }

  //Read the image
  vtkSmartPointer<vtkTIFFReader> reader =
    vtkSmartPointer<vtkTIFFReader>::New();
  reader->SetFileName ( argv[1] );
  reader->SetOrientationType(4);
  reader->Update();

  // Display the center slice
  int sliceNumber =
    (reader->GetOutput()->GetExtent()[5] +
     reader->GetOutput()->GetExtent()[4]) / 2;

  // Visualize
  vtkSmartPointer<vtkImageViewer2> imageViewer =
    vtkSmartPointer<vtkImageViewer2>::New();
  imageViewer->SetInputConnection(reader->GetOutputPort());
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  imageViewer->SetupInteractor(renderWindowInteractor);
  imageViewer->SetSlice(sliceNumber);
  imageViewer->Render();
  imageViewer->GetRenderer()->ResetCamera();
  renderWindowInteractor->Initialize();
  imageViewer->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
