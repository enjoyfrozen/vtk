/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkSmartPointer.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkTextProperty.h"
#include "vtkActor.h"
#include "vtkCornerAnnotation.h"
#include "vtkRegressionTestImage.h"

int TestEmptyCornerAnnotation( int argc, char * argv [] )
{
  // Visualize
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);

  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderer->SetBackground(0.5, 0.5, 0.5);

  // Annotate the image with window/level and mouse over pixel information
  vtkSmartPointer<vtkCornerAnnotation> cornerAnnotation =
    vtkSmartPointer<vtkCornerAnnotation>::New();
  cornerAnnotation->SetLinearFontScaleFactor( 2 );
  cornerAnnotation->SetNonlinearFontScaleFactor( 1 );
  cornerAnnotation->SetMaximumFontSize( 20 );
  cornerAnnotation->SetText( 0, "normal text" );
  cornerAnnotation->SetText( 1, "1234567890" );
  cornerAnnotation->SetText( 2, "~`!@#$%^&*()_-+=" );
  cornerAnnotation->SetText( 3, "text to remove" );
  cornerAnnotation->GetTextProperty()->SetColor( 1,0,0);

  renderer->AddViewProp(cornerAnnotation);

  renderWindow->Render();

  // This should empty the annotation #3 and not display a black or white box
  cornerAnnotation->SetText( 3, "" );
  renderWindow->Render();

  int retVal = vtkRegressionTestImage(renderWindow);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    renderWindowInteractor->Start();
  }

  return !retVal;
}
