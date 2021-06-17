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
#include "vtkIntArray.h"
#include "vtkLogger.h"
#include "vtkMath.h"
#include "vtkPointData.h"
#include "vtkPointSet.h"
#include "vtkSetGet.h"
#include "vtkStaticPointLocator.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkType.h"
#include <cstdio>
#include <cstring>

vtkStandardNewMacro(vtkBinningFilter);

static const std::string PARTICLES_COUNT_ARRAY_NAME = "NumberOfParticles";
static const std::string PARTICLES_CONCENTRATION_ARRAY_NAME = "Concentration";

//------------------------------------------------------------------------------
vtkBinningFilter::vtkBinningFilter()
{
  this->Dimensions[0] = 20;
  this->Dimensions[1] = 20;
  this->Dimensions[2] = 20;

  this->UseInputBounds = true;
  vtkMath::UninitializeBounds(this->OutputBounds);

  this->ParticleOfInterest = 0;
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
  vtkInformationVector** inVector, vtkInformationVector* outputVector)
{
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // initialize image output
  vtkImageData* output = this->GetOutput();
  output->SetExtent(outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()));
  output->SetSpacing(outInfo->Get(vtkDataObject::SPACING()));
  output->SetOrigin(outInfo->Get(vtkDataObject::ORIGIN()));

  vtkPointSet* input = vtkPointSet::SafeDownCast(this->GetInput());

  auto outCellData = output->GetCellData();
  auto inPointData = input->GetPointData();

  vtkDataArray* particlesType = this->GetInputArrayToProcess(0, inVector);
  if (!particlesType)
  {
    vtkLogF(INFO, "No input array to process. Will not compute concentration.");
  }
  else
  {
    vtkLogF(TRACE, "Will compute concentration based on '%s' array", particlesType->GetName());
  }

  // Initialize output arrays
  vtkIdType nbOfCells = output->GetNumberOfCells();
  auto inIt = vtkCellData::Iterator(inPointData);
  for (vtkDataArray* inArray = inIt.Begin(); !inIt.End(); inArray = inIt.Next())
  {
    // do not forward particle type to output.
    if (inArray == particlesType)
    {
      continue;
    }
    const char* name = inArray->GetName();
    if (vtkDoubleArray::SafeDownCast(inArray))
    {
      vtkNew<vtkDoubleArray> outArray;
      outArray->SetNumberOfComponents(inArray->GetNumberOfComponents());
      outArray->SetNumberOfTuples(nbOfCells);
      outArray->FillValue(0);
      outArray->SetName(name);
      outCellData->AddArray(outArray);
    }
    else if (inArray)
    {
      vtkNew<vtkFloatArray> outArray;
      outArray->SetNumberOfComponents(inArray->GetNumberOfComponents());
      outArray->SetNumberOfTuples(nbOfCells);
      outArray->FillValue(0);
      outArray->SetName(name);
      outCellData->AddArray(outArray);
    }
  }

  vtkNew<vtkIntArray> nbOfParticles;
  nbOfParticles->SetName(PARTICLES_COUNT_ARRAY_NAME.c_str());
  nbOfParticles->SetNumberOfTuples(nbOfCells);
  nbOfParticles->FillValue(0);
  outCellData->AddArray(nbOfParticles);

  if (particlesType)
  {
    vtkNew<vtkFloatArray> concentration;
    concentration->SetName(PARTICLES_CONCENTRATION_ARRAY_NAME.c_str());
    concentration->SetNumberOfTuples(nbOfCells);
    concentration->FillValue(0);
    outCellData->AddArray(concentration);
  }

  this->ComputeCellData(inVector, output);

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
bool vtkBinningFilter::ComputeCellData(vtkInformationVector** inVector, vtkImageData* output)
{
  vtkPointSet* input = vtkPointSet::SafeDownCast(this->GetInput());

  auto outCellData = output->GetCellData();
  auto inPointData = input->GetPointData();

  // get type array
  vtkDataArray* particlesType = this->GetInputArrayToProcess(0, inVector);
  auto countRange =
    vtk::DataArrayValueRange(outCellData->GetArray(PARTICLES_COUNT_ARRAY_NAME.c_str()));

  // Put input points in output cell and sum their contribution
  for (vtkIdType ptId = 0; ptId < input->GetNumberOfPoints(); ptId++)
  {
    double pt[3];
    input->GetPoint(ptId, pt);
    vtkIdType cellId = GetCellId(pt);

    auto inIt = vtkCellData::Iterator(inPointData);
    for (vtkDataArray* inArray = inIt.Begin(); !inIt.End(); inArray = inIt.Next())
    {
      const char* name = inArray->GetName();
      vtkDataArray* outArray = inPointData->GetArray(name);
      if (!inArray)
      {
        continue;
      }
      // count particles of interest
      if (particlesType && strcmp(particlesType->GetName(), inArray->GetName()) == 0)
      {
        auto concentrationRange = vtk::DataArrayValueRange(
          outCellData->GetArray(PARTICLES_CONCENTRATION_ARRAY_NAME.c_str()));
        auto typeRange = vtk::DataArrayValueRange(particlesType);
        if (typeRange[ptId] == this->ParticleOfInterest)
        {
          concentrationRange[cellId]++;
        }
      }
      else
      {
        auto outRange = vtk::DataArrayTupleRange(outArray);
        auto inRange = vtk::DataArrayTupleRange(inArray);
        const vtk::ComponentIdType numComps = inRange.GetTupleSize();
        const auto inTuple = inRange[ptId];
        auto outTuple = outRange[cellId];
        for (vtk::ComponentIdType compId = 0; compId < numComps; ++compId)
        {
          outTuple[compId] += inTuple[compId];
        }
      }
    }
    countRange[cellId] += 1;
  }

  // compute mean for each cell data
  auto outIt = vtkCellData::Iterator(outCellData);
  for (vtkDataArray* outArray = outIt.Begin(); !outIt.End(); outArray = outIt.Next())
  {
    if (strcmp(outArray->GetName(), PARTICLES_COUNT_ARRAY_NAME.c_str()) == 0)
    {
      continue;
    }
    auto arrRange = vtk::DataArrayTupleRange(outArray);
    const vtk::TupleIdType numTuples = arrRange.size();
    const vtk::ComponentIdType numComps = arrRange.GetTupleSize();

    for (vtk::TupleIdType tupleId = 0; tupleId < numTuples; ++tupleId)
    {
      if (countRange[tupleId] == 0)
      {
        continue;
      }

      auto outTuple = arrRange[tupleId];

      for (vtk::ComponentIdType compId = 0; compId < numComps; ++compId)
      {
        outTuple[compId] /= countRange[tupleId];
      }
    }
  }

  return true;
}
