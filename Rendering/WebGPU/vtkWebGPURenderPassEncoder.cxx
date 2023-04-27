/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPURenderPassEncoder.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkWebGPURenderPassEncoder.h"

#include "vtkObjectFactory.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPURenderPassEncoder);

//-------------------------------------------------------------------------------------------------
vtkWebGPURenderPassEncoder::vtkWebGPURenderPassEncoder() {}

//-------------------------------------------------------------------------------------------------
vtkWebGPURenderPassEncoder::~vtkWebGPURenderPassEncoder() {}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURenderPassEncoder::PrintSelf(ostream& os, vtkIndent indent)
{
  // os << indent << " = " << this-> << endl;
}
VTK_ABI_NAMESPACE_END
