/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGlyphPackingFilter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkGlyphPackingFilter.h"

#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkLogger.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkGlyphPackingFilter);

//----------------------------------------------------------------------------
vtkGlyphPackingFilter::vtkGlyphPackingFilter()
{
}

//----------------------------------------------------------------------------
vtkGlyphPackingFilter::~vtkGlyphPackingFilter()
{
}

//----------------------------------------------------------------------------
int vtkGlyphPackingFilter::
RequestData(vtkInformation* vtkNotUsed(request),
            vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // get the info objects
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // get the input
  vtkPolyData* input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));

  // Initialize
  vtkLog(INFO, "Packing glyphs");

  if (input == nullptr || input->GetPoints() == nullptr)
  {
    return 1;
  }

  return 1;
}

//----------------------------------------------------------------------------
void vtkGlyphPackingFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
