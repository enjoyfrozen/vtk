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
#include <vtkCellGridCellCenters.h>
#include <vtkCellGridToUnstructuredGrid.h>
#include <vtkArrowSource.h>
#include <vtkGlyph3DMapper.h>

#include <QApplication>
#include <QCheckBox>
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

void updateGlyphSources(
  vtkCellGridCellSource* cellSource,
  QComboBox* attributeSelector)
{
  cellSource->Update();
  int idx = -1;
  auto currentSource = attributeSelector->currentText();
  attributeSelector->clear();
  attributeSelector->addItem("–none–");
  auto* cellGrid = cellSource->GetOutput();
  for (const auto& attribute : cellGrid->GetCellAttributeList())
  {
    if (attribute == cellGrid->GetShapeAttribute()) { continue; }

    if (attribute->GetName().IsValid() && attribute->GetName().HasData())
    {
      auto label = QString::fromStdString(attribute->GetName().Data());
      if (label == currentSource)
      {
        idx = attributeSelector->count();
      }
      attributeSelector->addItem(label);
    }
  }
  if (idx >= 0)
  {
    attributeSelector->setCurrentText(currentSource);
  }
  else
  {
    attributeSelector->setCurrentIndex(0);
  }
}

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
  mainWindow.addDockWidget(Qt::BottomDockWidgetArea, &controlDock);

  QLabel controlDockTitle("Editor");
  controlDockTitle.setMargin(20);
  controlDock.setTitleBarWidget(&controlDockTitle);

  QPointer<QVBoxLayout> dockLayout = new QVBoxLayout();
  QWidget layoutContainer;
  layoutContainer.setLayout(dockLayout);
  controlDock.setWidget(&layoutContainer);

  QLabel cellTypeLabel("Cell Type");
  QComboBox cellType;
  QLabel arrayGroupLabel("Array Group");
  QComboBox arrayGroupSelector;
  QLabel bdyLabel("Boundaries");
  QCheckBox bdyBtn;
  QLabel glyLabel("Glyphs");
  QComboBox glySelector;
  QHBoxLayout hbct;
  QHBoxLayout hbag;
  QHBoxLayout hbbd;
  hbct.addWidget(&cellTypeLabel);
  hbct.addWidget(&cellType);
  hbag.addWidget(&arrayGroupLabel);
  hbag.addWidget(&arrayGroupSelector);
  hbbd.addWidget(&bdyLabel);
  hbbd.addWidget(&bdyBtn);
  hbbd.addWidget(&glyLabel);
  hbbd.addWidget(&glySelector);
  dockLayout->addLayout(&hbct);
  dockLayout->addLayout(&hbag);
  dockLayout->addLayout(&hbbd);
  // dockLayout->addWidget(&cellType);
  // dockLayout->addWidget(&arrayGroupSelector);

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

  ArrayGroupModel model(cellSource->GetOutput(), "points", nullptr);
  tableView.setModel(&model);

  vtkNew<vtkCellGridComputeSides> cellSides;
  cellSides->SetInputDataObject(0, cellSource->GetOutput());
  cellSides->PreserveRenderableInputsOn();
  cellSides->OmitSidesForRenderableInputsOff();

  vtkNew<vtkCellGridMapper> mapper;
  vtkNew<vtkActor> actor;
  mapper->SetInputConnection(cellSides->GetOutputPort());
  actor->SetMapper(mapper);
  actor->GetProperty()->SetEdgeVisibility(true);
  actor->GetProperty()->SetRepresentationToSurface();

  vtkNew<vtkCellGridComputeSides> cellEdges;
  cellEdges->SetInputDataObject(0, cellSource->GetOutput());
  cellEdges->SetOutputDimensionControl(vtkCellGridSidesQuery::SideFlags::EdgesOfInputs);
  cellEdges->PreserveRenderableInputsOff();
  cellEdges->OmitSidesForRenderableInputsOff();

  vtkNew<vtkCellGridMapper> bdyMapper;
  vtkNew<vtkActor> bdyActor;
  bdyMapper->SetInputConnection(cellEdges->GetOutputPort());
  bdyActor->SetMapper(bdyMapper);
  bdyActor->GetProperty()->SetEdgeVisibility(true);
  bdyActor->GetProperty()->SetRepresentationToSurface();
  bdyActor->SetVisibility(false); // Turn off initially.

  vtkNew<vtkCellGridCellCenters> centers;
  vtkNew<vtkCellGridToUnstructuredGrid> ugridCvt;
  vtkNew<vtkGlyph3DMapper> glyMapper;
  vtkNew<vtkArrowSource> arrow;
  vtkNew<vtkActor> glyActor;
  centers->SetInputConnection(cellEdges->GetOutputPort());
  ugridCvt->SetInputConnection(centers->GetOutputPort());
  glyMapper->SetInputConnection(ugridCvt->GetOutputPort());
  glyMapper->OrientOn();
  glyMapper->SetOrientationArray("curl");
  glyMapper->SetSourceConnection(arrow->GetOutputPort());
  glyMapper->ScalingOn();
  glyMapper->SetScaleMode(vtkGlyph3DMapper::SCALE_BY_MAGNITUDE);
  glyMapper->SetScaleArray("curl");
  glyMapper->SetScaleFactor(1.);
  glyActor->SetMapper(glyMapper);
  glyActor->SetVisibility(false);

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->AddActor(bdyActor);
  renderer->AddActor(glyActor);

  renderer->ResetCamera();
  window->AddRenderer(renderer);

  // Re-render upon each user edit of a cell-grid data-array:
  QObject::connect(&model, &ArrayGroupModel::dataChanged, [&vtkRenderWidget]()
    {
      vtkRenderWidget->renderWindow()->Render();
    }
  );

  QObject::connect(&bdyBtn, &QCheckBox::toggled, [&](bool enabled)
    {
      bdyActor->SetVisibility(enabled);
      actor->SetVisibility(!enabled);
      vtkRenderWidget->renderWindow()->Render();
    }
  );

  QObject::connect(&glySelector, &QComboBox::currentTextChanged, [&](const QString& text)
    {
      if (text == QString("–none–"))
      {
        glyActor->SetVisibility(false);
      }
      else
      {
        glyActor->SetVisibility(true);
        glyMapper->SetOrientationArray(text.toStdString().c_str());
        glyMapper->SetScaleArray(text.toStdString().c_str());
      }
      vtkRenderWidget->renderWindow()->Render();
    }
  );

  // connect the buttons
  QObject::connect(&cellType, &QComboBox::currentTextChanged, [&](const QString& text)
    {
      cellSource->SetCellType(text.toStdString().c_str());
      updateGlyphSources(cellSource, &glySelector);
      updateArrayGroups(model, cellSource, &arrayGroupSelector, true);
      vtkRenderWidget->renderWindow()->Render();
    }
  );
  QObject::connect(&arrayGroupSelector, &QComboBox::currentTextChanged, [&](const QString& text)
    {
      model.setGroupName(text.toStdString(), true);
    }
  );
  updateGlyphSources(cellSource, &glySelector);
  updateArrayGroups(model, cellSource, &arrayGroupSelector, false);

  mainWindow.show();

  return app.exec();
}
