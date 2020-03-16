/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGlyphPackingFilter.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See LICENSE file for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkGlyphPackingFilter.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkPointSet.h"
#include "vtkPoints.h"

vtkStandardNewMacro(vtkGlyphPackingFilter);


//================= Begin class proper =======================================
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

  // get the input and output datasets
  vtkSmartPointer<vtkPointSet> input = vtkPointSet::GetData(inputVector[0]);
  vtkPointSet* output = vtkPointSet::GetData(outputVector);

  return 1;
}

//----------------------------------------------------------------------------
int vtkGlyphPackingFilter::
FillInputPortInformation(int, vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
  return 1;
}

//----------------------------------------------------------------------------
int vtkGlyphPackingFilter::
RequestDataObject(vtkInformation* request, vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector)
{
  return this->Superclass::RequestDataObject(request, inputVector, outputVector);
}

//----------------------------------------------------------------------------
void vtkGlyphPackingFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

}
