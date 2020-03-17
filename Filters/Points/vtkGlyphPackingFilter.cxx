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
#include "vtkLogger.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkDataArray.h"

vtkStandardNewMacro(vtkGlyphPackingFilter);


//================= Begin class proper =======================================
//----------------------------------------------------------------------------
vtkGlyphPackingFilter::vtkGlyphPackingFilter()
{
  this->PackingMode = DEFAULT_PACKING;
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

  // Initialize
  vtkDebugMacro(<< "Packing glyphs!");

  // Make sure that scalars or tensors are available
  vtkPointData *inPD=input->GetPointData(), *outPD=output->GetPointData();
  vtkDataArray *inScalars = inPD->GetScalars();
  vtkDataArray *inTensors = inPD->GetTensors();
  if ( !inScalars && !inTensors )
  {
    vtkLog(ERROR, "Filter requires point scalars or tensors");
    return 1;
  }

  // First, copy the input to the output as a starting point
  output->CopyStructure(input);

  // Copy point data
  outPD->PassData(inPD);

  // Copy cell data
  vtkCellData *inCD=input->GetCellData(), *outCD=output->GetCellData();
  outCD->PassData(inCD);

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
void vtkGlyphPackingFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Packing Mode: " << this->PackingMode << endl;
}
