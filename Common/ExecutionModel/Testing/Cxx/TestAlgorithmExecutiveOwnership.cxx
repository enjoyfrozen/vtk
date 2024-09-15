/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestAlgorithmExecutiveOwnership.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// Test a simple pipeline with one connection.

#include "vtkCellArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkNew.h"
#include "vtkPassInputTypeAlgorithm.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkTrivialProducer.h"

// To avoid linking FiltersCore
class PassThrough : public vtkPassInputTypeAlgorithm
{
public:
  static PassThrough* New();
  vtkTypeMacro(PassThrough, vtkPassInputTypeAlgorithm);

protected:
  PassThrough() = default;
  ~PassThrough() override = default;
  int RequestDataObject(
    vtkInformation* request, vtkInformationVector** inVec, vtkInformationVector* outVec) override
  {
    if (this->GetNumberOfInputPorts() != 0 && inVec[0]->GetInformationObject(0) == nullptr)
    {
      for (int i = 0; i < this->GetNumberOfOutputPorts(); ++i)
      {
        vtkPolyData* obj = vtkPolyData::New();
        outVec->GetInformationObject(i)->Set(vtkDataObject::DATA_OBJECT(), obj);
        obj->FastDelete();
      }
      return 1;
    }
    else
    {
      return this->Superclass::RequestDataObject(request, inVec, outVec);
    }
  }

  int RequestData(vtkInformation* vtkNotUsed(request), vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override
  {
    vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation* outInfo = outputVector->GetInformationObject(0);

    vtkDataObject* input = inInfo->Get(vtkDataObject::DATA_OBJECT());
    vtkDataObject* output = outInfo->Get(vtkDataObject::DATA_OBJECT());
    output->DeepCopy(input);
    return 1;
  }
};
vtkStandardNewMacro(PassThrough);

int TestAlgorithmExecutiveOwnership(int, char*[])
{
  vtkNew<vtkPolyData> pd;
  vtkNew<vtkPoints> points;
  vtkNew<vtkCellArray> tris;
  points->InsertNextPoint(0, 0, 0);
  points->InsertNextPoint(1, 0, 0);
  points->InsertNextPoint(1, 1, 0);
  tris->InsertNextCell({ 0, 1, 2 });
  pd->SetPoints(points);
  pd->SetPolys(tris);

  vtkNew<vtkTrivialProducer> tp;
  tp->SetOutput(pd);
  vtkNew<PassThrough> pass;
  pass->SetInputConnection(tp->GetOutputPort());

  int result;
  // Test ownership
  result = (tp->GetReferenceCount() == 3) ? 0 : 1;
  result |= (pass->GetReferenceCount() == 2) ? 0 : 1;

  // Check whether `vtkAlgorithm` can give back the input executive, connection and port index.
  int algPort = -1;
  result |= (pass->GetInputExecutive(0, 0) == tp->GetExecutive()) ? 0 : 1;
  result |= (pass->GetInputConnection(0, 0) == tp->GetOutputPort()) ? 0 : 1;
  result |= (pass->GetInputAlgorithm(0, 0, algPort) == tp) ? 0 : 1;
  result |= algPort == 0 ? 0 : 1;

  pass->Update();
  vtkPolyData* out = vtkPolyData::SafeDownCast(pass->GetOutput());
  result |= (out->GetNumberOfPoints() > 0) ? 0 : 1;
  result |= (out->GetNumberOfCells() > 0) ? 0 : 1;

  // Check whether `vtkAlgorithm` can remove its input connections by specifying the producer
  // algorithm output.
  pass->RemoveInputConnection(0, tp->GetOutputPort());
  result |= (pass->GetNumberOfInputPorts() == 1) ? 0 : 1;
  result |= (pass->GetNumberOfInputConnections(0) == 0) ? 0 : 1;

  vtkNew<PassThrough> pass2;
  {
    vtkNew<vtkTrivialProducer> tp2;
    tp2->SetOutput(pd);
    pass2->SetInputConnection(tp2->GetOutputPort());
  }
  // the consumer is expected to keep its producer(s) alive.
  result |= (pass2->GetInputAlgorithm(0, 0) != nullptr) ? 0 : 1;
  result |= (pass2->GetInputAlgorithm(0, 0)->GetReferenceCount() == 2) ? 0 : 1;
  result |= (pass2->GetReferenceCount() == 2) ? 0 : 1;

  // Check whether `vtkAlgorithm` can remove its input connections.
  pass2->RemoveInputConnection(0, pass2->GetInputAlgorithm(0, 0)->GetOutputPort());
  result |= (pass2->GetNumberOfInputPorts() == 1) ? 0 : 1;
  result |= (pass2->GetNumberOfInputConnections(0) == 0) ? 0 : 1;

  return result;
}
