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

#include "vtkArrayDispatch.h"
#include "vtkArrayDispatch.txx"
#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkDataArrayMeta.h"
#include "vtkDataArrayRange.h"
#include "vtkDataObject.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkIdList.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkPointData.h"
#include "vtkPointSet.h"
#include "vtkStaticPointLocator.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkType.h"
#include <cstdio>

vtkStandardNewMacro(vtkBinningFilter);

struct SumArray
{
  template <typename InArrayT, typename OutArrayT>
  void operator()(InArrayT* inArray, OutArrayT* outArray, vtkIdType ptId, vtkIdType cellId) const
  {
    // TupleRanges iterate tuple-by-tuple:
    const auto inRange = vtk::DataArrayTupleRange(inArray);
    auto outRange = vtk::DataArrayTupleRange(outArray);

    const vtk::ComponentIdType numComps = inRange.GetTupleSize();
    assert(numComps == outRange.GetTupleSize());
    assert(ptId < inRange.size());
    assert(cellId < outRange.size());

    // sum for each tuple
    const auto inTuple = inRange[ptId];
    auto outTuple = outRange[cellId];
    for (vtk::ComponentIdType compId = 0; compId < numComps; ++compId)
    {
      outTuple[compId] += inTuple[compId];
    }
  }
};

//------------------------------------------------------------------------------
vtkBinningFilter::vtkBinningFilter()
{
  this->Dimensions[0] = 20;
  this->Dimensions[1] = 20;
  this->Dimensions[2] = 20;

  this->UseInputBounds = true;
  vtkMath::UninitializeBounds(this->OutputBounds);
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

  if (this->UseInputBounds)
  {
    input->GetBounds(bounds);
  }
  else
  {
    this->GetOutputBounds(bounds);
  }

  // Use Dimensions as the output Extent.
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), 0, this->Dimensions[0], 0,
    this->Dimensions[1], 0, this->Dimensions[2]);

  // use epsilon to avoid missing points on bondary.
  constexpr double epsilon = 1.0001;
  // Use input bounds to set output origin and spacing.
  for (i = 0; i < 3; i++)
  {
    origin[i] = bounds[2 * i];
    if (this->Dimensions[i] <= 1)
    {
      spacing[i] = (bounds[2 * i + 1] - bounds[2 * i]) * epsilon;
    }
    else
    {
      spacing[i] = (bounds[2 * i + 1] - bounds[2 * i]) * epsilon / (this->Dimensions[i]);
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

  auto outCellData = output->GetCellData();
  auto inPointData = input->GetPointData();

  vtkIdType nbOfCells = output->GetNumberOfCells();
  auto inIt = vtkCellData::Iterator(inPointData);
  for (vtkDataArray* inArray = inIt.Begin(); !inIt.End(); inArray = inIt.Next())
  {
    // if (vtkFloatArray::SafeDownCast(inArray))
    // {
    //   auto outArray = vtkFloatArray::SafeDownCast(inArray->NewInstance());
    //   outArray->SetNumberOfTuples(nbOfCells);
    //   outArray->FillValue(0);
    //   outArray->SetName(name);
    //   outCellData->AddArray(outArray);
    //   outArray->Delete();
    // }
    // else if (inArray)
    if (inArray)
    {
      const char* name = inArray->GetName();
      vtkNew<vtkDoubleArray> outArray;
      outArray->SetNumberOfComponents(inArray->GetNumberOfComponents());
      outArray->SetNumberOfTuples(nbOfCells);
      outArray->FillValue(0);
      outArray->SetName(name);
      outCellData->AddArray(outArray);
    }
  }

  this->ComputeCellData(input, output);

  return 1;
}

//------------------------------------------------------------------------------
vtkIdType vtkBinningFilter::GetCellId(double pt[3])
{
  auto image = this->GetOutput();
  double spacing[3];
  image->GetSpacing(spacing);
  double origin[3];
  image->GetOrigin(origin);
  int indices[3];
  for (int i = 0; i < 3; i++)
  {
    indices[i] = vtkMath::Floor((pt[i] - origin[i]) / spacing[i]);
  }

  return image->ComputeCellId(indices);
}

//------------------------------------------------------------------------------
bool vtkBinningFilter::ComputeCellData(vtkPointSet* input, vtkImageData* output)
{
  auto outCellData = output->GetCellData();
  auto inPointData = input->GetPointData();

  vtkIdType nbOfCells = output->GetNumberOfCells();

  vtkNew<vtkIntArray> numbers;
  numbers->SetName("NumberOfParticles");
  numbers->SetNumberOfTuples(nbOfCells);
  numbers->FillValue(0);
  outCellData->AddArray(numbers);

  double origin[3];
  this->GetOutput()->GetOrigin(origin);

  // Put each input point an output cell and add its contribution
  for (vtkIdType ptId = 0; ptId < input->GetNumberOfPoints(); ptId++)
  {
    double pt[3];
    input->GetPoint(ptId, pt);
    vtkIdType cellId = GetCellId(pt);

    auto outIt = vtkCellData::Iterator(outCellData);
    for (vtkDataArray* outArray = outIt.Begin(); !outIt.End(); outArray = outIt.Next())
    {
      const char* name = outArray->GetName();
      vtkDataArray* inArray = inPointData->GetArray(name);
      if (inArray)
      {
        // make summation
        using Dispatcher = vtkArrayDispatch::Dispatch2ByValueType<vtkArrayDispatch::AllTypes,
          vtkArrayDispatch::Reals>;
        SumArray worker;
        if (!Dispatcher::Execute(inArray, outArray, worker, ptId, cellId))
        {
          // If Execute(...) fails, the arrays don't match the constraints.
          // Run the algorithm using the slower vtkDataArray double API instead:
          worker(inArray, outArray, ptId, cellId);
        }
      }
    }
    numbers->SetValue(cellId, numbers->GetValue(cellId) + 1);
  }

  // compute mean for each cell data
  auto outIt = vtkCellData::Iterator(outCellData);
  for (vtkDataArray* outArray = outIt.Begin(); !outIt.End(); outArray = outIt.Next())
  {
    vtkDoubleArray* array = vtkDoubleArray::SafeDownCast(outArray);
    if (!array)
    {
      continue;
    }
    auto arrRange = vtk::DataArrayTupleRange(array);
    const vtk::TupleIdType numTuples = arrRange.size();
    const vtk::ComponentIdType numComps = arrRange.GetTupleSize();

    for (vtk::TupleIdType tupleId = 0; tupleId < numTuples; ++tupleId)
    {
      auto outTuple = arrRange[tupleId];

      for (vtk::ComponentIdType compId = 0; compId < numComps; ++compId)
      {
        outTuple[compId] /= numbers->GetValue(tupleId);
      }
    }
  }

  return true;
}
