/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkBinningFilter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkBinningFilter.h"

#include "vtkCellData.h"
#include "vtkDataObject.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkPointSet.h"
#include "vtkStaticPointLocator.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkType.h"

vtkStandardNewMacro(vtkBinningFilter);

// Internal class
class vtkBinningFilter::Internals
{
public:
  Internals() = default;
  vtkNew<vtkStaticPointLocator> Locator;
};

//------------------------------------------------------------------------------
vtkBinningFilter::vtkBinningFilter()
  : Internal(new vtkBinningFilter::Internals())
{
  this->Dimensions[0] = 20;
  this->Dimensions[1] = 20;
  this->Dimensions[2] = 20;
}

//------------------------------------------------------------------------------
vtkBinningFilter::~vtkBinningFilter()
{
  delete this->Internal;
  this->Internal = nullptr;
}

//------------------------------------------------------------------------------
int vtkBinningFilter::FillInputPortInformation(int, vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
  return 1;
}

//------------------------------------------------------------------------------
int vtkBinningFilter::RequestInformation(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);

  vtkPointSet* input = vtkPointSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));

  int i;
  double spacing[3];
  double origin[3];
  double bounds[6];

  input->GetBounds(bounds);

  // Use Dimensions as the output Extent.
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), 0, this->Dimensions[0] - 1, 0,
    this->Dimensions[1] - 1, 0, this->Dimensions[2] - 1);

  // Use input bounds to set output origin and spacing.
  for (i = 0; i < 3; i++)
  {
    origin[i] = bounds[2 * i];
    if (this->Dimensions[i] <= 1)
    {
      spacing[i] = 1;
    }
    else
    {
      spacing[i] = (bounds[2 * i + 1] - bounds[2 * i]) / (this->Dimensions[i] - 1);
    }
  }
  outInfo->Set(vtkDataObject::ORIGIN(), origin, 3);
  outInfo->Set(vtkDataObject::SPACING(), spacing, 3);

  return 1;
}

//------------------------------------------------------------------------------
int vtkBinningFilter::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);

  // initialize image output
  vtkImageData* output = this->GetOutput();
  output->SetExtent(outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()));
  output->SetSpacing(outInfo->Get(vtkDataObject::SPACING()));
  output->SetOrigin(outInfo->Get(vtkDataObject::ORIGIN()));

  vtkPointSet* input = vtkPointSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  this->Internal->Locator->SetAutomatic(false);
  this->Internal->Locator->SetDivisions(this->Dimensions);
  this->Internal->Locator->SetDataSet(input);
  this->Internal->Locator->BuildLocator();

  vtkIdType nbOfCells = this->Internal->Locator->GetNumberOfBuckets();
  assert(nbOfCells == this->Dimensions[0] * this->Dimensions[1] * this->Dimensions[2]);
  vtkNew<vtkIntArray> numbers;
  numbers->SetName("NumberOfParticles");
  numbers->SetNumberOfTuples(nbOfCells);
  for (vtkIdType id = 0; id < nbOfCells; id++)
  {
    numbers->SetValue(id, this->Internal->Locator->GetNumberOfPointsInBucket(id));
  }
  output->GetCellData()->AddArray(numbers);

  return 1;
}
