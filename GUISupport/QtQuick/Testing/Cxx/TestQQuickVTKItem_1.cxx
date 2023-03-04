/*=========================================================================

Program:   Visualization Toolkit
Module:    TestQQuickVTKItem_1.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// Description
// Tests QQuickVTKItem

#include "QQuickVTKItem.h"
#include "vtkActor.h"
#include "vtkConeSource.h"
#include "vtkNew.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkTestUtilities.h"
#include "vtkTesting.h"
#include "vtkWindowToImageFilter.h"

#include <QApplication>
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTimer>
#include <QUrl>

#if defined(Q_OS_WIN) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include "QVTKRenderWindowAdapter.h"
#include <QOpenGLFramebufferObject>
#endif

namespace
{
struct MyVtkItem : QQuickVTKItem
{
  struct Data : vtkObject
  {
    static Data* New();
    vtkTypeMacro(Data, vtkObject);
  };
  vtkUserData initializeVTK(vtkRenderWindow* renderWindow) override
  {
    auto vtk = vtkNew<Data>();

    // Create a cone pipeline and add it to the view
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkActor> actor;
    vtkNew<vtkPolyDataMapper> mapper;
    vtkNew<vtkConeSource> cone;
    renderWindow->AddRenderer(renderer);
    mapper->SetInputConnection(cone->GetOutputPort());
    actor->SetMapper(mapper);
    renderer->AddActor(actor);
    renderer->ResetCamera();
    renderer->SetBackground2(0.7, 0.7, 0.7);
    renderer->SetGradientBackground(true);

    return vtk;
  }
};
vtkStandardNewMacro(MyVtkItem::Data);
}

int TestQQuickVTKItem_1(int argc, char* argv[])
{
  cout << "CTEST_FULL_OUTPUT (Avoid ctest truncation of output)" << endl;

#if defined(Q_OS_WIN) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QSurfaceFormat fmt = QVTKRenderWindowAdapter::defaultFormat(false);
  fmt.setAlphaBufferSize(0);
  QSurfaceFormat::setDefaultFormat(fmt);
  QQuickWindow::setSceneGraphBackend(QSGRendererInterface::OpenGL);
#else
  QQuickVTKItem::setGraphicsApi();
#endif
  QApplication app(argc, argv);

  qmlRegisterType<MyVtkItem>("Vtk", 1, 0, "MyVtkItem");

  QQmlApplicationEngine engine;
  engine.setOutputWarningsToStandardError(true);
  qDebug() << "QML2_IMPORT_PATH:" << engine.importPathList();
  engine.load(QUrl("qrc:///TestQQuickVTKItem_1.qml"));

  QObject* topLevel = engine.rootObjects().value(0);
  QQuickWindow* window = qobject_cast<QQuickWindow*>(topLevel);

  vtkNew<vtkTesting> vtktesting;
  vtktesting->AddArguments(argc, argv);
  if (vtktesting->IsInteractiveModeSpecified())
  {
    window->show();
    return QApplication::exec();
  }

  // Capture a screenshot of the item
  int retVal = EXIT_SUCCESS;
  QImage im;

#if defined(Q_OS_WIN) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QScopedPointer<QOpenGLFramebufferObject> fbo;
  QObject::connect(window, &QQuickWindow::beforeRendering, [&] {
    if (fbo.isNull())
    {
      QOpenGLFramebufferObjectFormat fmt;
      fbo.reset(new QOpenGLFramebufferObject(window->size(), fmt));
      window->setRenderTarget(fbo.data());
    }
  });
  QObject::connect(window, &QQuickWindow::afterRendering, [&] {
    if (im.isNull() && !fbo.isNull())
    {
      im = fbo->toImage();
    }
  });
#endif

  window->show();

  // Wait a little for the application and window to be set up properly
  QEventLoop loop;
  QTimer::singleShot(1000, &loop, SLOT(quit()));
  loop.exec();

  if (retVal != EXIT_SUCCESS)
    return retVal;

#if !(defined(Q_OS_WIN) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  im = window->grabWindow();
#endif

  std::string validName = std::string(vtktesting->GetValidImageFileName());
  std::string::size_type slashPos = validName.rfind('/');
  if (slashPos != std::string::npos)
  {
    validName = validName.substr(slashPos + 1);
  }
  std::string tmpDir = vtktesting->GetTempDirectory();
  std::string vImage = tmpDir + "/" + validName;
  im.save(QString::fromStdString(vImage), "PNG");

  retVal = vtktesting->RegressionTest(vImage, 10);

  switch (retVal)
  {
    case vtkTesting::FAILED:
    case vtkTesting::NOT_RUN:
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
