// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkCellGridTransform.h"

#include "vtkCellGrid.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkAbstractTransform.h"
#include "vtkLinearTransform.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkSmartPointer.h"

#include <vector>

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkCellGridTransform);
vtkStandardNewMacro(vtkCellGridTransform::Query);
vtkCxxSetObjectMacro(vtkCellGridTransform::Query, CellAttribute, vtkCellAttribute);
vtkCxxSetObjectMacro(vtkCellGridTransform::Query, Transform, vtkAbstractTransform);

vtkCellGridTransform::Query::Query() = default;

vtkCellGridTransform::Query::~Query()
{
  this->SetCellAttribute(nullptr);
  this->SetTransform(nullptr);
}

void vtkCellGridTransform::Query::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "CellAttribute: " << this->CellAttribute << "\n";
  os << indent << "Transform: " << this->Transform << "\n";
  os << indent << "Output Points Precision: " << this->OutputPointsPrecision << "\n";
}

vtkMTimeType vtkCellGridTransform::Query::GetMTime()
{
  vtkMTimeType mTime = this->MTime.GetMTime();
  vtkMTimeType transMTime;

  if (this->Transform)
  {
    transMTime = this->Transform->GetMTime();
    mTime = (transMTime > mTime ? transMTime : mTime);
  }

  return mTime;
}

vtkDataArray* vtkCellGridTransform::Query::CreateNewDataArray(vtkDataArray* input) const
{
  if (this->OutputPointsPrecision == vtkAlgorithm::DEFAULT_PRECISION && input != nullptr)
  {
    return input->NewInstance();
  }

  switch (this->OutputPointsPrecision)
  {
    case vtkAlgorithm::DOUBLE_PRECISION:
      return vtkDoubleArray::New();
    case vtkAlgorithm::SINGLE_PRECISION:
    default:
      return vtkFloatArray::New();
  }
}

// ------------

vtkCellGridTransform::vtkCellGridTransform()
{
}

vtkCellGridTransform::~vtkCellGridTransform()
{
}

void vtkCellGridTransform::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Request:\n";
  vtkIndent i2 = indent.GetNextIndent();
  this->Request->PrintSelf(os, i2);
}

vtkMTimeType vtkCellGridTransform::GetMTime()
{
  auto super = this->Superclass::GetMTime();
  auto req = this->Request->GetMTime();
  return super > req ? super : req;
}

void vtkCellGridTransform::SetTransform(vtkAbstractTransform* tfm)
{
  this->Request->SetTransform(tfm);
}

void vtkCellGridTransform::SetCellAttribute(vtkCellAttribute* att)
{
  this->Request->SetCellAttribute(att);
}

int vtkCellGridTransform::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkSmartPointer<vtkCellGrid> input = vtkCellGrid::GetData(inputVector[0]);
  vtkCellGrid* output = vtkCellGrid::GetData(outputVector);

  if (!input)
  {
    vtkErrorMacro(<< "Invalid or missing input");
    return 0;
  }

  if (!this->Request->GetTransform())
  {
    vtkErrorMacro(<< "No transform provided.");
    return 0;
  }

  output->ShallowCopy(input);
  if (!output->Query(this->Request))
  {
    vtkErrorMacro("Could not transform input.");
    return 0;
  }

  return 1;
#if 0
  vtkPoints* inPts;
  vtkDataArray *inVectors, *inCellVectors;
  vtkDataArray *inNormals, *inCellNormals;
  vtkIdType numPts, numCells;
  vtkPointData *pd = input->GetPointData(), *outPD = output->GetPointData();
  vtkCellData *cd = input->GetCellData(), *outCD = output->GetCellData();

  vtkDebugMacro(<< "Executing transform filter");

  // Check input
  //
  if (this->Transform == nullptr)
  {
    vtkErrorMacro(<< "No transform defined!");
    return 1;
  }

  inPts = input->GetPoints();
  inVectors = pd->GetVectors();
  inNormals = pd->GetNormals();
  inCellVectors = cd->GetVectors();
  inCellNormals = cd->GetNormals();

  if (!inPts)
  {
    return 1;
  }

  // First, copy the input to the output as a starting point
  output->CopyStructure(input);

  numPts = inPts->GetNumberOfPoints();
  numCells = input->GetNumberOfCells();

  // Allocate transformed points
  vtkNew<vtkPoints> newPts;
  // Set the desired precision for the points in the output.
  if (this->OutputPointsPrecision == vtkAlgorithm::DEFAULT_PRECISION)
  {
    newPts->SetDataType(inPts->GetDataType());
  }
  else if (this->OutputPointsPrecision == vtkAlgorithm::SINGLE_PRECISION)
  {
    newPts->SetDataType(VTK_FLOAT);
  }
  else if (this->OutputPointsPrecision == vtkAlgorithm::DOUBLE_PRECISION)
  {
    newPts->SetDataType(VTK_DOUBLE);
  }
  newPts->Allocate(numPts);

  vtkSmartPointer<vtkDataArray> newVectors;
  if (inVectors)
  {
    newVectors.TakeReference(this->CreateNewDataArray(inVectors));
    newVectors->SetNumberOfComponents(3);
    newVectors->Allocate(3 * numPts);
    newVectors->SetName(inVectors->GetName());
  }
  vtkSmartPointer<vtkDataArray> newNormals;
  if (inNormals)
  {
    newNormals.TakeReference(this->CreateNewDataArray(inNormals));
    newNormals->SetNumberOfComponents(3);
    newNormals->Allocate(3 * numPts);
    newNormals->SetName(inNormals->GetName());
  }

  this->UpdateProgress(.2);
  // Loop over all points, updating position
  //

  int nArrays = pd->GetNumberOfArrays();
  std::vector<vtkDataArray*> inVrsArr(nArrays, nullptr);
  std::vector<vtkDataArray*> outVrsArr(nArrays, nullptr);
  int nInputVectors = 0;

  if (this->TransformAllInputVectors)
  {
    vtkSmartPointer<vtkDataArray> tmpOutArray;
    for (int i = 0; i < nArrays; i++)
    {
      if (this->CheckAbort())
      {
        break;
      }
      vtkDataArray* tmpArray = pd->GetArray(i);
      if (tmpArray != inVectors && tmpArray != inNormals && tmpArray->GetNumberOfComponents() == 3)
      {
        inVrsArr[nInputVectors] = tmpArray;
        tmpOutArray.TakeReference(this->CreateNewDataArray(tmpArray));
        tmpOutArray->SetNumberOfComponents(3);
        tmpOutArray->Allocate(3 * numPts);
        tmpOutArray->SetName(tmpArray->GetName());
        outVrsArr[nInputVectors] = tmpOutArray;
        outPD->AddArray(tmpOutArray);
        nInputVectors++;
      }
    }
  }

  if (inVectors || inNormals || nInputVectors > 0)
  {
    this->Transform->TransformPointsNormalsVectors(inPts, newPts, inNormals, newNormals, inVectors,
      newVectors, nInputVectors, inVrsArr.data(), outVrsArr.data());
  }
  else
  {
    this->Transform->TransformPoints(inPts, newPts);
  }

  this->UpdateProgress(.6);

  // Can only transform cell normals/vectors if the transform
  // is linear.
  vtkLinearTransform* lt = vtkLinearTransform::SafeDownCast(this->Transform);
  vtkSmartPointer<vtkDataArray> newCellVectors;
  vtkSmartPointer<vtkDataArray> newCellNormals;
  if (lt)
  {
    if (inCellVectors)
    {
      newCellVectors.TakeReference(this->CreateNewDataArray(inCellVectors));
      newCellVectors->SetNumberOfComponents(3);
      newCellVectors->Allocate(3 * numCells);
      newCellVectors->SetName(inCellVectors->GetName());
      lt->TransformVectors(inCellVectors, newCellVectors);
    }

    if (this->TransformAllInputVectors)
    {
      vtkSmartPointer<vtkDataArray> tmpOutArray;
      for (int i = 0; i < cd->GetNumberOfArrays(); i++)
      {
        if (this->CheckAbort())
        {
          break;
        }
        vtkDataArray* tmpArray = cd->GetArray(i);
        if (tmpArray != inCellVectors && tmpArray != inCellNormals &&
          tmpArray->GetNumberOfComponents() == 3)
        {
          tmpOutArray.TakeReference(this->CreateNewDataArray(tmpArray));
          tmpOutArray->SetNumberOfComponents(3);
          tmpOutArray->Allocate(3 * numCells);
          tmpOutArray->SetName(tmpArray->GetName());
          lt->TransformVectors(tmpArray, tmpOutArray);
          outCD->AddArray(tmpOutArray);
        }
      }
    }

    if (inCellNormals)
    {
      newCellNormals.TakeReference(this->CreateNewDataArray(inCellNormals));
      newCellNormals->SetNumberOfComponents(3);
      newCellNormals->Allocate(3 * numCells);
      newCellNormals->SetName(inCellNormals->GetName());
      lt->TransformNormals(inCellNormals, newCellNormals);
    }
  }

  this->UpdateProgress(.8);

  // Update ourselves and release memory
  //
  output->SetPoints(newPts);

  if (newNormals)
  {
    outPD->SetNormals(newNormals);
    outPD->CopyNormalsOff();
  }

  if (newVectors)
  {
    outPD->SetVectors(newVectors);
    outPD->CopyVectorsOff();
  }

  if (newCellNormals)
  {
    outCD->SetNormals(newCellNormals);
    outCD->CopyNormalsOff();
  }

  if (newCellVectors)
  {
    outCD->SetVectors(newCellVectors);
    outCD->CopyVectorsOff();
  }

  if (this->TransformAllInputVectors)
  {
    for (int i = 0; i < pd->GetNumberOfArrays(); i++)
    {
      if (this->CheckAbort())
      {
        break;
      }
      if (!outPD->GetArray(pd->GetAbstractArray(i)->GetName()))
      {
        outPD->AddArray(pd->GetAbstractArray(i));
        int attributeType = pd->IsArrayAnAttribute(i);
        if (attributeType >= 0 && attributeType != vtkDataSetAttributes::VECTORS &&
          attributeType != vtkDataSetAttributes::NORMALS)
        {
          outPD->SetAttribute(pd->GetAbstractArray(i), attributeType);
        }
      }
    }
    for (int i = 0; i < cd->GetNumberOfArrays(); i++)
    {
      if (!outCD->GetArray(cd->GetAbstractArray(i)->GetName()))
      {
        outCD->AddArray(cd->GetAbstractArray(i));
        int attributeType = pd->IsArrayAnAttribute(i);
        if (attributeType >= 0 && attributeType != vtkDataSetAttributes::VECTORS &&
          attributeType != vtkDataSetAttributes::NORMALS)
        {
          outPD->SetAttribute(pd->GetAbstractArray(i), attributeType);
        }
      }
    }
    // TODO does order matters ?
  }
  else
  {
    outPD->PassData(pd);
    outCD->PassData(cd);
  }

  // Process field data if any
  vtkFieldData* inFD = input->GetFieldData();
  if (inFD)
  {
    vtkFieldData* outFD = output->GetFieldData();
    if (!outFD)
    {
      vtkNew<vtkFieldData> newFD;
      output->SetFieldData(newFD);
    }
    output->GetFieldData()->PassData(inFD);
  }

#endif
}

VTK_ABI_NAMESPACE_END
