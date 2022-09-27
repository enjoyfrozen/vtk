/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkZSpaceHardwarePicker.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkZSpaceHardwarePicker.h"

#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkHardwareSelector.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkSelection.h"
#include "vtkTransform.h"

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkZSpaceHardwarePicker);

vtkSelection* vtkZSpaceHardwarePicker::GetSelection()
{
  return this->Selection;
}

// Setup for picking
void vtkZSpaceHardwarePicker::Initialize()
{
  this->Superclass::Initialize();
}

// Perform the picking
int vtkZSpaceHardwarePicker::PickProp(
  const double p0[3], vtkTransform* dir, int fieldAssociation, vtkRenderer* renderer)
{
  //  Initialize picking process
  this->Initialize();
  this->Renderer = renderer;

  // Invoke start pick method if defined
  this->InvokeEvent(vtkCommand::StartPickEvent, nullptr);

  // Setup hardware selector
  vtkNew<vtkHardwareSelector> sel;
  sel->SetFieldAssociation(fieldAssociation);
  sel->SetRenderer(renderer);
  sel->SetActorPassOnly(false);

  // Save current zSpace camera
  vtkCamera* originalCamera = renderer->GetActiveCamera();

  // Change the camera to disable projection/view matrix of zSpace
  // and to make sure the picked point will be at the middle of the viewport
  vtkNew<vtkCamera> pickingCamera;
  this->GetRenderer()->SetActiveCamera(pickingCamera);

  double pin[4] = { 0.0, 0.0, 1.0, 1.0 };
  double dop[4];
  dir->MultiplyPoint(pin, dop);
  for (int i = 0; i < 3; ++i)
  {
    dop[i] = dop[i] / dop[3];
  }
  vtkMath::Normalize(dop);

  pickingCamera->SetPosition(p0);
  double distance = originalCamera->GetDistance();
  pickingCamera->SetFocalPoint(
    p0[0] + dop[0] * distance, p0[1] + dop[1] * distance, p0[2] + dop[2] * distance);
  pickingCamera->OrthogonalizeViewUp();

  // Set rendering area for capturing buffers
  int pickingRadius = 0;
  if (fieldAssociation == vtkDataObject::FIELD_ASSOCIATION_POINTS)
  {
    // Picking radius to facilitate point picking
    pickingRadius = this->PointPickingRadius;
  }

  int* size = this->Renderer->GetSize();
  int* origin = this->Renderer->GetOrigin();

  // Clamp radius to renderer size, extreme case (certainly useless)
  int xMin = origin[0] + std::max(0, size[0] / 2 - pickingRadius);
  int ymin = origin[1] + std::max(0, size[1] / 2 - pickingRadius);
  int xMax = origin[0] + std::min(size[0] - 1, size[0] / 2 + pickingRadius);
  int yMax = origin[1] + std::min(size[1] - 1, size[1] / 2 + pickingRadius);

  sel->SetArea(xMin, ymin, xMax, yMax);

  // Generate selection
  this->Selection = nullptr;
  if (sel->CaptureBuffers())
  {
    unsigned int outPos[2];
    unsigned int inPos[2] = { static_cast<unsigned int>(size[0] / 2),
      static_cast<unsigned int>(size[1] / 2) };
    // Pick at the center of the viewport (with a tolerance for point picking)
    vtkHardwareSelector::PixelInformation pinfo =
      sel->GetPixelInformation(inPos, pickingRadius, outPos);
    if (pinfo.Valid)
    {
      this->Selection.TakeReference(
        sel->GenerateSelection(outPos[0], outPos[1], outPos[0], outPos[1]));
    }
  }

  // Restore the original zSpace camera
  renderer->SetActiveCamera(originalCamera);

  // Return 1 if something has been picked, 0 otherwise
  if (this->Selection && this->Selection->GetNode(0))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

//------------------------------------------------------------------------------
void vtkZSpaceHardwarePicker::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  if (this->Selection)
  {
    this->Selection->PrintSelf(os, indent);
  }
}
VTK_ABI_NAMESPACE_END
