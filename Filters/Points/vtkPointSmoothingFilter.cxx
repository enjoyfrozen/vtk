/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPointSmoothingFilter.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See LICENSE file for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPointSmoothingFilter.h"

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
#include "vtkSMPThreadLocalObject.h"
#include "vtkSMPTools.h"
#include "vtkStaticPointLocator.h"

vtkStandardNewMacro(vtkPointSmoothingFilter);

vtkCxxSetObjectMacro(vtkPointSmoothingFilter, FrameFieldArray, vtkDataArray);
vtkCxxSetObjectMacro(vtkPointSmoothingFilter, Locator, vtkAbstractPointLocator);


//================= Begin class proper =======================================
//----------------------------------------------------------------------------
vtkPointSmoothingFilter::vtkPointSmoothingFilter()
{
  this->SmoothingMode = DEFAULT_SMOOTHING;
  this->Convergence = 0.0; // runs to number of specified iterations
  this->NumberOfIterations = 20;
  this->RelaxationFactor = .01;
  this->FrameFieldArray = nullptr;

  this->Locator = vtkStaticPointLocator::New();
}

//----------------------------------------------------------------------------
vtkPointSmoothingFilter::~vtkPointSmoothingFilter()
{
  this->SetFrameFieldArray(nullptr);
  this->SetLocator(nullptr);
}

//----------------------------------------------------------------------------
int vtkPointSmoothingFilter::
RequestData(vtkInformation* vtkNotUsed(request),
            vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // get the info objects
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // get the input and output datasets
  vtkSmartPointer<vtkPointSet> input = vtkPointSet::GetData(inputVector[0]);
  vtkPointSet* output = vtkPointSet::GetData(outputVector);

  // Check the input
  vtkIdType numPts=input->GetNumberOfPoints();
  if ( numPts < 1 )
  {
    vtkLog(ERROR, "Points required");
    return 0;
  }
  if (!this->Locator)
  {
    vtkLog(ERROR, "Point locator required\n");
    return 0;
  }

  // Determine the smoothing mode
  vtkPointData *inPD=input->GetPointData(), *outPD=output->GetPointData();
  vtkDataArray *inScalars = inPD->GetScalars();
  vtkDataArray *inTensors = inPD->GetTensors();
  int smoothingMode=GEOMETRIC_SMOOTHING;
  if ( this->SmoothingMode == DEFAULT_SMOOTHING )
  {
    smoothingMode = (inTensors != nullptr ? TENSOR_SMOOTHING :
                   (inScalars != nullptr ? SCALAR_SMOOTHING : GEOMETRIC_SMOOTHING) );
  }
  else if ( this->SmoothingMode == SCALAR_SMOOTHING && inScalars != nullptr )
  {
    smoothingMode = SCALAR_SMOOTHING;
  }
  else if ( this->SmoothingMode == TENSOR_SMOOTHING && inTensors != nullptr )
  {
    smoothingMode = TENSOR_SMOOTHING;
  }
  vtkDebugMacro(<< "Smoothing glyphs: mode is: " << smoothingMode);

  // Copy the input to the output as a starting point. We'll replace
  // the points and update point data later on.
  output->CopyStructure(input);
  output->GetCellData()->PassData(input->GetCellData());

  // We'll build a locator for two purposes: 1) to build a point connectivity
  // list (connections to close points); and 2) interpolate data from the close
  // points.
  this->Locator->SetDataSet(input);
  this->Locator->BuildLocator();






  // Copy point data
  outPD->PassData(inPD);

  // Copy cell data

  return 1;
}

//----------------------------------------------------------------------------
int vtkPointSmoothingFilter::
FillInputPortInformation(int, vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
  return 1;
}

//----------------------------------------------------------------------------
void vtkPointSmoothingFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Smoothing Mode: " << this->SmoothingMode << endl;
  os << indent << "Frame Field Array: " << this->FrameFieldArray << "\n";
}
