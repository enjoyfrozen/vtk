/*=========================================================================

  Program:   Visualization Toolkit

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkOpenXRCamera.h"
#include "vtkOpenXRRenderWindow.h"
#include "vtkOpenXRRenderWindowInteractor.h"
#include "vtkOpenXRRenderer.h"

//------------------------------------------------------------------------------
// Only initialize, requires a OpenXR implementation but do not render anything
int TestOpenXRInitialization(int argc, char* argv[])
{
  vtkNew<vtkOpenXRRenderer> renderer;

  vtkNew<vtkOpenXRRenderWindow> renderWindow;
  vtkNew<vtkOpenXRRenderWindowInteractor> iren;
  vtkNew<vtkOpenXRCamera> cam;

  renderWindow->AddRenderer(renderer);
  vtkNew<vtkActor> actor;
  renderer->AddActor(actor);
  iren->SetRenderWindow(renderWindow);
  renderer->SetActiveCamera(cam);

  return 0;
}
