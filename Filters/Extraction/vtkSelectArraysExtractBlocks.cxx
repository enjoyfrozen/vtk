/*=========================================================================

  Program:   ParaView
  Module:    vtkSelectArraysExtractBlocks.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSelectArraysExtractBlocks.h"

#include <vtkCompositeDataSet.h>
#include <vtkExtractBlockUsingDataAssembly.h>
#include <vtkInformation.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPassSelectedArrays.h>

class vtkSelectArraysExtractBlocks::vtkInternals
{
public:
  vtkNew<vtkPassSelectedArrays> PassSelectedArrays;
  vtkNew<vtkExtractBlockUsingDataAssembly> ExtractBlocks;
};

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSelectArraysExtractBlocks);

//----------------------------------------------------------------------------
vtkSelectArraysExtractBlocks::vtkSelectArraysExtractBlocks()
  : Internals(new vtkSelectArraysExtractBlocks::vtkInternals())
{
  // Connect the internal pipeline
  this->Internals->ExtractBlocks->SetInputConnection(
    this->Internals->PassSelectedArrays->GetOutputPort());
}

//----------------------------------------------------------------------------
vtkSelectArraysExtractBlocks::~vtkSelectArraysExtractBlocks() = default;

//----------------------------------------------------------------------------
void vtkSelectArraysExtractBlocks::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------------
int vtkSelectArraysExtractBlocks::FillInputPortInformation(int, vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPartitionedDataSetCollection");
  info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkMultiBlockDataSet");
  info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkUniformGridAMR");
  return 1;
}

//------------------------------------------------------------------------------
int vtkSelectArraysExtractBlocks::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkCompositeDataSet* input = vtkCompositeDataSet::GetData(inputVector[0], 0);
  if (!input)
  {
    vtkErrorMacro("Unable to retrieve the input !");
    return 0;
  }

  vtkCompositeDataSet* output = vtkCompositeDataSet::GetData(outputVector, 0);
  if (!output)
  {
    vtkErrorMacro("Unable to retrieve the output !");
    return 0;
  }

  this->Internals->PassSelectedArrays->SetInputData(input);
  this->Internals->ExtractBlocks->Update();
  output->ShallowCopy(this->Internals->ExtractBlocks->GetOutput());

  return 1;
}

//----------------------------------------------------------------------------
int vtkSelectArraysExtractBlocks::RequestDataObject(
  vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  return this->Internals->ExtractBlocks->ProcessRequest(request, inputVector, outputVector);
}

//----------------------------------------------------------------------------
void vtkSelectArraysExtractBlocks::SetPassArraysEnabled(bool enabled)
{
  this->Internals->PassSelectedArrays->SetEnabled(enabled);
  this->Modified();
}

//----------------------------------------------------------------------------
bool vtkSelectArraysExtractBlocks::GetPassArraysEnabled()
{
  return this->Internals->PassSelectedArrays->GetEnabled();
}

//----------------------------------------------------------------------------
void vtkSelectArraysExtractBlocks::SetExtractBlocksEnabled(bool enabled)
{
  this->Internals->ExtractBlocks->SetEnabled(enabled);
  this->Modified();
}

//----------------------------------------------------------------------------
bool vtkSelectArraysExtractBlocks::GetExtractBlocksEnabled()
{
  return this->Internals->ExtractBlocks->GetEnabled();
}

//----------------------------------------------------------------------------
vtkDataArraySelection* vtkSelectArraysExtractBlocks::GetPointDataArraySelection()
{
  return this->Internals->PassSelectedArrays->GetPointDataArraySelection();
}

//----------------------------------------------------------------------------
vtkDataArraySelection* vtkSelectArraysExtractBlocks::GetCellDataArraySelection()
{
  return this->Internals->PassSelectedArrays->GetCellDataArraySelection();
}

//----------------------------------------------------------------------------
vtkDataArraySelection* vtkSelectArraysExtractBlocks::GetFieldDataArraySelection()
{
  return this->Internals->PassSelectedArrays->GetFieldDataArraySelection();
}

//----------------------------------------------------------------------------
vtkDataArraySelection* vtkSelectArraysExtractBlocks::GetVertexDataArraySelection()
{
  return this->Internals->PassSelectedArrays->GetVertexDataArraySelection();
}

//----------------------------------------------------------------------------
vtkDataArraySelection* vtkSelectArraysExtractBlocks::GetEdgeDataArraySelection()
{
  return this->Internals->PassSelectedArrays->GetEdgeDataArraySelection();
}

//----------------------------------------------------------------------------
vtkDataArraySelection* vtkSelectArraysExtractBlocks::GetRowDataArraySelection()
{
  return this->Internals->PassSelectedArrays->GetRowDataArraySelection();
}

//----------------------------------------------------------------------------
bool vtkSelectArraysExtractBlocks::AddSelector(const char* selector)
{
  if (this->Internals->ExtractBlocks->AddSelector(selector))
  {
    this->Modified();
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
void vtkSelectArraysExtractBlocks::ClearSelectors()
{
  this->Internals->ExtractBlocks->ClearSelectors();
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkSelectArraysExtractBlocks::SetAssemblyName(const char* assemblyName)
{
  this->Internals->ExtractBlocks->SetAssemblyName(assemblyName);
  this->Modified();
}

//----------------------------------------------------------------------------
char* vtkSelectArraysExtractBlocks::GetAssemblyName()
{
  return this->Internals->ExtractBlocks->GetAssemblyName();
}
