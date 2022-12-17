#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>

#include <QtQuick/QQuickWindow>

#include <QtGui/QGuiApplication>

#include <QtCore/QPointer.h>

#include <QQuickVTKItem.h>
#include <QVTKRenderWindowAdapter.h>

#include <vtkActor.h>
#include <vtkCapsuleSource.h>
#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

struct Presenter : QObject
{
  Q_OBJECT
  Q_PROPERTY(QStringList sources READ sources CONSTANT)
  QStringList sources() const
  {
    return QStringList{} << "Cone"
                         << "Sphere"
                         << "Capsule";
  }
};

struct MyVtkItem : QQuickVtkItem
{
  Q_OBJECT
public:
  MyVtkItem()
  {
    connect(this, &QQuickItem::widthChanged, this, &MyVtkItem::resetCamera);
    connect(this, &QQuickItem::heightChanged, this, &MyVtkItem::resetCamera);
  }

  struct Data : vtkObject
  {
    static Data* New();
    vtkTypeMacro(Data, vtkObject);

    vtkNew<vtkActor> actor;
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkConeSource> cone;
    vtkNew<vtkSphereSource> sphere;
    vtkNew<vtkCapsuleSource> capsule;
    vtkNew<vtkPolyDataMapper> mapper;
  };

  vtkUserData initializeVTK(vtkRenderWindow* renderWindow) override
  {
    vtkNew<Data> vtk;

    vtk->actor->SetMapper(vtk->mapper.Get());

    vtk->renderer->AddActor(vtk->actor);
    vtk->renderer->ResetCamera();
    vtk->renderer->SetBackground(0.5, 0.5, 0.7);
    vtk->renderer->SetBackground2(0.7, 0.7, 0.7);
    vtk->renderer->SetGradientBackground(true);

    renderWindow->AddRenderer(vtk->renderer);

    // Remember: QML can delete our underlying QSGNode (which calls this method) at any time.
    // We have to re-synchronize our Qt properties with our VTK properties at any time.
    // To this end we've added a "force" parameter to our Qt property setter which is set true
    // here in initializeVtk but is defaulted false whenever QML (or other C++ code) invokes it.
    //
    // To see QML randomly delete our QSGNode, left-click to split horizontally, then
    // right-click to split vertically and then middle-click in the smallest top-most view and
    // observe the console output.
    //
    // You'll see something like:
    //
    // clang-format off
    //qml: constructed ItemDelegate(0x1f8d43946a0, "viewBase 0") SplitView_QMLTYPE_1_QML_5(0x1f8d4395660, "splitView 0")
    //qml: constructed ItemDelegate(0x1f8d4395780, "itemBase 0")
    //qml: constructed ItemDelegate(0x1f8d4395c00, "itemBase 1")
    //qml: constructed ItemDelegate(0x1f8d4394e80, "viewBase 1") SplitView_QMLTYPE_1_QML_5(0x1f8d4394fa0, "splitView 1")
    //qml: constructed ItemDelegate(0x1f8d43950c0, "itemBase 2")
    //qml: destructed ItemDelegate(0x1f8d4395780, "itemBase 0") SplitView_QMLTYPE_1_QML_5(0x1f8d4394fa0, "splitView 1")
    //qml: destructed ItemDelegate(0x1f8d4394e80, "viewBase 1") SplitView_QMLTYPE_1_QML_5(0x1f8d4395660, "splitView 0")
    // clang-format on
    //
    // Notice that there are 2 (two) 'destructed' messages but you only clicked the middle-button
    // once!! QML deleted both "small" QSGNodes and then created a new QSGNode to fill the empty
    // column.

    setSource(_source, true);

    return vtk;
  }

  void resetCamera()
  {
    dispatch_async([pThis = QPointer<MyVtkItem>(this), this](
                     vtkRenderWindow* renderWindow, vtkUserData userData) {
      if (!pThis)
        return;

      auto* vtk = Data::SafeDownCast(userData);
      if (!vtk)
        return;

      vtk->renderer->ResetCamera();
      scheduleRender();
    });
  }

  Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
  QString source() const { return _source; }
  void setSource(QString v, bool forceVtk = false)
  {
    if (_source != v)
      emit sourceChanged((forceVtk = true, _source = v));

    if (forceVtk)
      dispatch_async([pThis = QPointer<MyVtkItem>(this), this](
                       vtkRenderWindow* renderWindow, vtkUserData userData) {
        if (!pThis)
          return;

        auto* vtk = Data::SafeDownCast(userData);
        if (!vtk)
          return;

        // clang-format off
          vtk->mapper->SetInputConnection(
                _source == "Cone"    ? vtk->cone->GetOutputPort()
              : _source == "Sphere"  ? vtk->sphere->GetOutputPort()
              : _source == "Capsule" ? vtk->capsule->GetOutputPort()
              : (qWarning() << Q_FUNC_INFO << "YIKES!! Unknown source:'" << _source << "'", nullptr));
        // clang-format on

        resetCamera();
      });
  }
  Q_SIGNAL void sourceChanged(QString);
  QString _source;
};
vtkStandardNewMacro(MyVtkItem::Data);

int main(int argc, char* argv[])
{
  QQuickVtkItem::setGraphicsApi();

#if defined(Q_OS_WIN)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  QGuiApplication app(argc, argv);
  Presenter presenter;

  qmlRegisterType<MyVtkItem>("com.vtk.example", 1, 0, "MyVtkItem");
  qmlRegisterUncreatableType<Presenter>("com.vtk.example", 1, 0, "Presenter", "!!");

  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("presenter", &presenter);
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
  if (engine.rootObjects().isEmpty())
    return -1;

  return app.exec();
}

#include "main.moc"
