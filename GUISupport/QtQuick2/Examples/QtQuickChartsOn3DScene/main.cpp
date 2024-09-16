#include <QtQml/QQmlApplicationEngine>

#include <QtGui/QGuiApplication>
#include <QtGui/QSurfaceFormat>

#include <QQuickVTKItem.h>
#include <QVTKRenderWindowAdapter.h>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkChartXY.h>
#include <vtkContextActor.h>
#include <vtkContextScene.h>
#include <vtkCubeSource.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlotPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>

struct MyVtkItem : QQuickVTKItem
{
  vtkNew<vtkRenderer> renderer;

  void initializeVTK(vtkRenderWindow* renwin) override
  {
    vtkNew<vtkNamedColors> colors;

    vtkSmartPointer<vtkRenderWindowInteractor> iren = renwin->GetInteractor();

    renderer->SetBackground(colors->GetColor4d("seagreen").GetData());
    renwin->AddRenderer(renderer);

    renderer->ResetCamera();
    renderer->GetActiveCamera()->SetPosition(1.0, 1.0, -4.0);
    renderer->GetActiveCamera()->Azimuth(40);
    double vp[4];
    qtRect2vtkViewport(boundingRect(), vp);
    renderer->SetViewport(vp);

    // Cube Source 1
    vtkNew<vtkCubeSource> cube;

    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(colors->GetColor4d("peacock").GetData());
    renderer->AddActor(cubeActor);
    cubeActor->GetProperty()->SetRepresentationToSurface();

    // Now the chart
    vtkNew<vtkChartXY> chart;
    vtkNew<vtkContextScene> chartScene;
    vtkNew<vtkContextActor> chartActor;

    chart->SetAutoSize(false);
    chart->SetSize(vtkRectf(0.0, 0.0, 320, 220));

    chartScene->AddItem(chart);
    chartActor->SetScene(chartScene);

    // both needed
    renderer->AddActor(chartActor);
    chartScene->SetRenderer(renderer);

    // Create a table with some points in it...
    vtkNew<vtkTable> table;

    vtkNew<vtkFloatArray> arrX;
    arrX->SetName("X Axis");
    table->AddColumn(arrX);

    vtkNew<vtkFloatArray> arrC;
    arrC->SetName("Cosine");
    table->AddColumn(arrC);

    vtkNew<vtkFloatArray> arrS;
    arrS->SetName("Sine");
    table->AddColumn(arrS);

    vtkNew<vtkFloatArray> arrT;
    arrT->SetName("Tan");
    table->AddColumn(arrT);

    // Test charting with a few more points...
    int numPoints = 69;
    float inc = 7.5 / (numPoints - 1);
    table->SetNumberOfRows(numPoints);
    table->SetNumberOfRows(numPoints);
    for (int i = 0; i < numPoints; ++i)
    {
      table->SetValue(i, 0, i * inc);
      table->SetValue(i, 1, cos(i * inc) + 0.0);
      table->SetValue(i, 2, sin(i * inc) + 0.0);
      table->SetValue(i, 3, tan(i * inc) + 0.5);
    }

    // Add multiple line plots, setting the colors etc
    vtkColor3d color3d = colors->GetColor3d("banana");

    vtkPlot* points = chart->AddPlot(vtkChart::POINTS);
    points->SetInputData(table, 0, 1);
    points->SetColor(color3d.GetRed(), color3d.GetGreen(), color3d.GetBlue());
    points->SetWidth(1.0);
    dynamic_cast<vtkPlotPoints*>(points)->SetMarkerStyle(vtkPlotPoints::CROSS);
    points = chart->AddPlot(vtkChart::POINTS);
    points->SetInputData(table, 0, 2);
    points->SetColor(color3d.GetRed(), color3d.GetGreen(), color3d.GetBlue());
    points->SetWidth(1.0);
    dynamic_cast<vtkPlotPoints*>(points)->SetMarkerStyle(vtkPlotPoints::PLUS);
    points = chart->AddPlot(vtkChart::POINTS);
    points->SetInputData(table, 0, 3);
    points->SetColor(color3d.GetRed(), color3d.GetGreen(), color3d.GetBlue());
    points->SetWidth(1.0);

    renwin->SetMultiSamples(0);
  }

  void syncVTK(vtkRenderWindow* renderWindow) override {}
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
