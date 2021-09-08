/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkExtractGhostCells.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkExtractGhostCells.h"

#include "vtkCellData.h"
#include "vtkDataObject.h"
#include "vtkDataSet.h"
#include "vtkDataSetAttributes.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkNew.h"
#include "vtkThreshold.h"
#include "vtkUnsignedCharArray.h"

vtkStandardNewMacro(vtkExtractGhostCells);

//------------------------------------------------------------------------------
vtkExtractGhostCells::vtkExtractGhostCells()
{
  this->SetOutputGhostArrayName("GhostType");
}

//------------------------------------------------------------------------------
vtkExtractGhostCells::~vtkExtractGhostCells()
{
  this->SetOutputGhostArrayName(nullptr);
}

//------------------------------------------------------------------------------
int vtkExtractGhostCells::FillInputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkCompositeDataSet");
  info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}

//----------------------------------------------------------------------------
int vtkExtractGhostCells::RequestData(
  vtkInformation*, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  auto inputDO = vtkDataObject::GetData(inputVector[0], 0);
  auto outputDO = vtkDataObject::GetData(outputVector, 0);

  if (!inputDO)
  {
    return 1;
  }

  vtkDataSet* outputDS = vtkDataSet::SafeDownCast(outputDO);
  if (!outputDS)
  {
    vtkErrorMacro("Output does not downcast to vtkDataSet. Aborting.");
    return 0;
  }

  vtkNew<vtkThreshold> threshold;
  threshold->SetInputData(inputDO);
  // DUPLICATECELL == 1. Any number above that is a ghost.
  threshold->SetUpperThreshold(vtkDataSetAttributes::CellGhostTypes::DUPLICATECELL);
  threshold->SetThresholdFunction(vtkThreshold::THRESHOLD_UPPER);
  threshold->SetInputArrayToProcess(
    0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, vtkDataSetAttributes::GhostArrayName());
  threshold->Update();

  outputDO->ShallowCopy(threshold->GetOutputDataObject(0));

  if (vtkUnsignedCharArray* ghosts = outputDS->GetCellGhostArray())
  {
    // This is fine since vtkThreshold has already created a copy of the array.
    ghosts->SetName(this->OutputGhostArrayName);
  }

  return 1;
}

//----------------------------------------------------------------------------
void vtkExtractGhostCells::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "OutputGhostArrayName: " << this->OutputGhostArrayName << std::endl;
}
