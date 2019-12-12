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
#include <vtkPolyDataAlgorithm.h>

class vtkImageData;
class vtkPolyData;
class vtkUnstructuredGrid;
class vtkStreamSurface;
class vtkGradientFilter;

class VTKFILTERSTOPOLOGY_EXPORT vtkVectorFieldTopology : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkVectorFieldTopology, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Construct object to extract the topolofy of a  vector field.
   * The defaults are: MaxNumSteps = 100,
   * IntegrationStepUnit=2, IntegrationStepSize = 1, SeparatrixDistance = 1
   * That means one cell away
   * UseIterativeSeeding = 0 and ComputeSurfaces = 0
   * That means, no surfaces are computed via default
   */
  static vtkVectorFieldTopology* New();

  /**
   * Specify a uniform integration step unit for MinimumIntegrationStep,
   * InitialIntegrationStep, and MaximumIntegrationStep.
   * 1 = LENGTH_UNIT, i.e. all sizes are expresed in coordinate scale or cell scale
   * 2 = CELL_LENGTH_UNIT, i.e. all sizes are expresed in cell scale
   */
  vtkSetMacro(IntegrationStepUnit, int);
  vtkGetMacro(IntegrationStepUnit, int);

  /**
   * Specify/see the maximal number of iterations in this class and in vtkStreamTracer
   */
  vtkSetMacro(MaxNumSteps, int);
  vtkGetMacro(MaxNumSteps, int);

  /**
   * Specify the Initial, minimum, and maximum step size used for line integration,
   * expressed in IntegrationStepUnit
   */
  vtkSetMacro(IntegrationStepSize, double);
  vtkGetMacro(IntegrationStepSize, double);

  /**
   * Specify/see the distance by which the seedpoints of the separatrices are placed away from the
   * saddle expressed in IntegrationStepUnit
   */
  vtkSetMacro(SeparatrixDistance, double);
  vtkGetMacro(SeparatrixDistance, double);

  /**
   * Specify/see if the simple (fast) or iterative (correct) version is called
   */
  vtkSetMacro(UseIterativeSeeding, bool);
  vtkGetMacro(UseIterativeSeeding, bool);

  /**
   * Specify/see if the separatring surfaces (separatrices in 3D) are computed or not
   */
  vtkSetMacro(ComputeSurfaces, bool);
  vtkGetMacro(ComputeSurfaces, bool);

protected:
  vtkVectorFieldTopology();
  ~vtkVectorFieldTopology() override = default;
  int FillInputPortInformation(int port, vtkInformation* info) override;
  int FillOutputPortInformation(int port, vtkInformation* info) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  int IntegrationStepUnit;

private:
  vtkVectorFieldTopology(const vtkVectorFieldTopology&) = delete;
  void operator=(const vtkVectorFieldTopology&) = delete;

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
   * @param integrationStepUnit: whether the sizes are expresed in coordinate scale or cell scale
   * @param dist: size of the offset of the seeding
   * @param stepSize: stepsize of the integrator
   * @param maxNumSteps: maximal number of integration steps
   * @param computeSurfaces: depending on this boolen the separatring surfaces are computed or not
   * @param useIterativeSeeding: depending on this boolen the separatring surfaces  are computed
   * either good or fast
   */
  int ComputeSeparatrices(vtkSmartPointer<vtkPolyData> criticalPoints,
    vtkSmartPointer<vtkPolyData> separatrices, vtkSmartPointer<vtkPolyData> surfaces,
    vtkSmartPointer<vtkImageData> dataset, vtkSmartPointer<vtkImageData> graddataset,
    int integrationStepUnit, double dist, double stepSize, int maxNumSteps, bool computeSurfaces,
    bool useIterativeSeeding);

  /**
   * this method computes streamsurfaces
   * in the plane of the two eigenvectors of the same sign around saddles
   * @param isBackward: is 1 if the integration direction is backward and 0 for forward
   * @param normal: direction along the one eigenvector with opposite sign
   * @param zeroPos: location of the saddle
   * @param streamSurfaces: surfaces that have so far been computed
   * @param dataset: the vector field in which we advect
   * @param integrationStepUnit: whether the sizes are expresed in coordinate scale or cell scale
   * @param dist: size of the offset of the seeding
   * @param stepSize: stepsize of the integrator
   * @param maxNumSteps: maximal number of integration steps
   * @param useIterativeSeeding: depending on this boolen the separatring surfaces  are computed
   * either good or fast
   */
  int ComputeSurface(bool isBackward, double normal[3], double zeroPos[3],
    vtkSmartPointer<vtkPolyData> streamSurfaces, vtkSmartPointer<vtkImageData> dataset,
    int integrationStepUnit, double dist, double stepSize, int maxNumSteps,
    bool useIterativeSeeding);

  /**
   * determine which type of critical point we have based on the eigenvalues of the Jacobian in 3D
   * @param countReal: number of real valued eigenvalues
   * @param countReal: number of complex valued eigenvalues
   * @param countPos: number of positive eigenvalues
   * @param countNeg: number of negative eigenvalues
   */
  int classify3D(int countReal, int countComplex, int countPos, int countNeg);

  /**
   * determine which type of critical point we have based on the eigenvalues of the Jacobian in 2D
   * @param countReal: number of real valued eigenvalues
   * @param countReal: number of complex valued eigenvalues
   * @param countPos: number of positive eigenvalues
   * @param countNeg: number of negative eigenvalues
   */
  int classify2D(int countReal, int countComplex, int countPos, int countNeg);

  /**
   * number of iterations in this class and in vtkStreamTracer
   */
  int MaxNumSteps;

  /**
   * this value is used as stepsize for the integration
   */
  double IntegrationStepSize;

  /**
   * the separatrices are seeded with this offset from the critical points
   */
  double SeparatrixDistance;

  /**
   * depending on this boolen the simple (fast) or iterative (correct) version is called
   */
  bool UseIterativeSeeding;

  /**
   * depending on this boolen the separatring surfaces (separatrices in 3D) are computed or not
   */
  bool ComputeSurfaces;

  vtkNew<vtkStreamSurface> StreamSurface;
  vtkNew<vtkGradientFilter> GradientFilter;
};
#endif
