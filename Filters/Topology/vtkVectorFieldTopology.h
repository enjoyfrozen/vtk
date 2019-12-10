/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVectorFieldTopology.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkVectorFieldTopology
 * @brief   extract the topological skeleton as output datasets
 *
 * the output is the critical points and the 1D separatrices (lines)
 * if the data is 3D and the user enables ComputeSurfaces, also the 2D separatrices are computed
 * (surfaces)
 *
 * @par Thanks:
 * Developed by Roxana Bujack and Karen Tsai at Los Alamos National Laboratory under LDRD 20190143ER
 */

#ifndef vtkVectorFieldTopology_h
#define vtkVectorFieldTopology_h

#include <vtkFiltersTopologyModule.h> // For export macro
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkUnstructuredGrid.h>

class VTKFILTERSTOPOLOGY_EXPORT vtkVectorFieldTopology : public vtkPolyDataAlgorithm
{
public:
  static vtkVectorFieldTopology* New();
  vtkTypeMacro(vtkVectorFieldTopology, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkSetMacro(MaxNumSteps, int);
  vtkGetMacro(MaxNumSteps, int);

  vtkSetMacro(IntegrationStepSize, double);
  vtkGetMacro(IntegrationStepSize, double);

  vtkSetMacro(SeparatrixDistance, double);
  vtkGetMacro(SeparatrixDistance, double);

  vtkSetMacro(UseIterativeSeeding, bool);
  vtkGetMacro(UseIterativeSeeding, bool);

  vtkSetMacro(ComputeSurfaces, bool);
  vtkGetMacro(ComputeSurfaces, bool);

protected:
  vtkVectorFieldTopology();
  int FillInputPortInformation(int port, vtkInformation* info) override;
  int FillOutputPortInformation(int port, vtkInformation* info) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

private:
  vtkVectorFieldTopology(const vtkVectorFieldTopology&) = delete;
  void operator=(const vtkVectorFieldTopology&) = delete;

  /**
   * number of iterations in this class and in vtkStreamTracer
   */
  int MaxNumSteps;

  /**
   * this value is also used as stepsize for the integration
   */
  double IntegrationStepSize;

  /**
   * the separatrices are seeded with this offset from the critical points
   */
  double SeparatrixDistance;

  /**
   * depending on this boolen the simple or iterative version is called
   */
  bool UseIterativeSeeding;

  /**
   * depending on this boolen the separatring surfaces (separatrices in 3D) are computed or not
   */
  bool ComputeSurfaces;

  /**
   * for each triangle, we solve the linear vector field analytically for its zeros
   *  if this location is inside the triangle, we have found a critical point
   * @param criticalPoints: list of the locations where the vf is zero
   * @param tridataset: input vector field after triangulation
   */
  int ComputeCriticalPoints2D(
    vtkSmartPointer<vtkPolyData> criticalPoints, vtkSmartPointer<vtkUnstructuredGrid> tridataset);

  /**
   * for each tetrahedron, we solve the linear vector field analytically for its zeros
   *  if this location is inside the tetrahedron, we have found a critical point
   * @param criticalPoints: list of the locations where the vf is zero
   * @param tridataset: input vector field after tetrahedrization
   */
  int ComputeCriticalPoints3D(
    vtkSmartPointer<vtkPolyData> criticalPoints, vtkSmartPointer<vtkUnstructuredGrid> tridataset);

  /**
   * we classify the critical points based on the eigenvalues of the jacobian
   * for the saddles, we seed in an offset of dist and integrate
   * @param criticalPoints: list of the locations where the vf is zero
   * @param separatrices: inegration lines starting at saddles
   * @param surfaces: inegration surfaces starting at saddles
   * @param dataset: input vector field
   * @param graddataset: the jacobian of the vector field
   * @param dist: size of the offset of the seeding
   * @param stepSize: stepsize of the integrator
   * @param maxNumSteps: maximal number of integration steps
   * @param computeSurfaces: depending on this boolen the separatring surfaces are computed or not
   * @param useIterativeSeeding: depending on this boolen the separatring surfaces  are computed
   * either good or fast
   */
  int ComputeSeparatrices(vtkSmartPointer<vtkPolyData> criticalPoints,
    vtkSmartPointer<vtkPolyData> separatrices, vtkSmartPointer<vtkPolyData> surfaces,
    vtkSmartPointer<vtkImageData> dataset, vtkSmartPointer<vtkImageData> graddataset, double dist,
    double stepSize, int maxNumSteps, bool computeSurfaces, bool useIterativeSeeding);
};
#endif
