/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/



#include <QtWidgets/QApplication>
#include <QtGui/QSurfaceFormat>

#include "vtkGenericOpenGLRenderWindow.h"
#include "vtkImageViewer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkPNGReader.h"
#include "vtkTestUtilities.h"

#include "QVTKOpenGLWidget.h"

int main(int argc, char** argv)
{
  // set surface format before application initialization
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());
  QApplication app(argc, argv);
  QVTKOpenGLWidget widget;
  widget.resize(256,256);
  vtkNew<vtkGenericOpenGLRenderWindow> renWin;
  widget.setRenderWindow(renWin);

  vtkNew<vtkPNGReader> reader;
  char* fname = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/vtk.png");
  reader->SetFileName(fname);
  delete [] fname;

  vtkNew<vtkImageViewer> image_view;
  // use our render window with image_view
  image_view->SetRenderWindow(renWin);
  image_view->SetInputConnection(reader->GetOutputPort());
  image_view->SetupInteractor(renWin->GetInteractor());
  image_view->SetColorLevel(138.5);
  image_view->SetColorWindow(233);

  widget.show();

  app.exec();
  return 0;
}
