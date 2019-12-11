/*=========================================================================

 Program:   Visualization Toolkit
 Module:    vtkPolyDataAlgorithm.h

 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/
#include <vtkStreamSurface.h>

#include <vtkAppendPolyData.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkRuledSurfaceFilter.h>
#include <vtkStreamTracer.h>
#include <vtkTriangle.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkStreamSurface);

//----------------------------------------------------------------------------
vtkStreamSurface::vtkStreamSurface()
{
  this->UseIterativeSeeding = false;
  this->vtkRuledSurface = vtkNew<vtkRuledSurfaceFilter>();
  this->streamTracer = vtkNew<vtkStreamTracer>();
  this->appendSurfaces = vtkNew<vtkAppendPolyData>();
}

//----------------------------------------------------------------------------
void vtkStreamSurface::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "UseIterativeSeeding =  " << this->UseIterativeSeeding << "\n";
}

//----------------------------------------------------------------------------
void vtkStreamSurface::AdvectIterative(vtkImageData* field, vtkPolyData* seeds, vtkPolyData* output)
{
  vtkNew<vtkPolyData> currentSeeds;
  currentSeeds->ShallowCopy(seeds);
  vtkNew<vtkDoubleArray> seedIntegrationTimeArray;
  seedIntegrationTimeArray->SetName("IntegrationTime");
  seedIntegrationTimeArray->SetNumberOfTuples(currentSeeds->GetNumberOfPoints());
  seedIntegrationTimeArray->Fill(0.0);
  currentSeeds->GetPointData()->AddArray(seedIntegrationTimeArray);

  for (int currentIteration = 0; currentIteration < this->MaximumNumberOfSteps; currentIteration++)
  {
    // advect currentSeeds
    // the output will be ordered: 0, advect(0), 1, advect(1), 2...
    // but if a point reaches the boundary, its advected point is just missing
    this->streamTracer->SetInputData(field);
    this->streamTracer->SetSourceData(currentSeeds);
    this->streamTracer->SetIntegratorType(this->GetIntegratorType());
    this->streamTracer->SetIntegrationStepUnit(this->IntegrationStepUnit);
    this->streamTracer->SetInitialIntegrationStep(this->InitialIntegrationStep);
    this->streamTracer->SetIntegrationDirection(this->IntegrationDirection);
    this->streamTracer->SetComputeVorticity(this->ComputeVorticity);
    this->streamTracer->SetMaximumNumberOfSteps(0);
    this->streamTracer->SetMaximumPropagation(this->MaximumPropagation);
    this->streamTracer->Update();

    // fill in points that were not advected because they reached the boundary
    // i.e. copy a point k with integrationtime(k)==0 if its successor also has
    // integrationtime(k+1)==0
    vtkNew<vtkPolyData> orderedSurface;
    vtkNew<vtkPoints> orderedSurfacePoints;
    vtkNew<vtkCellArray> orderedSurfaceCells;
    orderedSurface->SetPoints(orderedSurfacePoints);
    orderedSurface->SetPolys(orderedSurfaceCells);

    vtkNew<vtkDoubleArray> integrationTimeArray;
    integrationTimeArray->SetName("IntegrationTime");
    orderedSurface->GetPointData()->AddArray(integrationTimeArray);

    int currentCircleIndex = -1;
    int numPts = this->streamTracer->GetOutput()->GetNumberOfPoints() - 1;
    for (int k = 0; k < numPts; k++)
    {
      if (this->streamTracer->GetOutput()
            ->GetPointData()
            ->GetArray("IntegrationTime")
            ->GetTuple1(k) == 0)
      {
        currentCircleIndex++;
      }
      orderedSurfacePoints->InsertNextPoint(this->streamTracer->GetOutput()->GetPoint(k));
      integrationTimeArray->InsertNextTuple1(
        this->streamTracer->GetOutput()->GetPointData()->GetArray("IntegrationTime")->GetTuple1(k) +
        currentSeeds->GetPointData()->GetArray("IntegrationTime")->GetTuple1(currentCircleIndex));

      if (this->streamTracer->GetOutput()
            ->GetPointData()
            ->GetArray("IntegrationTime")
            ->GetTuple1(k) == 0)
      {
        if (this->streamTracer->GetOutput()
              ->GetPointData()
              ->GetArray("IntegrationTime")
              ->GetTuple1(k + 1) == 0)
        {
          orderedSurfacePoints->InsertNextPoint(this->streamTracer->GetOutput()->GetPoint(k));
          integrationTimeArray->InsertNextTuple1(currentSeeds->GetPointData()
                                                   ->GetArray("IntegrationTime")
                                                   ->GetTuple1(currentCircleIndex));
        }
      }
    }

    orderedSurfacePoints->InsertNextPoint(this->streamTracer->GetOutput()->GetPoint(numPts));
    integrationTimeArray->InsertNextTuple1(this->streamTracer->GetOutput()
                                             ->GetPointData()
                                             ->GetArray("IntegrationTime")
                                             ->GetTuple1(numPts) +
      currentSeeds->GetPointData()->GetArray("IntegrationTime")->GetTuple1(currentCircleIndex));
    if (this->streamTracer->GetOutput()
          ->GetPointData()
          ->GetArray("IntegrationTime")
          ->GetTuple1(numPts) == 0)
    {
      orderedSurfacePoints->InsertNextPoint(this->streamTracer->GetOutput()->GetPoint(numPts));
      integrationTimeArray->InsertNextTuple1(
        currentSeeds->GetPointData()->GetArray("IntegrationTime")->GetTuple1(currentCircleIndex));
    }

    // add arrays
    vtkNew<vtkDoubleArray> iterationArray;
    iterationArray->SetName("iteration");
    iterationArray->SetNumberOfTuples(orderedSurface->GetNumberOfPoints());
    iterationArray->Fill(currentIteration);
    orderedSurface->GetPointData()->AddArray(iterationArray);

    vtkNew<vtkDoubleArray> indexArray;
    indexArray->SetName("index");
    indexArray->SetNumberOfTuples(orderedSurface->GetNumberOfPoints());
    orderedSurface->GetPointData()->AddArray(indexArray);
    for (int k = 0; k < orderedSurface->GetNumberOfPoints(); k++)
    {
      indexArray->SetTuple1(k, k);
    }

    // insert cells
    for (int k = 0; k < orderedSurface->GetNumberOfPoints() - 2; k += 2)
    {
      if (std::abs(orderedSurface->GetPointData()->GetArray("IntegrationTime")->GetTuple1(k + 1) -
            orderedSurface->GetPointData()->GetArray("IntegrationTime")->GetTuple1(k)) > 1e-10 &&
        std::abs(orderedSurface->GetPointData()->GetArray("IntegrationTime")->GetTuple1(k + 3) -
          orderedSurface->GetPointData()->GetArray("IntegrationTime")->GetTuple1(k + 2)) > 1e-10)
      {
        double p0[3];
        orderedSurface->GetPoint(k, p0);
        double p1[3];
        orderedSurface->GetPoint(k + 1, p1);
        double p2[3];
        orderedSurface->GetPoint(k + 2, p2);
        double p3[3];
        orderedSurface->GetPoint(k + 3, p3);
        vtkNew<vtkTriangle> triangle1;
        vtkNew<vtkTriangle> triangle2;

        // make the triangles across the shorter diagonal
        if (sqrt(vtkMath::Distance2BetweenPoints(p0, p3)) >
          sqrt(vtkMath::Distance2BetweenPoints(p1, p2)))
        {
          triangle1->GetPointIds()->SetId(0, k);
          triangle1->GetPointIds()->SetId(1, k + 1);
          triangle1->GetPointIds()->SetId(2, k + 2);

          triangle2->GetPointIds()->SetId(0, k + 1);
          triangle2->GetPointIds()->SetId(1, k + 3);
          triangle2->GetPointIds()->SetId(2, k + 2);
        }
        else
        {
          triangle1->GetPointIds()->SetId(0, k);
          triangle1->GetPointIds()->SetId(1, k + 3);
          triangle1->GetPointIds()->SetId(2, k + 2);

          triangle2->GetPointIds()->SetId(0, k);
          triangle2->GetPointIds()->SetId(1, k + 1);
          triangle2->GetPointIds()->SetId(2, k + 3);
        }
        orderedSurfaceCells->InsertNextCell(triangle1);
        orderedSurfaceCells->InsertNextCell(triangle2);
      }
    }

    // adaptively insert new points where neighbors have diverged
    vtkNew<vtkPoints> newCirclePoints;
    currentSeeds->SetPoints(newCirclePoints);
    vtkNew<vtkDoubleArray> newIntegrationTimeArray;
    newIntegrationTimeArray->SetName("IntegrationTime");
    currentSeeds->GetPointData()->RemoveArray("IntegrationTime");
    currentSeeds->GetPointData()->AddArray(newIntegrationTimeArray);
    for (int k = 0; k < orderedSurface->GetNumberOfPoints() - 2; k += 2)
    {
      newCirclePoints->InsertNextPoint(orderedSurface->GetPoint(k + 1));
      newIntegrationTimeArray->InsertNextTuple1(
        orderedSurface->GetPointData()->GetArray("IntegrationTime")->GetTuple1(k + 1));

      double p0[3];
      orderedSurface->GetPoint(k + 1, p0);
      double p1[3];
      orderedSurface->GetPoint(k + 3, p1);

      if (sqrt(vtkMath::Distance2BetweenPoints(p0, p1)) > this->InitialIntegrationStep &&
        std::abs(orderedSurface->GetPointData()->GetArray("IntegrationTime")->GetTuple1(k + 1) -
          orderedSurface->GetPointData()->GetArray("IntegrationTime")->GetTuple1(k)) > 1e-10 &&
        std::abs(orderedSurface->GetPointData()->GetArray("IntegrationTime")->GetTuple1(k + 3) -
          orderedSurface->GetPointData()->GetArray("IntegrationTime")->GetTuple1(k + 2)) > 1e-10)
      {
        newCirclePoints->InsertNextPoint(
          (p0[0] + p1[0]) / 2, (p0[1] + p1[1]) / 2, (p0[2] + p1[2]) / 2);
        newIntegrationTimeArray->InsertNextTuple1(
          (orderedSurface->GetPointData()->GetArray("IntegrationTime")->GetTuple1(k + 1) +
            orderedSurface->GetPointData()->GetArray("IntegrationTime")->GetTuple1(k + 3)) /
          2);
      }
    }
    newCirclePoints->InsertNextPoint(
      orderedSurface->GetPoint(orderedSurface->GetNumberOfPoints() - 1));
    newIntegrationTimeArray->InsertNextTuple1(
      orderedSurface->GetPointData()
        ->GetArray("IntegrationTime")
        ->GetTuple1(orderedSurface->GetNumberOfPoints() - 1));

    // add current surface strip to the so far computed stream surface
    this->appendSurfaces->RemoveAllInputs();
    this->appendSurfaces->AddInputData(orderedSurface);
    this->appendSurfaces->AddInputData(output);
    this->appendSurfaces->Update();
    output->ShallowCopy(this->appendSurfaces->GetOutput());

    // stop criterion if all points have left the boundary
    if (this->streamTracer->GetOutput()
          ->GetPointData()
          ->GetArray("IntegrationTime")
          ->GetRange()[1 - this->IntegrationDirection] == 0)
    {
      cout << currentIteration << " surface stagnates at IntegrationTime "
           << currentSeeds->GetPointData()
                ->GetArray("IntegrationTime")
                ->GetRange()[1 - this->IntegrationDirection]
           << endl;
      break;
    }
    if (currentSeeds == nullptr)
    {
      cout << "circle is empty." << endl;
      break;
    }
  }
}

//----------------------------------------------------------------------------
void vtkStreamSurface::AdvectSimple(vtkImageData* field, vtkPolyData* seeds, vtkPolyData* output)
{
  //  this is for comparison with the standard ruled surface
  this->streamTracer->SetInputData(field);
  this->streamTracer->SetSourceData(seeds);
  this->streamTracer->SetIntegratorType(this->GetIntegratorType());
  this->streamTracer->SetIntegrationStepUnit(this->IntegrationStepUnit);
  this->streamTracer->SetInitialIntegrationStep(this->InitialIntegrationStep);
  this->streamTracer->SetIntegrationDirection(this->IntegrationDirection);
  this->streamTracer->SetComputeVorticity(this->ComputeVorticity);
  this->streamTracer->SetMaximumNumberOfSteps(this->MaximumNumberOfSteps);
  this->streamTracer->SetMaximumPropagation(this->MaximumPropagation);

  this->vtkRuledSurface->SetInputConnection(streamTracer->GetOutputPort());
  this->vtkRuledSurface->SetRuledModeToResample();
  this->vtkRuledSurface->SetResolution(this->MaximumNumberOfSteps, 1);
  this->vtkRuledSurface->Update();

  output->ShallowCopy(this->vtkRuledSurface->GetOutput());
}

//----------------------------------------------------------------------------
int vtkStreamSurface::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // get the info objects
  vtkInformation* fieldInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* seedsInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // get the input and output
  vtkImageData* field = vtkImageData::SafeDownCast(fieldInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData* seeds = vtkPolyData::SafeDownCast(seedsInfo->Get(vtkDataObject::DATA_OBJECT()));

  // make output
  vtkPolyData* output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (this->UseIterativeSeeding)
  {
    AdvectIterative(field, seeds, output);
  }
  else
  {
    AdvectSimple(field, seeds, output);
  }
  return 1;
}
