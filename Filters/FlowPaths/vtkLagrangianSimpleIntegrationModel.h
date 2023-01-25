/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkLagrangianSimpleIntegrationModel.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

    This software is distributed WITHOUT ANY WARRANTY; without even
    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
    PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkLagrangianSimpleIntegrationModel
 * vtkLagrangianBasicIntegrationModel implementation
 *
 *
 * vtkLagrangianBasicIntegrationModel simple implementation using just velocity
 * to move particles around,
 * Input Array to process are expected as follow :
 * Index 1 is the "FlowVelocity" from flow input in the tracker
 *
 * @sa
 * vtkLagrangianParticleTracker vtkLagrangianParticle
 * vtkLagrangianBasicIntegrationModel vtkLagrangianMatidaIntegrationModel
 */

#ifndef vtkLagrangianSimpleIntegrationModel_h
#define vtkLagrangianSimpleIntegrationModel_h

#include "vtkFiltersFlowPathsModule.h" // For export macro
#include "vtkLagrangianBasicIntegrationModel.h"

class VTKFILTERSFLOWPATHS_EXPORT vtkLagrangianSimpleIntegrationModel
  : public vtkLagrangianBasicIntegrationModel
{
public:
  vtkTypeMacro(vtkLagrangianSimpleIntegrationModel, vtkLagrangianBasicIntegrationModel);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  static vtkLagrangianSimpleIntegrationModel* New();

  // Needed for multiple signatures polymorphism
  using Superclass::FunctionValues;

  /**
   * Evaluate the integration model velocity field
   * f at position x, using data from cell in dataSet with index cellId
   */
  int FunctionValues(vtkLagrangianParticle* particle, vtkDataSet* dataSet, vtkIdType cellId,
    double* weights, double* x, double* f) override;

protected:
  vtkLagrangianSimpleIntegrationModel();
  ~vtkLagrangianSimpleIntegrationModel() override;

private:
  vtkLagrangianSimpleIntegrationModel(const vtkLagrangianSimpleIntegrationModel&) = delete;
  void operator=(const vtkLagrangianSimpleIntegrationModel&) = delete;
};

#endif
