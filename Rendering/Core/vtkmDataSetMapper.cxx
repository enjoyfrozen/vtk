/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkmDataSetMapper.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// VTK includes
#include "vtkmDataSetMapper.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkmDataSetMapper);

//------------------------------------------------------------------------------------------------
void vtkmDataSetMapper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------------------------------
void vtkmDataSetMapper::Render(vtkRenderer* vtkNotUsed(ren), vtkActor* vtkNotUsed(act))
{
  vtkWarningMacro(<< "VTK is not linked to OSPRay. Cannot render vtkmDataSet directly.");
}

//------------------------------------------------------------------------------
int vtkmDataSetMapper::FillInputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkmDataSet");
  return 1;
}
VTK_ABI_NAMESPACE_END
