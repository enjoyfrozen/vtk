/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPointFeatures.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkPointFeatures
 * @brief   extract geometry from data (or convert data to polygonal type)
 *
 * vtkPointFeatures is a general-purpose filter to extract points of
 * interest from an unstructured grid or polydata object.
 * These points correspond to 4 types of geometric surface features:
 * + points on the boundary of a surface manifold.
 * + points where geometry is non-manifold
 * + points along a ridge line (where face dihedral angle is high along
 *   a pair of edges incident to the point)
 * + points that are corners (where face dihedral angle is high along 3
 *   or more incident edges)
 * The output of this filter is a polydata whose points each have a
 * normal vector related to the geometric feature. The same point
 * coordinates may appear multiple times with different normal vectors.
 * Output points will have a pedigree ID specifying the corresponding
 * point from the input data object.
 */
#ifndef vtkPointFeatures_h
#define vtkPointFeatures_h

#include "vtkFiltersParallelDIY2Module.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class VTKFILTERSPARALLELDIY2_EXPORT vtkPointFeatures : public vtkPolyDataAlgorithm
{
public:
  static vtkPointFeatures* New();
  vtkTypeMacro(vtkPointFeatures, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set the feature angle (dihedral angle between faces of incident edges)
   * that determines whether a point should be considered a feature or not.
   * The default is 30 degrees.
   */
  vtkSetMacro(FeatureAngle, double);
  vtkGetMacro(FeatureAngle, double);
  //@}

protected:
  vtkPointFeatures();
  ~vtkPointFeatures() override;

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int FillInputPortInformation(int port, vtkInformation* info) override;

  int FeatureAngle;

private:
  template<typename EdgeFeatureSet>
  void EdgePoints(vtkPolyData*, EdgeFeatureSet&);

  template<typename SurfacePointMap, typename EdgeFeatureSet>
  void SurfaceCorners(vtkPolyData*, EdgeFeatureSet&, SurfacePointMap&);

  template<typename SurfacePointMap>
  void EdgeCorners(vtkPolyData*, SurfacePointMap&);
};

#endif // vtkPointFeatures_h
