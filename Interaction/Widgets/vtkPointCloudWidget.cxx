/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPointCloudWidget.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPointCloudWidget.h"
#include "vtkPointCloudRepresentation.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkEvent.h"
#include "vtkEventData.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkWidgetCallbackMapper.h"
#include "vtkWidgetEvent.h"
#include "vtkWidgetEventTranslator.h"

vtkStandardNewMacro(vtkPointCloudWidget);

//----------------------------------------------------------------------------
vtkPointCloudWidget::vtkPointCloudWidget()
{
  this->WidgetState = vtkPointCloudWidget::Start;
  this->ManagesCursor = 1;

  // Define widget events
  this->CallbackMapper->SetCallbackMethod(
    vtkCommand::MouseMoveEvent, vtkWidgetEvent::Move, this, vtkPointCloudWidget::MoveAction);

  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent, vtkEvent::NoModifier, 0,
    0, nullptr, vtkWidgetEvent::Select, this, vtkPointCloudWidget::SelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent, vtkEvent::AnyModifier,
    0, 0, nullptr, vtkWidgetEvent::EndSelect, this, vtkPointCloudWidget::EndSelectAction);

  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent, vtkEvent::ShiftModifier, 0,
    0, nullptr, vtkWidgetEvent::Select, this, vtkPointCloudWidget::QueryAction);

  this->CallbackMapper->SetCallbackMethod(vtkCommand::MiddleButtonPressEvent,
    vtkWidgetEvent::Select, this, vtkPointCloudWidget::QueryAction);

  this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent,
    vtkWidgetEvent::Select, this, vtkPointCloudWidget::QueryAction);

}

//----------------------------------------------------------------------------
vtkPointCloudWidget::~vtkPointCloudWidget()
{
}

//----------------------------------------------------------------------------
void vtkPointCloudWidget::SetEnabled(int enabling)
{
  int enabled = this->Enabled;

  // We do this step first because it sets the CurrentRenderer
  this->Superclass::SetEnabled(enabling);

}

//----------------------------------------------------------------------
void vtkPointCloudWidget::MoveAction(vtkAbstractWidget* w)
{
  vtkPointCloudWidget* self = reinterpret_cast<vtkPointCloudWidget*>(w);

  // See whether we're active
  if (self->WidgetState == vtkPointCloudWidget::Start)
  {
    return;
  }

  // compute some info we need for all cases
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  // Okay, adjust the representation
  double e[2];
  e[0] = static_cast<double>(X);
  e[1] = static_cast<double>(Y);
  self->WidgetRep->WidgetInteraction(e);

  // moving something
  self->EventCallbackCommand->SetAbortFlag(1);
  self->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
  self->Render();
}

//----------------------------------------------------------------------
void vtkPointCloudWidget::SelectAction(vtkAbstractWidget* w)
{
  // We are in a static method, cast to ourself
  vtkPointCloudWidget* self = reinterpret_cast<vtkPointCloudWidget*>(w);

  // Get the event position
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  // Okay, make sure that the pick is in the current renderer
  if (!self->CurrentRenderer || !self->CurrentRenderer->IsInViewport(X, Y))
  {
    self->WidgetState = vtkPointCloudWidget::Start;
    return;
  }

  // Begin the widget interaction which has the side effect of setting the
  // interaction state.
  double e[2];
  e[0] = static_cast<double>(X);
  e[1] = static_cast<double>(Y);
  self->WidgetRep->StartWidgetInteraction(e);
  int interactionState = self->WidgetRep->GetInteractionState();
  if (interactionState == vtkPointCloudRepresentation::Outside)
  {
    return;
  }



  // start the interaction
  self->EventCallbackCommand->SetAbortFlag(1);
  self->StartInteraction();
  self->InvokeEvent(vtkCommand::StartInteractionEvent, nullptr);
  self->Render();
}

//----------------------------------------------------------------------
void vtkPointCloudWidget::QueryAction(vtkAbstractWidget* w)
{
  // We are in a static method, cast to ourself
  vtkPointCloudWidget* self = reinterpret_cast<vtkPointCloudWidget*>(w);

  // Get the event position
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  // Okay, make sure that the pick is in the current renderer
  if (!self->CurrentRenderer || !self->CurrentRenderer->IsInViewport(X, Y))
  {
    self->WidgetState = vtkPointCloudWidget::Start;
    return;
  }

  // Begin the widget interaction which has the side effect of setting the
  // interaction state.
  double e[2];
  e[0] = static_cast<double>(X);
  e[1] = static_cast<double>(Y);
  self->WidgetRep->StartWidgetInteraction(e);
  int interactionState = self->WidgetRep->GetInteractionState();
  if (interactionState == vtkPointCloudRepresentation::Outside)
  {
    return;
  }



  // start the interaction
  self->EventCallbackCommand->SetAbortFlag(1);
  self->StartInteraction();
  self->InvokeEvent(vtkCommand::StartInteractionEvent, nullptr);
  self->Render();
}

//----------------------------------------------------------------------
void vtkPointCloudWidget::EndSelectAction(vtkAbstractWidget* w)
{
  vtkPointCloudWidget* self = reinterpret_cast<vtkPointCloudWidget*>(w);
  if (self->WidgetState == vtkPointCloudWidget::Start)
  {
    return;
  }

  // Return state to not active
  self->WidgetState = vtkPointCloudWidget::Start;
  reinterpret_cast<vtkPointCloudRepresentation*>(self->WidgetRep)
    ->SetInteractionState(vtkPointCloudRepresentation::Outside);
  self->ReleaseFocus();

  self->EventCallbackCommand->SetAbortFlag(1);
  self->EndInteraction();
  self->InvokeEvent(vtkCommand::EndInteractionEvent, nullptr);
  self->Render();
}

//----------------------------------------------------------------------
void vtkPointCloudWidget::CreateDefaultRepresentation()
{
  if (!this->WidgetRep)
  {
    this->WidgetRep = vtkPointCloudRepresentation::New();
  }
}

//----------------------------------------------------------------------------
void vtkPointCloudWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

}
