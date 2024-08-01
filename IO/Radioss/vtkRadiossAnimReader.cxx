// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkRadiossAnimReader.h"

#include "RadiossAnim.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPartitionedDataSetCollection.h"

#include <vtksys/SystemTools.hxx>

VTK_ABI_NAMESPACE_BEGIN

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkRadiossAnimReader);

//------------------------------------------------------------------------------
vtkRadiossAnimReader::vtkRadiossAnimReader()
{
  this->SetNumberOfInputPorts(0);
}

//------------------------------------------------------------------------------
vtkRadiossAnimReader::~vtkRadiossAnimReader() = default;

//------------------------------------------------------------------------------
void vtkRadiossAnimReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << "FileName: " << (this->FileName.empty() ? "Empty" : this->FileName) << std::endl;
}

//------------------------------------------------------------------------------
int vtkRadiossAnimReader::RequestInformation(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* outputVector)
{
  if (this->FileName.empty())
  {
    vtkErrorMacro("RequestInformation called for RadiossAnimReader without file name");
    return 0;
  }

  RadiossAnim radiossAnim(this->FileName);

  return 1;
}

// ----------------------------------------------------------------------------
int vtkRadiossAnimReader::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* outputVector)
{
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkPartitionedDataSetCollection* output = vtkPartitionedDataSetCollection::GetData(outInfo);

  if (!output)
  {
    vtkErrorMacro("Unable to retrieve the output!");
    return 0;
  }

  return 1;
}

VTK_ABI_NAMESPACE_END
