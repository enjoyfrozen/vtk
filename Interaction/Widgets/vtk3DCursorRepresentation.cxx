/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtk3DCursorRepresentation.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtk3DCursorRepresentation.h"
#include "vtkCollection.h"
#include "vtkCursor3D.h"
#include "vtkHardwarePicker.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLPolyDataMapper.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkViewport.h"

#include <map>
#include <string>

VTK_ABI_NAMESPACE_BEGIN
struct vtk3DCursorRepresentation::vtkInternals
{
  vtkNew<vtkHardwarePicker> Picker;
};

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtk3DCursorRepresentation);

//------------------------------------------------------------------------------
vtk3DCursorRepresentation::vtk3DCursorRepresentation()
  : Internals(new vtk3DCursorRepresentation::vtkInternals())
{
  vtkNew<vtkCursor3D> cross;
  cross->AllOff();
  cross->AxesOn();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(cross->GetOutputPort());
  mapper->Update();

  this->Cursor = vtkSmartPointer<vtkActor>::New();
  this->Cursor->SetMapper(mapper);
  this->Cursor->SetPickable(false);
  this->Cursor->GetProperty()->SetColor(1., 0., 0.);
  this->Cursor->GetProperty()->SetLineWidth(2.);

  this->HandleSize = 15;
  this->ValidPick = true;
}

//------------------------------------------------------------------------------
void vtk3DCursorRepresentation::WidgetInteraction(double newEventPos[2])
{
  if (!this->Renderer)
  {
    return;
  }

  vtkCollectionSimpleIterator cookie;
  vtkActorCollection* actorcol = this->Renderer->GetActors();
  vtkActor* actor;
  std::map<vtkOpenGLPolyDataMapper*, std::string> pointArrayNames;
  std::map<vtkOpenGLPolyDataMapper*, std::string> cellArrayNames;

  // Iterate through mappers to disable the potential use of point/cell data
  // arrays for selection and enforce the use of cell IDs.
  // This is needed in order to prevent a mismatch between the value retrieved
  // with hardware picking and the ID of the point/cell we want to extract before
  // computing the ray intersection (see vtkHardwarePicker).
  for (actorcol->InitTraversal(cookie); (actor = actorcol->GetNextActor(cookie));)
  {
    if (auto* mapper = vtkOpenGLPolyDataMapper::SafeDownCast(actor->GetMapper()))
    {
      if (mapper->GetPointIdArrayName())
      {
        pointArrayNames.emplace(mapper, mapper->GetPointIdArrayName());
        mapper->SetPointIdArrayName(nullptr);
      }
      if (mapper->GetCellIdArrayName())
      {
        cellArrayNames.emplace(mapper, mapper->GetCellIdArrayName());
        mapper->SetCellIdArrayName(nullptr);
      }
    }
  }

  this->Internals->Picker->Pick(newEventPos[0], newEventPos[1], 0.0, this->Renderer);

  // Restore the original cell data arrays after picking
  for (auto item = pointArrayNames.begin(); item != pointArrayNames.end(); item++)
  {
    item->first->SetPointIdArrayName(item->second.c_str());
  }
  for (auto item = cellArrayNames.begin(); item != cellArrayNames.end(); item++)
  {
    item->first->SetCellIdArrayName(item->second.c_str());
  }

  double pos[3] = { 0.0 };
  this->Internals->Picker->GetPickPosition(pos);
  this->Cursor->SetPosition(pos);
}

//------------------------------------------------------------------------------
void vtk3DCursorRepresentation::BuildRepresentation()
{
  // Target size = HandleSize in world coordinates
  double cursorPos[3];
  this->Cursor->GetPosition(cursorPos);
  double targetSize = this->SizeHandlesInPixels(1.0, cursorPos);

  double cursorBounds[6] = { 0.0 };
  this->Cursor->GetBounds(cursorBounds);

  const double sizeRatio = 2 * targetSize / (cursorBounds[1] - cursorBounds[0]);

  // Harware Picker can return NaN position when the ray cast picking do not find any
  // intersection due to floating-point arithmetic imprecisions (for example, when hitting
  // the border of a cell)
  if (std::isnan(sizeRatio))
  {
    return;
  }

  // Rescale the actor to fit the target size
  double scale[3] = { 0.0 };
  this->Cursor->GetScale(scale);
  vtkMath::MultiplyScalar(scale, sizeRatio);
  this->Cursor->SetScale(scale);
}

//------------------------------------------------------------------------------
void vtk3DCursorRepresentation::ReleaseGraphicsResources(vtkWindow* win)
{
  this->Cursor->ReleaseGraphicsResources(win);
}

//------------------------------------------------------------------------------
int vtk3DCursorRepresentation::RenderOpaqueGeometry(vtkViewport* viewport)
{
  this->BuildRepresentation();
  return this->Cursor->RenderOpaqueGeometry(viewport);
}

//------------------------------------------------------------------------------
void vtk3DCursorRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
VTK_ABI_NAMESPACE_END
