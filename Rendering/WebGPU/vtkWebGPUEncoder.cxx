/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUEncoder.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkWebGPUEncoder.h"

#include "vtkObjectFactory.h"
#include "vtkWebGPUPipeline.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkCxxSetObjectMacro(vtkWebGPUEncoder, Pipeline, vtkWebGPUPipeline);

//-------------------------------------------------------------------------------------------------
vtkWebGPUEncoder::vtkWebGPUEncoder() {}

//-------------------------------------------------------------------------------------------------
vtkWebGPUEncoder::~vtkWebGPUEncoder()
{
  if (this->Pipeline)
  {
    this->Pipeline->UnRegister(this);
    this->Pipeline = nullptr;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUEncoder::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << " Pipeline = ";
  if (this->Pipeline)
  {
    os << endl;
    this->Pipeline->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << "(null)" << endl;
  }
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
