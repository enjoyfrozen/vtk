#include "vtkActor.h"
#include "vtkCallbackCommand.h"
#include "vtkCollectionIterator.h"
#include "vtkInteractorStyle3DCursor.h"
#include "vtkObjectFactory.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSphereSource.h"

// Pickers
#include "vtkHardwarePicker.h"
#include "vtkWorldPointPicker.h"

VTK_ABI_NAMESPACE_BEGIN
//------------------------------------------------------------------------------
void PlaceCursorCallBack(vtkObject* vtkNotUsed(caller), long unsigned int vtkNotUsed(eventId),
  void* clientData, void* vtkNotUsed(callData))
{
  vtkInteractorStyle3DCursor* self = reinterpret_cast<vtkInteractorStyle3DCursor*>(clientData);
  if (self)
  {
    self->PlaceCursor();
  }
}

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkInteractorStyle3DCursor);

//------------------------------------------------------------------------------
vtkInteractorStyle3DCursor::vtkInteractorStyle3DCursor()
{
  this->Place3DCursor->SetCallback(PlaceCursorCallBack);
  this->Place3DCursor->SetClientData(this);
  // Ignore focus
  this->Place3DCursor->SetPassiveObserver(true);

  vtkNew<vtkSphereSource> sphere;
  sphere->SetThetaResolution(6);
  sphere->SetPhiResolution(6);
  sphere->SetRadius(0.05);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphere->GetOutputPort());

  this->Cursor->SetMapper(mapper);
  this->Cursor->GetProperty()->SetColor(0.8900, 0.6600, 0.4100);
}

//------------------------------------------------------------------------------
void vtkInteractorStyle3DCursor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------------
void vtkInteractorStyle3DCursor::SetInteractor(vtkRenderWindowInteractor* interactor)
{
  if (interactor == this->Interactor)
  {
    return;
  }

  this->Superclass::SetInteractor(interactor);

  if (this->Interactor) // The interactor has been updated and is not null
  {
    // Add a new passive observer displaying the 3D cursor under the mouse
    this->Interactor->AddObserver(vtkCommand::MouseMoveEvent, this->Place3DCursor, this->Priority);

    // Retrieve current renderer
    this->FindPokedRenderer(0, 0);

    // Fill pickable props list
    this->Picker->PickFromListOn();
    vtkPropCollection* propCollection = this->GetCurrentRenderer()->GetViewProps();
    vtkCollectionSimpleIterator iterator;
    vtkProp* prop = nullptr;
    for (propCollection->InitTraversal(iterator); (prop = propCollection->GetNextProp(iterator));)
    {
      this->Picker->AddPickList(prop);
    }

    // Add cursor actor (not in the pickable list)
    this->GetCurrentRenderer()->AddActor(this->Cursor);

    // Hide cursor (DOES NOT SEEMS TO WORK HERE - vtkXOpenGLRenderWindow)
    this->Interactor->GetRenderWindow()->HideCursor();
  }
}

//------------------------------------------------------------------------------
void vtkInteractorStyle3DCursor::SetCurrentRenderer(vtkRenderer* ren)
{
  this->Superclass::SetCurrentRenderer(ren);
}

//------------------------------------------------------------------------------
void vtkInteractorStyle3DCursor::PlaceCursor()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  this->FindPokedRenderer(x, y);
  this->Picker->Pick(x, y, 0.0, this->GetCurrentRenderer());

  double pos[3] = { 0.0 };
  this->Picker->GetPickPosition(pos);
  this->Cursor->SetPosition(pos);

  this->Interactor->Render();
}

VTK_ABI_NAMESPACE_END
