/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkActor.h"
#include "vtkConeSource.h"
#include "vtkDataSetMapper.h"
#include "vtkGenericOpenGLRenderWindow.h"
#include "vtkNew.h"
#include "vtkProperty.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"
#include "QVTKWidget2.h"
#include "QTestMainWindow.h"

#include <QApplication>
#include <QTimer>

int TestQVTKWidget2(int argc, char* argv[])
{
  QApplication app(argc, argv);

  vtkNew<vtkSphereSource> sphere;
  vtkNew<vtkConeSource> cone;

  vtkNew<vtkDataSetMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphere->GetOutputPort());
  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);

  vtkNew<vtkDataSetMapper> coneMapper;
  coneMapper->SetInputConnection(cone->GetOutputPort());
  vtkNew<vtkActor> coneActor;
  coneActor->SetMapper(coneMapper);

  sphereActor->GetProperty()->SetOpacity(0.3);

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(sphereActor);
  renderer->AddActor(coneActor);
  renderer->ResetCamera();

  vtkNew<vtkGenericOpenGLRenderWindow> renWin;
  renWin->AddRenderer(renderer);
  renWin->SetMultiSamples(0);

  QTestMainWindow* qwindow = new QTestMainWindow(renWin, argc, argv);
  QVTKWidget2* widget = new QVTKWidget2(renWin);
  widget->setMinimumSize(QSize(300, 300));
  widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  qwindow->setCentralWidget(widget);

  qwindow->show();

  QTimer::singleShot(500, qwindow, SLOT(captureImage()));
  int appVal = app.exec();
  int retVal = qwindow->regressionImageResult();

  delete qwindow;

  return !retVal + appVal;
}
