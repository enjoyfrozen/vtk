/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkDataArrayPrivateInstantiate6.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#define VTK_COMPUTE_SCALAR_RANGE_IMPL
#include "vtkDataArrayPrivate.txx"

namespace vtkDataArrayPrivate
{
vtkInstantiateComputeScalarRangeMacro(vtkAOSDataArrayTemplate, 6);
}
