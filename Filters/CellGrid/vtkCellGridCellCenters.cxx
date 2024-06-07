// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkCellGridCellCenters.h"

#include "vtkCellGridCopyQuery.h"
#include "vtkDataSetAttributes.h"
#include "vtkIdTypeArray.h"
#include "vtkObjectFactory.h"

#include <sstream>

VTK_ABI_NAMESPACE_BEGIN

vtkStandardNewMacro(vtkCellGridCellCenters);
vtkStandardNewMacro(vtkCellGridCellCenters::Query);

void vtkCellGridCellCenters::Query::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

bool vtkCellGridCellCenters::Query::Initialize()
{
  return true;
}

bool vtkCellGridCellCenters::Query::Finalize()
{
  return true;
}

void vtkCellGridCellCenters::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Query:\n";
  vtkIndent i2 = indent.GetNextIndent();
  this->Request->PrintSelf(os, i2);
}

int vtkCellGridCellCenters::RequestData(
  vtkInformation* vtkNotUsed(request), vtkInformationVector** inInfo, vtkInformationVector* ouInfo)
{
  auto* input = vtkCellGrid::GetData(inInfo[0]);
  auto* output = vtkCellGrid::GetData(ouInfo);
  if (!input)
  {
    vtkWarningMacro("Empty input.");
    return 1;
  }
  if (!output)
  {
    vtkErrorMacro("Empty output.");
    return 0;
  }

  // Copy the input but leave it empty except for cell-attributes.
  vtkNew<vtkCellGridCopyQuery> copyQuery;
  copyQuery->SetSource(input);
  copyQuery->SetTarget(output);
  copyQuery->CopyCellTypesOff();
  copyQuery->CopyCellsOff();
  copyQuery->CopyOnlyShapeOff();
  copyQuery->CopyArraysOff();
  copyQuery->CopyArrayValuesOff();
  copyQuery->CopySchemaOn();
  // copyQuery->AddAllSourceCellAttributeIds();
  if (!input->Query(copyQuery))
  {
    vtkErrorMacro("Could not copy input to output.");
    return 0;
  }

  this->Request->Output = output;
  // Run the cell-center query on the request.
  if (!input->Query(this->Request))
  {
    vtkErrorMacro("Input failed to respond to query.");
    return 0;
  }

  return 1;
}

VTK_ABI_NAMESPACE_END
