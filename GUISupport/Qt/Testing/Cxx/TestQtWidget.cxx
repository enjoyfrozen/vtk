/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestQtWidget.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// Tests QVTKOpenGLStereoWidget/QVTKOpenGLWindow/QVTKOpenGLNativeWidget

#include "TestQtCommon.h"

#include "vtkActor.h"
#include "vtkGenericOpenGLRenderWindow.h"
#include "vtkNew.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"
#include "vtkTesting.h"

#include <QApplication>
#include <QSurfaceFormat>
#include <qmainwindow.h>

int TestQtWidget(int argc, char* argv[])
{
  // disable multisampling.
  vtkOpenGLRenderWindow::SetGlobalMaximumNumberOfMultiSamples(0);

  auto type = detail::select_widget(argc, argv);
  // setup default format, if needed.
  detail::set_default_format(type);

  QByteArray a = qgetenv("QT_OPENGL");
  vtkLogF(INFO, "Getting QT_OPENGL set to %s", a.constData());
  QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);

  QApplication app(argc, argv);

  vtkNew<vtkTesting> vtktesting;
  vtktesting->AddArguments(argc, argv);

  auto widgetOrWindow = detail::create_widget_or_window(type, nullptr);
  {
    vtkNew<vtkGenericOpenGLRenderWindow> window0;
    detail::set_render_window(widgetOrWindow, window0);
    detail::show(widgetOrWindow, QSize(200, 200));
  }

  // make sure rendering works correctly after switching to a new render window
  vtkNew<vtkGenericOpenGLRenderWindow> window;
  detail::set_render_window(widgetOrWindow, window);

  vtkNew<vtkRenderer> ren;
  ren->SetGradientBackground(true);
  ren->SetBackground2(0.7, 0.7, 0.7);
  window->AddRenderer(ren);

  vtkNew<vtkSphereSource> sphere;
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphere->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  ren->AddActor(actor);

  vtkLogF(INFO, "About to show window");
  detail::show(widgetOrWindow, QSize(300, 300));
  vtkLogF(INFO, "Showed window");
  detail::process_events_and_wait(1000); // let's wait a little longer for the resize
  vtkLogF(INFO, "Resized window");

  const int* windowSize = window->GetSize();
  const int* screenSize = window->GetScreenSize();
  vtkLogF(INFO, "Resized window dims: (%i, %i)", windowSize[0], windowSize[1]);
  if (screenSize[0] < windowSize[0] || screenSize[1] < windowSize[1])
  {
    std::cout << "Expected vtkGenericOpenGLRenderWindow::GetScreenSize() "
                 "dimensions to be larger than the render window size"
              << std::endl;
    return EXIT_FAILURE;
  }

  vtktesting->SetRenderWindow(window);
  vtkLogF(INFO, "Set Render Window for testing");

  int retVal = vtktesting->RegressionTest(10);
  vtkLogF(INFO, "RegressionTest");
  switch (retVal)
  {
    case vtkTesting::DO_INTERACTOR:
      return QApplication::exec();
    case vtkTesting::FAILED:
    case vtkTesting::NOT_RUN:
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
