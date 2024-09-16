#include <QtQml/QQmlApplicationEngine>

#include <QtGui/QGuiApplication>
#include <QtGui/QSurfaceFormat>

#include <QQuickVTKItem.h>
#include <QVTKRenderWindowAdapter.h>

#include <vtkActor.h>
#include <vtkBoxWidget.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkConeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

namespace
{
class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback* New() { return new vtkMyCallback; }
  void Execute(vtkObject* caller, unsigned long, void*) override
  {
    vtkNew<vtkTransform> t;
    auto widget = reinterpret_cast<vtkBoxWidget*>(caller);
    widget->GetTransform(t);
    widget->GetProp3D()->SetUserTransform(t);
  }
};
}

struct MyVtkItem : QQuickVTKItem
{
  void initializeVTK(vtkRenderWindow* renderWindow) override
  {
    // Create a cone pipeline and add it to the view
    vtkNew<vtkConeSource> cone;
    cone->SetHeight(3.0);
    cone->SetRadius(1.0);
    cone->SetResolution(10);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(cone->GetOutputPort());

    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("Bisque").GetData());

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();
    renderer->SetBackground(colors->GetColor3d("LightBlue").GetData());
    renderer->SetBackgroundAlpha(1.0);
    double vp[4];
    qtRect2vtkViewport(boundingRect(), vp);
    renderer->SetViewport(vp);

    renderWindow->AddRenderer(renderer);
    renderWindow->SetMultiSamples(16);

    mBoxWidget = vtkSmartPointer<vtkBoxWidget>::New();
    mBoxWidget->SetInteractor(renderWindow->GetInteractor());
    mBoxWidget->SetPlaceFactor(1.25);
    mBoxWidget->GetOutlineProperty()->SetColor(colors->GetColor3d("Gold").GetData());
    mBoxWidget->SetProp3D(actor);
    mBoxWidget->PlaceWidget();
    mBoxWidget->On();

    mCallback = vtkSmartPointer<vtkMyCallback>::New();
    mBoxWidget->AddObserver(vtkCommand::InteractionEvent, mCallback);
  }

  void syncVTK(vtkRenderWindow* renderWindow) override {}

  vtkSmartPointer<vtkBoxWidget> mBoxWidget;
  vtkSmartPointer<vtkMyCallback> mCallback;
};

int main(int argc, char* argv[])
{
  QSurfaceFormat::setDefaultFormat(QVTKRenderWindowAdapter::defaultFormat());

#if defined(Q_OS_WIN)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  QGuiApplication app(argc, argv);

  qmlRegisterType<MyVtkItem>("com.bluequartz.example", 1, 0, "MyVtkItem");

  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
  if (engine.rootObjects().isEmpty())
    return -1;

  return app.exec();
}
