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
#include "vtkCompositeDataSet.h"
#include "vtkDataObject.h"
#include "vtkDataObjectTree.h"
#include "vtkDataObjectTreeRange.h"
#include "vtkDataSet.h"
#include "vtkDataSetAttributes.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkNew.h"
#include "vtkPointData.h"
#include "vtkRange.h"
#include "vtkSmartPointer.h"
#include "vtkThreshold.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnstructuredGrid.h"

#include <vector>

vtkStandardNewMacro(vtkExtractGhostCells);

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
  vtkDataObject* inputDO = vtkDataObject::GetData(inputVector[0], 0);
  vtkDataObject* outputDO = vtkDataObject::GetData(outputVector, 0);

  if (auto dot = vtkDataObjectTree::SafeDownCast(outputDO))
  {
    dot->CopyStructure(vtkDataObjectTree::SafeDownCast(inputDO));

    using Opts = vtk::DataObjectTreeOptions;
    auto outputs = vtk::Range(dot, Opts::None);
    for (auto output : outputs)
    {
      output = vtkSmartPointer<vtkUnstructuredGrid>::New();
    }
  }

  std::vector<vtkDataSet*> inputs = vtkCompositeDataSet::GetDataSets(inputDO);
  std::vector<vtkDataSet*> outputs = vtkCompositeDataSet::GetDataSets(outputDO);

  for (int partitionId = 0; partitionId < static_cast<int>(inputs.size()); ++partitionId)
  {
    vtkDataSet* input = inputs[partitionId];
    vtkDataSet* output = outputs[partitionId];

    if (!input)
    {
      continue;
    }

    auto tmp = vtkSmartPointer<vtkDataSet>::Take(input->NewInstance());
    tmp->ShallowCopy(input);

    vtkUnsignedCharArray* ghosts = tmp->GetCellGhostArray();
    if (!ghosts)
    {
      continue;
    }

    vtkNew<vtkUnsignedCharArray> ghostsWithNewName;
    ghostsWithNewName->ShallowCopy(ghosts);
    ghostsWithNewName->SetName(this->OutputGhostArrayName());

    vtkCellData* tmpCD = tmp->GetCellData();
    tmpCD->RemoveArray(ghosts->GetName());
    tmpCD->AddArray(ghostsWithNewName);

    tmp->GetPointData()->RemoveArray(ghosts->GetName());

    vtkNew<vtkThreshold> threshold;
    threshold->SetInputData(tmp);
    threshold->SetLowerThreshold(1);
    threshold->SetUpperThreshold(255);
    threshold->SetInputArrayToProcess(
      0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, ghostsWithNewName->GetName());
    threshold->Update();

    output->ShallowCopy(threshold->GetOutputDataObject(0));
  }

  return 1;
}

//------------------------------------------------------------------------------
int vtkExtractGhostCells::RequestDataObject(
  vtkInformation*, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  if (this->GetNumberOfInputPorts() == 0 || this->GetNumberOfOutputPorts() == 0)
  {
    return 1;
  }

  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  if (!inInfo)
  {
    return 0;
  }
  vtkDataObject* input = inInfo->Get(vtkDataObject::DATA_OBJECT());

  // This filter internally uses the output of vtkThreshold, which is unstructured grid.
  // If the input is not composite, we output an unstructured grid, and if it is,
  // we pass the input type in the output while changing each output partition to unstructured grid.
  if (vtkDataSet::SafeDownCast(input))
  {
    vtkInformation* info = outputVector->GetInformationObject(0);
    vtkDataObject* output = info->Get(vtkDataObject::DATA_OBJECT());

    if (!output || !output->IsA("vtkUnstructuredGrid"))
    {
      info->Set(vtkDataSet::DATA_OBJECT(), vtkNew<vtkUnstructuredGrid>());
    }
    return 1;
  }
  else if (vtkCompositeDataSet::SafeDownCast(input))
  {
    vtkInformation* info = outputVector->GetInformationObject(0);
    vtkDataObject* output = info->Get(vtkDataObject::DATA_OBJECT());

    if (!output || !output->IsA(input->GetClassName()))
    {
      info->Set(
        vtkDataObject::DATA_OBJECT(), vtkSmartPointer<vtkDataObject>::Take(input->NewInstance()));
    }
    return 1;
  }
  return 0;
}

//----------------------------------------------------------------------------
void vtkExtractGhostCells::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
