/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPointCloudRepresentation.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPointCloudRepresentation.h"
#include "vtkPointSet.h"
#include "vtkCallbackCommand.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkGlyphSource2D.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPropCollection.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkOutlineFilter.h"
#include "vtkActor2D.h"
#include "vtkMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkCoordinate.h"
#include "vtkRenderer.h"
#include "vtkWindow.h"
#include "vtkPicker.h"
#include "vtkPointPicker.h"
#include "vtkPickingManager.h"
#include "vtkInteractorObserver.h"

vtkStandardNewMacro(vtkPointCloudRepresentation);

//-------------------------------------------------------------------------
vtkPointCloudRepresentation::vtkPointCloudRepresentation()
{
  // Internal state
  this->PointCloud = nullptr;
  this->PointCloudActor = nullptr;
  this->Highlighting = true;
  this->PointId = (-1);
  this->PointCoordinates[0] = this->PointCoordinates[1] = this->PointCoordinates[2] = 0.0;
  this->InteractionState = vtkPointCloudRepresentation::Outside;
  this->Tolerance = 2; //in pixels

  // Manage the picking stuff
  this->OutlinePicker = vtkPicker::New();
  this->OutlinePicker->PickFromListOn();
  this->PointPicker = vtkPointPicker::New();
  this->PointPicker->PickFromListOn();

  // The outline around the points
  this->OutlineFilter = vtkOutlineFilter::New();

  this->OutlineMapper = vtkPolyDataMapper::New();
  this->OutlineMapper->SetInputConnection(this->OutlineFilter->GetOutputPort());

  this->OutlineActor = vtkActor::New();
  this->OutlineActor->SetMapper(this->OutlineMapper);

  // Create the selection prop
  this->SelectionShape = vtkGlyphSource2D::New();
  this->SelectionShape->SetGlyphTypeToCircle();
  this->SelectionShape->SetResolution(32);
  this->SelectionShape->SetScale(10);

  this->SelectionCoordinate = vtkCoordinate::New();
  this->SelectionCoordinate->SetCoordinateSystemToDisplay();

  this->SelectionMapper = vtkPolyDataMapper2D::New();
  this->SelectionMapper->SetInputConnection(this->SelectionShape->GetOutputPort());
  //  this->SelectionMapper->SetTransformCoordinate(this->SelectionCoordinate);

  this->SelectionActor = vtkActor2D::New();
  this->SelectionActor->SetMapper(this->SelectionMapper);

  // Set up the initial selection properties
  this->CreateDefaultProperties();
  this->SelectionActor->SetProperty(this->SelectionProperty);
}

//-------------------------------------------------------------------------
vtkPointCloudRepresentation::~vtkPointCloudRepresentation()
{
  if ( this->PointCloud )
  {
    this->PointCloud->Delete();
    this->PointCloudActor->Delete();
  }
  this->OutlinePicker->Delete();
  this->PointPicker->Delete();
  this->OutlineFilter->Delete();
  this->OutlineMapper->Delete();
  this->OutlineActor->Delete();
  this->SelectionShape->Delete();
  this->SelectionCoordinate->Delete();
  this->SelectionMapper->Delete();
  this->SelectionActor->Delete();
  this->SelectionProperty->Delete();
}

//----------------------------------------------------------------------
void vtkPointCloudRepresentation::CreateDefaultProperties()
{
  this->SelectionProperty = vtkProperty2D::New();
  this->SelectionProperty->SetColor(1.0, 1.0, 1.0);
  this->SelectionProperty->SetLineWidth(1.0);
}

//-------------------------------------------------------------------------
void vtkPointCloudRepresentation::SetPointCloudActor(vtkActor *ps)
{
  // Return if nothing has changed
  if ( ps == this->PointCloudActor )
  {
    return;
  }

  // Make sure the prop has associated data of the proper type
  vtkMapper *mapper = ps->GetMapper();
  vtkPointSet *pc = reinterpret_cast<vtkPointSet*>(mapper->GetInput());
  if ( pc == nullptr )
  {
    if (this->PointCloudActor != nullptr )
    {
      this->PointCloud->Delete();
      this->PointCloudActor->Delete();
      this->PointCloud = nullptr;
      this->PointCloudActor = nullptr;
    }
    return;
  }

  // Set up everything
  if ( this->PointCloud != nullptr )
  {
    this->PointCloud->Delete();
    this->PointCloudActor->Delete();
  }
  this->PointCloud = pc;
  this->PointCloudActor = ps;
  this->PointCloud->Register(this);
  this->PointCloudActor->Register(this);

  this->OutlinePicker->InitializePickList();
  this->OutlinePicker->AddPickList(ps);

  this->PointPicker->InitializePickList();
  this->PointPicker->AddPickList(ps);

  this->PlaceWidget(pc->GetBounds());

  this->OutlineFilter->SetInputData(pc);

  this->Modified();
}

//-------------------------------------------------------------------------
double* vtkPointCloudRepresentation::GetBounds()
{
  if ( this->PointCloudActor )
  {
    return this->PointCloudActor->GetBounds();
  }
  else
  {
    return nullptr;
  }
}

//-------------------------------------------------------------------------
void vtkPointCloudRepresentation::BuildRepresentation()
{
  if ( ! this->PointCloud )
  {
    return;
  }

  if (this->GetMTime() > this->BuildTime ||
    (this->Renderer && this->Renderer->GetVTKWindow() &&
      this->Renderer->GetVTKWindow()->GetMTime() > this->BuildTime))
  {

  }

}

//-------------------------------------------------------------------------
int vtkPointCloudRepresentation::
ComputeInteractionState(int X, int Y, int vtkNotUsed(modify))
{
  // First pick the bounding box
  vtkAssemblyPath* path = this->GetAssemblyPath(X, Y, 0., this->OutlinePicker);
  if ( path != nullptr )
  {
    this->OutlineActor->VisibilityOn();
    // Now see if we can pick a point (with the appropriate tolerance)
    //    this->PointPicker->SetTolerance(this->Tolerance);
    this->PointPicker->SetTolerance(0.004);
    path = this->GetAssemblyPath(X, Y, 0., this->PointPicker);
    if (path != nullptr)
    {
      this->InteractionState = vtkPointCloudRepresentation::Over;
      // Create a tolerance and update the pick position
      double center[4], p[4];
      this->PointId = this->PointPicker->GetPointId();
      this->PointPicker->GetPickPosition(p); //in world coordinates
      vtkInteractorObserver::ComputeWorldToDisplay(this->Renderer,p[0],p[1],p[2],center);
      this->PointCoordinates[0] = p[0];
      this->PointCoordinates[1] = p[1];
      this->PointCoordinates[2] = p[2];
      this->SelectionShape->SetCenter(center);
      this->SelectionActor->VisibilityOn();
    }
    else
    {
      this->InteractionState = vtkPointCloudRepresentation::Outside;
      this->PointId = (-1);
      this->SelectionActor->VisibilityOff();
    }
  }
  else
  {
    this->OutlineActor->VisibilityOff();
    this->SelectionActor->VisibilityOff();
  }

  return this->InteractionState;
}

//-------------------------------------------------------------------------
void vtkPointCloudRepresentation::GetActors2D(vtkPropCollection* pc)
{
  pc->AddItem(this->SelectionActor);
  this->Superclass::GetActors2D(pc);
}

//-------------------------------------------------------------------------
void vtkPointCloudRepresentation::ReleaseGraphicsResources(vtkWindow* w)
{
  this->Superclass::ReleaseGraphicsResources(w);
}

//----------------------------------------------------------------------
int vtkPointCloudRepresentation::RenderOpaqueGeometry(vtkViewport* viewport)
{
  if ( this->OutlineActor->GetVisibility() )
  {
    return this->OutlineActor->RenderOpaqueGeometry(viewport);
  }
  return 0;
}

//-----------------------------------------------------------------------------
int vtkPointCloudRepresentation::
RenderTranslucentPolygonalGeometry(vtkViewport* viewport)
{
  if ( this->OutlineActor->GetVisibility() )
  {
    return this->OutlineActor->RenderTranslucentPolygonalGeometry(viewport);
  }
  return 0;
}

//-----------------------------------------------------------------------------
vtkTypeBool vtkPointCloudRepresentation::
HasTranslucentPolygonalGeometry()
{
  return this->OutlineActor->HasTranslucentPolygonalGeometry();
}

//-------------------------------------------------------------------------
int vtkPointCloudRepresentation::RenderOverlay(vtkViewport* v)
{
  int count = 0;
  if (this->PointId != (-1) && this->Highlighting )
  {
    vtkRenderer* ren = vtkRenderer::SafeDownCast(v);
    if (ren)
    {
      count += this->SelectionActor->RenderOverlay(v);
    }
    count += this->Superclass::RenderOverlay(v);
  }
  return count;
}

//----------------------------------------------------------------------
void vtkPointCloudRepresentation::RegisterPickers()
{
  vtkPickingManager* pm = this->GetPickingManager();
  if (!pm)
  {
    return;
  }
  pm->AddPicker(this->PointPicker, this);
}

//-------------------------------------------------------------------------
void vtkPointCloudRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  if ( this->PointCloudActor )
  {
    os << indent << "Point Cloud Actor: " << this->PointCloudActor << "\n";
  }
  else
  {
    os << indent << "Point Cloud Actor: (none)\n";
  }

  os << indent << "Point Id: " << this->PointId << "\n";
  os << indent << "Point Coordinates: ("
     << this->PointCoordinates[0] << ","
     << this->PointCoordinates[1] << ","
     << this->PointCoordinates[2] << ")\n";

  os << indent << "Highlighting: " << (this->Highlighting ? "On" : "Off") << "\n";
  os << indent << "Tolerance: " << this->Tolerance << "\n";

  if (this->SelectionProperty)
  {
    os << indent << "Selection Property: " << this->SelectionProperty << "\n";
  }
  else
  {
    os << indent << "Selection Property: (none)\n";
  }
}
