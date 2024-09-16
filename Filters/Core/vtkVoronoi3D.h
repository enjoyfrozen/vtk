/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVoronoi3D.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkVoronoi3D
 * @brief   create 3D Voronoi convex tessellation of input points
 *
 * vtkVoronoi3D is a filter that constructs a 3D Voronoi tessellation of a
 * list of input points. The points are presumed to lie within
 * 3D-space. These points may be represented by any dataset of type
 * vtkPointSet and subclasses. The output of the filter is an unstructured
 * grid dataset containing vtkPolyhedron. Each output cell is a convex
 * polyhedron.
 *
 * The 3D Voronoi tessellation is a tessellation of space, where each Voronoi
 * n-dimensional tile (in 3D, a polyhedron) represents the region nearest to
 * one of the input points. Voronoi tessellations are important in
 * computational geometry (and many other fields), and are the dual of
 * Delaunay triangulations.
 *
 * This filter is a reference implementation written with simplicity in
 * mind. Additional methods are available for debugging / instructional
 * purposes. This includes producing a single tile under various stages of
 * creation, as well as the Voronoi flower, related to the error metric for
 * point insertion / half-space clipping.
 *
 * Publications are in preparation to describe the algorithm. A brief summary
 * is as follows. In parallel, each (generating) input point is associated
 * with an initial Voronoi tile, which is simply the bounding box of the
 * point set. A locator is then used to identify nearby points: each neighbor
 * in turn generates a clipping plane positioned halfway between the
 * generating point and the neighboring point, and orthogonal to the line
 * connecting them. Clips are readily performed by evaluationg the vertices
 * of the convex Voronoi tile as being on either side (inside,outside) of the
 * clip line. If two intersections of the Voronoi tile are found, the portion
 * of the tile "outside" the clip line is discarded, resulting in a new
 * convex, Voronoi tile. As each clip occurs, the Voronoi "Flower" error
 * metric (the union of error spheres) is compared to the extent of the
 * region containing the neighboring clip points. The clip region (along with
 * the points contained in it) is grown by careful expansion (e.g., outward
 * spiraling iterator over all candidate clip points). When the Voronoi
 * Flower is contained within the clip region, the algorithm terminates and
 * the Voronoi tile is output. Once complete, it is possible to construct the
 * Delaunay triangulation from the Voronoi tessellation. Note that
 * topological and geometric information is used to generate a valid
 * triangulation (e.g., merging points and validating topology).
 *
 * @warning
 * Coincident input points are discarded. The Voronoi tessellation requires
 * unique input points.
 *
 * @warning
 * This is a novel approach which implements an embarrassingly parallel
 * algorithm. At the core of the algorithm a locator is used to determine
 * points close to a specified position. A vtkStaticPointLocator is used
 * because it is both threaded (when constructed) and supports thread-safe
 * queries. While other locators could be used in principal, they must
 * support thread-safe operations.
 *
 * @warning
 * This class has been threaded with vtkSMPTools. Using TBB or other
 * non-sequential type (set in the CMake variable
 * VTK_SMP_IMPLEMENTATION_TYPE) may improve performance significantly.
 *
 * @sa
 * vtkVoronoi3D vtkDelaunay2D vtkDelaunay3D
 */

#ifndef vtkVoronoi3D_h
#define vtkVoronoi3D_h

#include "vtkFiltersCoreModule.h" // For export macro
#include "vtkPointSetAlgorithm.h"
#include "vtkSmartPointer.h" // For self-destructing data members
#include "vtkSpheres.h" // For Voronoi Flower
#include "vtkStaticPointLocator.h" // For point locator


class VTKFILTERSCORE_EXPORT vtkVoronoi3D : public vtkPointSetAlgorithm
{
public:
  ///@{
  /**
   * Standard methods for instantiation, type information, and printing.
   */
  static vtkVoronoi3D* New();
  vtkTypeMacro(vtkVoronoi3D, vtkPointSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  ///@{
  /**
   * Specify a padding for the bounding box of the points. A >0 padding is
   * necessary in order to create valid Voronoi tiles on the boundary of the
   * tessellation. The padding is specified as a fraction of the diagonal
   * length of the bounding box of the points.
   */
  vtkSetClampMacro(Padding, double, 0.001, 0.25);
  vtkGetMacro(Padding, double);
  ///@}

  enum GenerateCellScalarsStrategy
  {
    NONE = 0,
    POINT_IDS = 1,
    THREAD_IDS = 2
  };

  ///@{
  /**
   * Indicate whether to create a cell scalar array as part of the
   * output. Options include generating no scalars; using input point ids, or
   * defining scalars by execution thread ids. By default no scalars are
   * generated.
   */
  vtkSetMacro(GenerateScalars, int);
  vtkGetMacro(GenerateScalars, int);
  void SetGenerateScalarsToNone() { this->SetGenerateScalars(NONE); }
  void SetGenerateScalarsToPointIds() { this->SetGenerateScalars(POINT_IDS); }
  void SetGenerateScalarsToThreadIds() { this->SetGenerateScalars(THREAD_IDS); }
  ///@}

  ///@{
  /**
   * These methods are for debugging or instructional purposes. When the
   * point of interest is specified (i.e., to a non-negative number) then the
   * algorithm will only process this single point (whose id is the
   * PointOfInterest). The maximum number of clips (the
   * MaximumNumberOfTileClips) can be specified. If
   * MaximumNumberOfTileClips=0, then the initial tile (single point within
   * the bounding box) is produced; if =1 then the split with the closest
   * point is produced; and so on. By default the PointOfInterest is set to
   * (-1), and the number of clips is unlimited (i.e.,
   * MaximumNumberOfTileClips=VTK_ID_MAX and therefore automatically limited
   * by the algorithm).
   */
  vtkSetClampMacro(PointOfInterest, vtkIdType, -1, VTK_ID_MAX);
  vtkGetMacro(PointOfInterest, vtkIdType);
  vtkSetClampMacro(MaximumNumberOfTileClips, vtkIdType, 1, VTK_ID_MAX);
  vtkGetMacro(MaximumNumberOfTileClips, vtkIdType);
  ///@}

  ///@{
  /**
   * Retrieve the internal locator to manually configure it, for example
   * specifying the number of points per bucket. This method is generally
   * used for debugging or testing purposes.
   */
  vtkStaticPointLocator* GetLocator() { return this->Locator; }
  ///@}

  ///@{
  /**
   * These methods are for debugging or instructional purposes. If
   * GenerateVoronoiFlower is on, and the PointOfIntersect is specified, then
   * second and third (optional) outputs are populated which contains a
   * representation of the Voronoi flower error metric (second output) and
   * the single Voronoi tile (corresponding to PointOfInterest) with point
   * scalar values indicating the radii of the Voronoi Flower petals (i.e.,
   * spheres contributing to the error metric).
   */
  vtkSetMacro(GenerateVoronoiFlower, vtkTypeBool);
  vtkGetMacro(GenerateVoronoiFlower, vtkTypeBool);
  vtkBooleanMacro(GenerateVoronoiFlower, vtkTypeBool);
  ///@}

  ///@{
  /**
   * Return the Voronoi flower (a collection of spheres) for the point of
   * interest in the form of a vtkSpheres implicit function. This is valid
   * only if GenerateVoronoiFlower and the PointOfInterest are set, and after
   * the filter executes. Typically this is used for debugging or educational
   * purposes.
   */
  vtkGetObjectMacro(Spheres, vtkSpheres);
  ///@}

  /**
   *  Return the number of threads actually used during execution. This is
   *  valid only after algorithm execution.
   */
  int GetNumberOfThreadsUsed() { return this->NumberOfThreadsUsed; }

  /**
   * Get the MTime of this object also considering the locator.
   */
  vtkMTimeType GetMTime() override;

protected:
  vtkVoronoi3D();
  ~vtkVoronoi3D() override = default;

  int GenerateScalars;
  double Padding;
  double Tolerance;
  vtkSmartPointer<vtkStaticPointLocator> Locator;
  vtkIdType PointOfInterest;
  vtkIdType MaximumNumberOfTileClips;
  vtkTypeBool GenerateVoronoiFlower;
  int NumberOfThreadsUsed;
  vtkSmartPointer<vtkSpheres> Spheres;

  // Satisfy pipeline-related API
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int FillOutputPortInformation(int port, vtkInformation* info) override;

private:
  vtkVoronoi3D(const vtkVoronoi3D&) = delete;
  void operator=(const vtkVoronoi3D&) = delete;
};

#endif
