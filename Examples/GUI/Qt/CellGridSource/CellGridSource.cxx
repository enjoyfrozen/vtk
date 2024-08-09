// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "ArrayGroupModel.h"

#include <QVTKOpenGLNativeWidget.h>
#include <vtkActor.h>
#include <vtkCellGrid.h>
#include <vtkCellGridMapper.h>
#include <vtkCellMetadata.h>
#include <vtkDoubleArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCellGridCellSource.h>
#include <vtkCellGridComputeSides.h>

#include <QApplication>
#include <QComboBox>
#include <QDockWidget>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPointer>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>

#include <cmath>
#include <cstdlib>
#include <random>

using namespace vtk::literals;

namespace {

void updateArrayGroups(
  ArrayGroupModel& model,
  vtkCellGridCellSource* cellSource,
  QComboBox* groupSelector,
  bool signalChange = true)
{
  cellSource->Update();
  int idx = -1;
  groupSelector->clear();
  auto* cellGrid = cellSource->GetOutput();
  auto curGroupName = model.groupName();
  for (const auto& entry : cellGrid->GetArrayGroups())
  {
    vtkStringToken groupName(static_cast<vtkStringToken::Hash>(entry.first));
    if (groupName.IsValid() && groupName.HasData())
    {
      if (groupName == curGroupName)
      {
        idx = groupSelector->count();
      }
      groupSelector->addItem(QString::fromStdString(groupName.Data()));
    }
  }
  if (idx < 0 && groupSelector->count() > 0)
  {
    idx = 0;
    curGroupName = groupSelector->currentText().toStdString();
  }
  model.setGroupName(curGroupName, signalChange);
}

} // anonymous namespace

int main(int argc, char* argv[])
{
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

  QApplication app(argc, argv);

  // main window
  QMainWindow mainWindow;
  mainWindow.resize(1200, 900);

  // control area
  QDockWidget controlDock;
  mainWindow.addDockWidget(Qt::LeftDockWidgetArea, &controlDock);

  QLabel controlDockTitle("Control Dock");
  controlDockTitle.setMargin(20);
  controlDock.setTitleBarWidget(&controlDockTitle);

  QPointer<QVBoxLayout> dockLayout = new QVBoxLayout();
  QWidget layoutContainer;
  layoutContainer.setLayout(dockLayout);
  controlDock.setWidget(&layoutContainer);

  QComboBox cellType;
  QComboBox arrayGroupSelector;
  dockLayout->addWidget(&cellType);
  dockLayout->addWidget(&arrayGroupSelector);

  QTableView tableView;
  dockLayout->addWidget(&tableView);

  // render area
  QPointer<QVTKOpenGLNativeWidget> vtkRenderWidget = new QVTKOpenGLNativeWidget();
  mainWindow.setCentralWidget(vtkRenderWidget);

  // VTK part
  vtkNew<vtkGenericOpenGLRenderWindow> window;
  vtkRenderWidget->setRenderWindow(window.Get());

  vtkNew<vtkCellGridCellSource> cellSource;
  vtkStringToken initialCellType = "vtkDGHex";
  cellSource->SetCellType(initialCellType.Data().c_str());
  cellSource->Update();
  int idx = 0;
  for (const auto& registeredCellType : vtkCellMetadata::CellTypes())
  {
    if (registeredCellType == initialCellType)
    {
      idx = cellType.count();
    }
    cellType.addItem(QString::fromStdString(registeredCellType.Data()));
  }
  cellType.setCurrentIndex(idx);

  vtkNew<vtkCellGridComputeSides> cellSides;
  cellSides->SetInputConnection(cellSource->GetOutputPort());

  vtkNew<vtkCellGridMapper> mapper;
  mapper->SetInputConnection(cellSides->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetEdgeVisibility(true);
  actor->GetProperty()->SetRepresentationToSurface();

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);

  renderer->ResetCamera();
  window->AddRenderer(renderer);

  ArrayGroupModel model(cellSource, "points", nullptr);
  tableView.setModel(&model);
  // Re-render upon each user edit of a cell-grid data-array:
  QObject::connect(&model, &ArrayGroupModel::dataChanged, [&vtkRenderWidget]()
    {
      vtkRenderWidget->renderWindow()->Render();
    }
  );

  // connect the buttons
  QObject::connect(&cellType, &QComboBox::currentTextChanged, [&](const QString& text)
    {
      cellSource->SetCellType(text.toStdString().c_str());
      updateArrayGroups(model, cellSource, &arrayGroupSelector, true);
      vtkRenderWidget->renderWindow()->Render();
    }
  );
  QObject::connect(&arrayGroupSelector, &QComboBox::currentTextChanged, [&](const QString& text)
    {
      model.setGroupName(text.toStdString(), true);
    }
  );
  updateArrayGroups(model, cellSource, &arrayGroupSelector, false);

  mainWindow.show();

  return app.exec();
}
