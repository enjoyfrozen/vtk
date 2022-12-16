/*=========================================================================

Program:   Visualization Toolkit
Module:    TestQQuickVTKRenderItem.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// Description
// Tests QQuickVtkItem

#include "QQuickVtkItem.h"
#include "vtkActor.h"
#include "vtkAppendPolyData.h"
#include "vtkCamera.h"
#include "vtkClipPolyData.h"
#include "vtkCommand.h"
#include "vtkConeSource.h"
#include "vtkGenericOpenGLRenderWindow.h"
#include "vtkGlyph3D.h"
#include "vtkImplicitPlaneRepresentation.h"
#include "vtkImplicitPlaneWidget2.h"
#include "vtkInteractorEventRecorder.h"
#include "vtkNew.h"
#include "vtkPNGWriter.h"
#include "vtkPlane.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkSphereSource.h"
#include "vtkTestUtilities.h"
#include "vtkTesting.h"
#include "vtkWindowToImageFilter.h"

#include <QApplication>
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTimer>
#include <QUrl>

namespace
{
struct MyConeItem : QQuickVtkItem
{
  struct Data : vtkObject
  {
    static Data* New();
    vtkTypeMacro(Data, vtkObject);
  };

  struct Callback : public vtkCommand
  {
    static Callback* New();
    void Execute(vtkObject* caller, unsigned long evt, void*) override
    {
      if (evt == vtkCommand::EndEvent)
      {
        vtkRenderWindow* renderWindow = vtkRenderWindow::SafeDownCast(caller);
        renderWindow->GetRenderers()->GetFirstRenderer()->ResetCamera();
        renderWindow->RemoveObserver(this);
        this->pThis->scheduleRender();
      }
    }
    MyConeItem* pThis;
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
    renderer->SetBackground(0.5, 0.5, 0.7);
    renderer->SetBackground2(0.7, 0.7, 0.7);
    renderer->SetGradientBackground(true);

    vtkNew<Callback> myCallback;
    myCallback->pThis = this;
    renderWindow->AddObserver(vtkCommand::EndEvent, myCallback);

    return vtk;
  }
};
vtkStandardNewMacro(MyConeItem::Data);
vtkStandardNewMacro(MyConeItem::Callback);

/*=========================================================================*/

struct MyWidgetItem : QQuickVtkItem
{
  struct Data : vtkObject
  {
    static Data* New();
    vtkTypeMacro(Data, vtkObject);

    vtkNew<vtkImplicitPlaneWidget2> planeWidget;
  };

  struct Callback : public vtkCommand
  {
    static Callback* New();
    void Execute(vtkObject* caller, unsigned long evt, void*) override
    {
      if (evt == vtkCommand::InteractionEvent)
      {
        vtkImplicitPlaneWidget2* planeWidget = vtkImplicitPlaneWidget2::SafeDownCast(caller);
        this->Rep->GetPlane(this->Plane);
        this->Actor->VisibilityOn();
      }

      if (evt == vtkCommand::EndEvent)
      {
        // Once the application is up, adjust the camera, widget reps, etc.
        this->Renderer->ResetCamera();
        this->Rep->SetPlaceFactor(1.25);
        this->Rep->PlaceWidget(this->Glyph->GetOutput()->GetBounds());
        this->Renderer->GetActiveCamera()->Azimuth(20);
        this->Renderer->GetRenderWindow()->RemoveObserver(this);
        this->pThis->scheduleRender();
      }
    }
    Callback()
      : Plane(nullptr)
      , Actor(nullptr)
    {
    }
    vtkPlane* Plane;
    vtkActor* Actor;
    vtkGlyph3D* Glyph;
    vtkRenderer* Renderer;
    vtkImplicitPlaneRepresentation* Rep;
    MyWidgetItem* pThis;
  };

  vtkUserData initializeVTK(vtkRenderWindow* renderWindow) override
  {
    auto vtk = vtkNew<Data>();

    vtkNew<vtkRenderer> renderer;
    renderWindow->AddRenderer(renderer);

    // Create a mace out of filters.
    //
    vtkNew<vtkSphereSource> sphere;
    vtkNew<vtkGlyph3D> glyph;
    vtkNew<vtkConeSource> cone;
    glyph->SetInputConnection(sphere->GetOutputPort());
    glyph->SetSourceConnection(cone->GetOutputPort());
    glyph->SetVectorModeToUseNormal();
    glyph->SetScaleModeToScaleByVector();
    glyph->SetScaleFactor(0.25);

    // The sphere and spikes are appended into a single polydata.
    // This just makes things simpler to manage.
    vtkNew<vtkAppendPolyData> apd;
    apd->AddInputConnection(glyph->GetOutputPort());
    apd->AddInputConnection(sphere->GetOutputPort());

    vtkNew<vtkPolyDataMapper> maceMapper;
    maceMapper->SetInputConnection(apd->GetOutputPort());

    vtkNew<vtkActor> maceActor;
    maceActor->SetMapper(maceMapper);
    maceActor->VisibilityOn();

    // This portion of the code clips the mace with the vtkPlanes
    // implicit function. The clipped region is colored green.
    vtkNew<vtkPlane> plane;
    vtkNew<vtkClipPolyData> clipper;
    clipper->SetInputConnection(apd->GetOutputPort());
    clipper->SetClipFunction(plane);
    clipper->InsideOutOn();

    vtkNew<vtkPolyDataMapper> selectMapper;
    selectMapper->SetInputConnection(clipper->GetOutputPort());

    vtkNew<vtkActor> selectActor;
    selectActor->SetMapper(selectMapper);
    selectActor->GetProperty()->SetColor(0, 1, 0);
    selectActor->VisibilityOff();
    selectActor->SetScale(1.01, 1.01, 1.01);

    vtkNew<vtkImplicitPlaneRepresentation> rep;

    // The SetInteractor method is how 3D widgets are associated with the render
    // window interactor. Internally, SetInteractor sets up a bunch of callbacks
    // using the Command/Observer mechanism (AddObserver()).
    vtkNew<Callback> myCallback;
    myCallback->Plane = plane;
    myCallback->Actor = selectActor;
    myCallback->Glyph = glyph;
    myCallback->Rep = rep;
    myCallback->Renderer = renderer;
    myCallback->pThis = this;

    vtk->planeWidget->SetRepresentation(rep);
    vtk->planeWidget->AddObserver(vtkCommand::InteractionEvent, myCallback);
    renderer->GetRenderWindow()->AddObserver(vtkCommand::EndEvent, myCallback);
    auto iren = renderWindow->GetInteractor();
    vtk->planeWidget->SetInteractor(iren);
    vtk->planeWidget->SetCurrentRenderer(renderer);
    vtk->planeWidget->SetEnabled(1);
    vtk->planeWidget->SetProcessEvents(1);

    renderer->AddActor(maceActor);
    renderer->AddActor(selectActor);

    return vtk;
  }
};
vtkStandardNewMacro(MyWidgetItem::Data);
vtkStandardNewMacro(MyWidgetItem::Callback);
}

int TestQQuickVtkItem_2(int argc, char* argv[])
{
  cout << "CTEST_FULL_OUTPUT (Avoid ctest truncation of output)" << endl;

  QQuickVtkItem::setGraphicsApi();
  QApplication app(argc, argv);

  qmlRegisterType<MyConeItem>("Vtk", 1, 0, "MyConeItem");
  qmlRegisterType<MyWidgetItem>("Vtk", 1, 0, "MyWidgetItem");

  QQmlApplicationEngine engine;
  engine.setOutputWarningsToStandardError(true);
  qDebug() << "QML2_IMPORT_PATH:" << engine.importPathList();
  engine.load(QUrl("qrc:///TestQQuickVtkItem_2.qml"));

  QObject* topLevel = engine.rootObjects().value(0);
  QQuickWindow* window = qobject_cast<QQuickWindow*>(topLevel);

  window->show();

  // Wait a little for the application and window to be set up properly
  QEventLoop loop;
  QTimer::singleShot(100, &loop, SLOT(quit()));
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
