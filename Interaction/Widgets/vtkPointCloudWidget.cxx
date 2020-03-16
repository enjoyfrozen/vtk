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

  // Define widget events: translate mouse events to widget events
  this->CallbackMapper->SetCallbackMethod(
    vtkCommand::MouseMoveEvent, vtkWidgetEvent::Move, this, vtkPointCloudWidget::MoveAction);

  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent, vtkEvent::NoModifier, 0,
    0, nullptr, vtkWidgetEvent::Select, this, vtkPointCloudWidget::SelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent, vtkEvent::AnyModifier,
    0, 0, nullptr, vtkWidgetEvent::EndSelect, this, vtkPointCloudWidget::EndSelectAction);
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

  // See whether we're active, return if we are in the middle of something
  // (i.e., a selection process).
  if (self->WidgetState == vtkPointCloudWidget::Active)
  {
    return;
  }

  // compute some info we need for all cases
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  // Retrieve the current point id before computing the interaction state. Invoke an
  // event only when a new point is selected.
  vtkIdType ptId = reinterpret_cast<vtkPointCloudRepresentation*>(self->WidgetRep)->GetPointId();
  int state = self->WidgetRep->ComputeInteractionState(X,Y);
  if ( ptId == reinterpret_cast<vtkPointCloudRepresentation*>(self->WidgetRep)->GetPointId() )
  {
    return;
  }

  // Something new has been picked
  if ( state == vtkPointCloudRepresentation::Over )
  {
    self->InvokeEvent(vtkCommand::PickEvent, nullptr);
  }

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

  // Only can select if we are over a point
  int state = self->WidgetRep->GetInteractionState();
  if ( state != vtkPointCloudRepresentation::Over )
  {
    return;
  }

  // Compare the old state to the new computed state. If nothing has
  // changed, just return.
  if ( state == self->WidgetRep->ComputeInteractionState(X,Y))
  {
    return;
  }

  // We are definitely selected
  self->WidgetState = vtkPointCloudWidget::Active;
  if (!self->Parent)
  {
    self->GrabFocus(self->EventCallbackCommand);
  }

  // Something has changed, so render to see the changes
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
