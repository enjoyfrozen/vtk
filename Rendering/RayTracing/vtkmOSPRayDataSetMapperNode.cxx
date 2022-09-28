/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkmOSPRayDataSetMapperNode.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkmOSPRayDataSetMapperNode.h"

#include "vtkObjectFactory.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkmOSPRayDataSetMapperNode);

//-------------------------------------------------------------------------------------------------
void vtkmOSPRayDataSetMapperNode::Render(bool prepass)
{
  if (prepass)
  {
    // Do the rendering
  }
}

//-------------------------------------------------------------------------------------------------
void vtkmOSPRayDataSetMapperNode::Invalidate(bool prepass)
{
  if (prepass)
  {
    this->RenderTime = 0;
  }
}

//------------------------------------------------------------------------------------------------
void vtkmOSPRayDataSetMapperNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
VTK_ABI_NAMESPACE_END
