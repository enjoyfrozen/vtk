/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestAlgorithmExecutiveOwnership1.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// Test a simple pipeline with multiple connections.

#include "vtkCellArray.h"
#include "vtkInformation.h"
#include "vtkNew.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkTrivialProducer.h"

class TestAlgorithmRepeatableInputs : public vtkAlgorithm
{
public:
  static TestAlgorithmRepeatableInputs* New();
  vtkTypeMacro(TestAlgorithmRepeatableInputs, vtkAlgorithm);

protected:
  TestAlgorithmRepeatableInputs()
  {
    this->SetNumberOfInputPorts(1);
    this->SetNumberOfOutputPorts(1);
  }
  ~TestAlgorithmRepeatableInputs() override = default;
  int FillInputPortInformation(int, vtkInformation* info) override
  {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataObject");
    info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
    return 1;
  }
  int FillOutputPortInformation(int, vtkInformation* info) override
  {
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkDataObject");
    return 1;
  }
};
vtkStandardNewMacro(TestAlgorithmRepeatableInputs);

int TestAlgorithmExecutiveOwnership1(int, char*[])
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

  vtkNew<vtkTrivialProducer> tp0;
  vtkNew<vtkTrivialProducer> tp1;
  vtkNew<vtkTrivialProducer> tp2;
  vtkNew<vtkTrivialProducer> tp3;
  tp0->SetOutput(pd);
  vtkNew<TestAlgorithmRepeatableInputs> testAlg;
  testAlg->AddInputConnection(tp0->GetOutputPort());
  testAlg->AddInputConnection(tp1->GetOutputPort());
  testAlg->AddInputConnection(tp2->GetOutputPort());
  testAlg->AddInputConnection(tp3->GetOutputPort());

  int result;
  // Test ownership
  result = (tp0->GetReferenceCount() == 3) ? 0 : 1;
  result |= (tp1->GetReferenceCount() == 3) ? 0 : 1;
  result |= (tp2->GetReferenceCount() == 3) ? 0 : 1;
  result |= (tp3->GetReferenceCount() == 3) ? 0 : 1;
  result |= (testAlg->GetReferenceCount() == 2) ? 0 : 1;

  // Check whether `vtkAlgorithm` can give back the input executive, connection and port index.
  int algPort = -1;
  result |= (testAlg->GetInputExecutive(0, 0) == tp0->GetExecutive()) ? 0 : 1;
  result |= (testAlg->GetInputConnection(0, 0) == tp0->GetOutputPort()) ? 0 : 1;
  result |= (testAlg->GetInputAlgorithm(0, 0, algPort) == tp0) ? 0 : 1;
  result |= algPort == 0 ? 0 : 1;

  algPort = -1;
  result |= (testAlg->GetInputExecutive(0, 1) == tp1->GetExecutive()) ? 0 : 1;
  result |= (testAlg->GetInputConnection(0, 1) == tp1->GetOutputPort()) ? 0 : 1;
  result |= (testAlg->GetInputAlgorithm(0, 1, algPort) == tp1) ? 0 : 1;
  result |= algPort == 0 ? 0 : 1;

  algPort = -1;
  result |= (testAlg->GetInputExecutive(0, 2) == tp2->GetExecutive()) ? 0 : 1;
  result |= (testAlg->GetInputConnection(0, 2) == tp2->GetOutputPort()) ? 0 : 1;
  result |= (testAlg->GetInputAlgorithm(0, 2, algPort) == tp2) ? 0 : 1;
  result |= algPort == 0 ? 0 : 1;

  algPort = -1;
  result |= (testAlg->GetInputExecutive(0, 3) == tp3->GetExecutive()) ? 0 : 1;
  result |= (testAlg->GetInputConnection(0, 3) == tp3->GetOutputPort()) ? 0 : 1;
  result |= (testAlg->GetInputAlgorithm(0, 3, algPort) == tp3) ? 0 : 1;
  result |= algPort == 0 ? 0 : 1;

  testAlg->Update();

  // Check whether `vtkAlgorithm` can remove its input connections by specifying the producer
  // algorithm output. tests removal in an order different to how they were assigned.
  testAlg->RemoveInputConnection(0, tp0->GetOutputPort());
  result |= (testAlg->GetNumberOfInputPorts() == 1) ? 0 : 1;
  result |= (testAlg->GetNumberOfInputConnections(0) == 3) ? 0 : 1;

  testAlg->RemoveInputConnection(0, tp2->GetOutputPort());
  result |= (testAlg->GetNumberOfInputPorts() == 1) ? 0 : 1;
  result |= (testAlg->GetNumberOfInputConnections(0) == 2) ? 0 : 1;

  testAlg->RemoveInputConnection(0, tp1->GetOutputPort());
  result |= (testAlg->GetNumberOfInputPorts() == 1) ? 0 : 1;
  result |= (testAlg->GetNumberOfInputConnections(0) == 1) ? 0 : 1;

  testAlg->RemoveInputConnection(0, tp3->GetOutputPort());
  result |= (testAlg->GetNumberOfInputPorts() == 1) ? 0 : 1;
  result |= (testAlg->GetNumberOfInputConnections(0) == 0) ? 0 : 1;

  return result;
}
