/*=========================================================================

Program:   Visualization Toolkit
Module:    TestQQuickVTKItem_3.cxx

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
#include "vtkColorTransferFunction.h"
#include "vtkConeSource.h"
#include "vtkGenericOpenGLRenderWindow.h"
#include "vtkGlyph3DMapper.h"
#include "vtkNew.h"
#include "vtkPNGWriter.h"
#include "vtkPiecewiseFunction.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkSmartVolumeMapper.h"
#include "vtkSphereSource.h"
#include "vtkTestUtilities.h"
#include "vtkTesting.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkWindowToImageFilter.h"
#include "vtkXMLImageDataReader.h"

#include <QApplication>
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTimer>
#include <QUrl>

namespace
{
int Argc = 0;
char** Argv = nullptr;

/*=========================================================================*/

struct MyGeomItem : QQuickVTKItem
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

    return vtk;
  }
};
vtkStandardNewMacro(MyGeomItem::Data);

/*=========================================================================*/

struct MyVolumeItem : QQuickVTKItem
{
  struct Data : vtkObject
  {
    static Data* New();
    vtkTypeMacro(Data, vtkObject);
  };

  vtkUserData initializeVTK(vtkRenderWindow* renderWindow) override
  {
    auto vtk = vtkNew<Data>();

    vtkNew<vtkRenderer> renderer;
    renderWindow->AddRenderer(renderer);

    // Create a volume pipeline and add it to the view
    vtkNew<vtkSmartVolumeMapper> volumeMapper;
    vtkNew<vtkXMLImageDataReader> reader;
    const char* volumeFile =
      vtkTestUtilities::ExpandDataFileName(Argc, Argv, "Data/vase_1comp.vti");
    reader->SetFileName(volumeFile);
    volumeMapper->SetInputConnection(reader->GetOutputPort());
    delete[] volumeFile;
    double scalarRange[2];
    volumeMapper->GetInput()->GetScalarRange(scalarRange);
    volumeMapper->SetAutoAdjustSampleDistances(1);
    volumeMapper->SetBlendModeToComposite();
    vtkNew<vtkPiecewiseFunction> scalarOpacity;
    scalarOpacity->AddPoint(scalarRange[0], 0.0);
    scalarOpacity->AddPoint(scalarRange[1], 0.09);
    vtkNew<vtkVolumeProperty> volumeProperty;
    volumeProperty->ShadeOff();
    volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);
    volumeProperty->SetScalarOpacity(scalarOpacity);
    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction =
      volumeProperty->GetRGBTransferFunction(0);
    colorTransferFunction->RemoveAllPoints();
    colorTransferFunction->AddRGBPoint(scalarRange[0], 0.6, 0.4, 0.1);
    // colorTransferFunction->AddRGBPoint(scalarRange[1], 0.2, 0.1, 0.3);
    vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);
    renderer->AddVolume(volume);
    renderer->ResetCamera();

    return vtk;
  }
};
vtkStandardNewMacro(MyVolumeItem::Data);

/*=========================================================================*/

struct MyGlyphItem : QQuickVTKItem
{
  struct Data : vtkObject
  {
    static Data* New();
    vtkTypeMacro(Data, vtkObject);
  };

  vtkUserData initializeVTK(vtkRenderWindow* renderWindow) override
  {
    auto vtk = vtkNew<Data>();

    vtkNew<vtkRenderer> renderer;
    renderWindow->AddRenderer(renderer);

    // Create the glyph pipeline
    vtkNew<vtkSphereSource> sphere;
    vtkNew<vtkGlyph3DMapper> glyphMapper;
    vtkNew<vtkConeSource> squad;
    glyphMapper->SetInputConnection(sphere->GetOutputPort());
    glyphMapper->SetSourceConnection(squad->GetOutputPort());
    glyphMapper->SetOrientationArray("Normals");
    vtkNew<vtkActor> glyphActor;
    glyphActor->SetMapper(glyphMapper);
    glyphActor->GetProperty()->SetDiffuseColor(0.5, 1.0, 0.8);
    renderer->AddActor(glyphActor);
    renderer->ResetCamera();

    return vtk;
  }
};
vtkStandardNewMacro(MyGlyphItem::Data);
}

/*=========================================================================*/

int TestQQuickVTKItem_3(int argc, char* argv[])
{
  Argc = argc;
  Argv = argv;

  cout << "CTEST_FULL_OUTPUT (Avoid ctest truncation of output)" << endl;

  QQuickVTKItem::setGraphicsApi();
  QApplication app(argc, argv);

  qmlRegisterType<MyGeomItem>("Vtk", 1, 0, "MyGeomItem");
  qmlRegisterType<MyGlyphItem>("Vtk", 1, 0, "MyGlyphItem");
  qmlRegisterType<MyVolumeItem>("Vtk", 1, 0, "MyVolumeItem");

  QQmlApplicationEngine engine;
  engine.setOutputWarningsToStandardError(true);
  qDebug() << "QML2_IMPORT_PATH:" << engine.importPathList();
  engine.load(QUrl("qrc:///TestQQuickVTKItem_3.qml"));

  QObject* topLevel = engine.rootObjects().value(0);
  QQuickWindow* window = qobject_cast<QQuickWindow*>(topLevel);

  window->show();

  // Wait a little for the application and window to be set up properly
  QEventLoop loop;
  QTimer::singleShot(1000, &loop, SLOT(quit()));
  loop.exec();

  vtkNew<vtkTesting> vtktesting;
  vtktesting->AddArguments(argc, argv);
  if (vtktesting->IsInteractiveModeSpecified())
  {
    return QApplication::exec();
  }

  // Capture a screenshot of the item
  QImage im = window->grabWindow();

  std::string validName = std::string(vtktesting->GetValidImageFileName());
  std::string::size_type slashPos = validName.rfind('/');
  if (slashPos != std::string::npos)
  {
    validName = validName.substr(slashPos + 1);
  }
  std::string tmpDir = vtktesting->GetTempDirectory();
  std::string vImage = tmpDir + "/" + validName;
  im.save(QString::fromStdString(vImage), "PNG");

  int retVal = vtktesting->RegressionTest(vImage, 10);

  switch (retVal)
  {
    case vtkTesting::FAILED:
    case vtkTesting::NOT_RUN:
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
