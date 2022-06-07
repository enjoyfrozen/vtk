/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkLagrangianSimpleIntegrationModel.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

    This software is distributed WITHOUT ANY WARRANTY; without even
    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
    PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkLagrangianSimpleIntegrationModel.h"

#include "vtkObjectFactory.h"

#include "vtkMath.h"

vtkObjectFactoryNewMacro(vtkLagrangianSimpleIntegrationModel);

//------------------------------------------------------------------------------
vtkLagrangianSimpleIntegrationModel::vtkLagrangianSimpleIntegrationModel() = default;

//------------------------------------------------------------------------------
vtkLagrangianSimpleIntegrationModel::~vtkLagrangianSimpleIntegrationModel() = default;

//------------------------------------------------------------------------------
void vtkLagrangianSimpleIntegrationModel::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------------
int vtkLagrangianSimpleIntegrationModel::FunctionValues(vtkLagrangianParticle* particle,
  vtkDataSet* dataSet, vtkIdType cellId, double* weights, double* x, double* f)
{
  // Initialize output
  std::fill(f, f + 6, 0.0);

  if (!particle)
  {
    vtkErrorMacro(<< "No particle to integrate");
    return 0;
  }

  // Sanity Check
  if (!dataSet || cellId == -1)
  {
    vtkErrorMacro(<< "No cell or dataset to integrate the particle on. Dataset: " << dataSet
                  << " CellId:" << cellId);
    return 0;
  }

  // Fetch flowVelocity at index 3
  double flowVelocity[3];
  if (this->GetFlowOrSurfaceDataNumberOfComponents(3, dataSet) != 3 ||
    !this->GetFlowOrSurfaceData(particle, 3, dataSet, cellId, weights, flowVelocity))
  {
    vtkErrorMacro(<< "Flow velocity is not set in source flow dataset or "
                     "has incorrect number of components, cannot use Simple equations");
    return 0;
  }
  vtkMath::Normalize(flowVelocity);

  // Compute function values
  for (int i = 0; i < 3; i++)
  {
    // Simple Equation
    f[i] = flowVelocity[i];
    f[i + 3] = flowVelocity[i];
  }
  return 1;
}
