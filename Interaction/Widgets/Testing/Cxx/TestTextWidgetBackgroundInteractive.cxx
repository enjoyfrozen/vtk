/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestTextWidgetBackgroundInteractive.cxx

=========================================================================*/
//
// This example tests the vtkTextWidget.

// First include the required header files for the VTK classes we are using.
#include "vtkActor.h"
#include "vtkBorderRepresentation.h"
#include "vtkBorderWidget.h"
#include "vtkCommand.h"
#include "vtkInteractorEventRecorder.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkTextRepresentation.h"
#include "vtkTextWidget.h"

int TestTextWidgetBackgroundInteractive(int, char*[])
{
  // Create the RenderWindow, Renderer and both Actors
  //
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);

  vtkSmartPointer<vtkRenderWindowInteractor> interactor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  interactor->SetRenderWindow(renderWindow);

  // Create a test pipeline
  //
  vtkSmartPointer<vtkSphereSource> ss = vtkSmartPointer<vtkSphereSource>::New();
  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(ss->GetOutputPort());
  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  // Create the widget and its representation
  // Default Border Widget
  vtkSmartPointer<vtkBorderRepresentation> rep = vtkSmartPointer<vtkBorderRepresentation>::New();
  rep->ProportionalResizeOn();
  rep->SetShowBorderToOn();
  // rep->UseBackgroundOn();
  rep->SetBackgroundColor(0.0, 1.0, 0.0);
  rep->SetBackgroundOpacity(0.2);

  vtkSmartPointer<vtkBorderWidget> widget = vtkSmartPointer<vtkBorderWidget>::New();
  widget->SetInteractor(interactor);
  widget->SetRepresentation(rep);
  widget->SelectableOff();

  // Top Left: Default Widget
  auto tlRep = vtkSmartPointer<vtkTextRepresentation>::New();
  tlRep->ProportionalResizeOff();
  tlRep->SetShowBorderToOn();
  tlRep->SetPosition(0.05, 0.75);
  tlRep->SetPosition2(0.3, 0.2);
  tlRep->UseBackgroundOn();
  tlRep->SetBackgroundColor(1.0, 0.0, 0.0);
  tlRep->SetBackgroundOpacity(0.5);

  auto tlWidget = vtkSmartPointer<vtkTextWidget>::New();
  tlWidget->SetInteractor(interactor);
  tlWidget->SetRepresentation(tlRep);

  // Top Right: Always On
  auto trRep = vtkSmartPointer<vtkTextRepresentation>::New();
  trRep->ProportionalResizeOff();
  trRep->SetShowBorderToOn();
  trRep->SetPosition(0.65, 0.75);
  trRep->SetPosition2(0.3, 0.2);
  trRep->UseBackgroundOn();
  trRep->SetBackgroundColor(0.0, 1.0, 0.0);

  auto trWidget = vtkSmartPointer<vtkTextWidget>::New();
  trWidget->SetInteractor(interactor);
  trWidget->SetRepresentation(trRep);

  // Bottom Right: Auto + Always Border
  auto brRep = vtkSmartPointer<vtkTextRepresentation>::New();
  brRep->ProportionalResizeOff();
  brRep->SetShowBorderToActive();
  brRep->SetPosition(0.65, 0.05);
  brRep->SetPosition2(0.3, 0.2);
  brRep->UseBackgroundOn();
  brRep->SetBackgroundColor(1.0, 0.0, 1.0);
  brRep->SetBackgroundOpacity(0.3);
  brRep->EnforceNormalizedViewportBoundsOn();
  brRep->SetMinimumNormalizedViewportSize(0.3, 0.2);

  auto brWidget = vtkSmartPointer<vtkTextWidget>::New();
  brWidget->SetInteractor(interactor);
  brWidget->SetRepresentation(brRep);

  // Add the actors to the renderer, set the background and size
  //
  renderer->AddActor(actor);
  renderer->SetBackground(0.1, 0.2, 0.4);
  renderWindow->SetSize(300, 300);

  // render the image
  //
  interactor->Initialize();
  renderWindow->Render();
  widget->On();
  tlWidget->On();
  trWidget->On();
  brWidget->On();

  interactor->Start();

  return EXIT_SUCCESS;
}
