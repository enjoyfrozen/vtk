#include <QtQml/QQmlApplicationEngine>

#include <QtGui/QGuiApplication>

#include "vtkRendererCollection.h"
#include <QQuickVTKItem.h>
#include <vtkActor.h>
#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

struct MyVtkItem : QQuickVtkItem
{
  void initializeVTK(vtkRenderWindow* renderWindow) override
  {
    // Create a cone pipeline and add it to the view
    vtkNew<vtkConeSource> cone;

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(cone->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();
    renderer->SetBackground(0.0, 1.0, 1.0);
    renderer->SetBackground2(1.0, 0.0, 0.0);
    renderer->SetGradientBackground(true);

    renderWindow->AddRenderer(renderer);
    renderWindow->SetMultiSamples(16);
  }

  void syncVTK(vtkRenderWindow* renderWindow) override
  {
    // Synchronize gui state with VTK state
    double vp[4];
    qtRect2vtkViewport(boundingRect(), vp);
    renderWindow->GetRenderers()->GetFirstRenderer()->SetViewport(vp);
  }
};

int main(int argc, char* argv[])
{
#if defined(Q_OS_WIN)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  QGuiApplication app(argc, argv);

  qmlRegisterType<MyVtkItem>("com.bluequartz.example1", 1, 0, "MyVtkItem");

  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
  if (engine.rootObjects().isEmpty())
    return -1;

  return app.exec();
}
