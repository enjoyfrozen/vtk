// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @class   vtkVoronoi2D
 * @brief   create 2D Voronoi convex tiling of input points
 *
 * vtkVoronoi2D is a filter that constructs a 2D Voronoi tessellation of a
 * set of input points. The points are assumed to lie in a plane. These
 * points may be represented by any dataset of type vtkPointSet and
 * subclasses. The output of the filter is a polygonal dataset. Each output
 * cell is a convex polygon (i.e., a Voronoi tile), although options exist
 * for producing other outputs including a 2D Delaunay triangulation.
 *
 * The 2D Voronoi tessellation is a tiling of space, where each Voronoi tile
 * represents the region nearest to one of the input points (the tile
 * generators). Voronoi tessellations are fundamental constructs in
 * computational geometry (and many other fields), and are the dual of
 * Delaunay triangulations.
 *
 * The input to this filter is a list of points specified in 3D, even though
 * the tessellation is 2D. Thus the tessellation is constructed in the x-y
 * plane, and the z coordinate is ignored (although carried through to the
 * output). If you desire to tessellate in a different plane, you can use
 * the vtkTransformFilter to transform the points into and out of the x-y
 * plane, or you can specify a transform to vtkVoronoi2D directly.  In the
 * latter case, the input points are transformed, the transformed points are
 * tessellated, and the output will use the tessellated topology for the
 * original (non-transformed) points.  This avoids transforming the data back
 * as would be required when using the vtkTransformFilter method.  Specifying
 * a transform directly also allows any transform to be used: rigid,
 * non-rigid, non-invertible, etc.
 *
 * This filter is a reference implementation written with simplicity in
 * mind. The filter also provides methods for debugging / instructional
 * purposes. This includes producing a single Voronoi tile under various
 * stages of creation, as well as the Voronoi flower, related to the
 * neighborhood metric for point insertion / half-space clipping.
 *
 * Publications are in preparation to describe the algorithm. A brief summary
 * is as follows. In parallel, each (generating) input point is associated
 * with an initial Voronoi tile, which is simply the bounding box of the
 * point set. A spatial locator is then used to identify nearby points: each
 * neighbor in turn generates a clipping line positioned halfway between the
 * generating point and the neighboring point, and orthogonal to the line
 * connecting them. Clips are readily performed by evaluating the vertices of
 * the convex Voronoi tile as being on either side (inside,outside) of the
 * clip line. If two intersections of the Voronoi tile are found, the portion
 * of the tile "outside" the clip line is discarded, resulting in a new
 * convex, Voronoi tile. As each clip occurs, the Voronoi "Flower"
 * neighborhood metric (the union of Delaunay circumcircles) is compared to
 * the extent of the region containing the neighboring clip points. The clip
 * region (along with the points contained in it) is grown by careful
 * expansion (e.g., outward, annular requests of point neighbors). When the
 * Voronoi Flower is contained within the clip region, the algorithm
 * terminates and the Voronoi tile is output. Once complete, it is possible
 * to construct the Delaunay triangulation from the Voronoi
 * tessellation. Note that topological and geometric information is used to
 * generate a valid triangulation (e.g., merging points and validating
 * topology).
 *
 * There are up to four outputs to this filter depending on how the filter is
 * configured. The first filter output #0 is the Voronoi tessellation. The
 * second output #1 is the Delaunay triangulation; the third (output #2) and
 * fourth (output #3) outputs are available when the PointOfInterest p is set
 * to a value 0 <= p < (number of input points) and GenerateVoronoiFlower is
 * enabled. The third output is a random sampling of points within the
 * flower; the fourth is the Voronoi tile of interest along with scalar
 * values corresponding to the Voronoi petals radii at each Voronoi tile
 * vertex point.
 *
 * This filter can be used to tessellate different regions using convex
 * polygons (i.e., Voronoi tiles), or create holes in Voronoi tessellations,
 * using a supplemental input single-component, scalar data array (the region
 * ids array). The size of the region ids array must match the number of
 * input points. In this array, a region id value <0 means the tile
 * associated with its associated point is considered "outside" and so is not
 * produced on output. Otherwise, any non-negative region is indicates which
 * region a particular tile belongs to.
 *
 * Note that an important concept of this algorithm is a graphical
 * representation referred to as the "wheel and spokes" data structure.  When
 * the Voronoi tessellation if generated, connections to the neighbors of
 * each Voronoi tile are known as spokes. The radially ordered (in
 * counterclockwise direction) collection of all spokes associated with a
 * tile is known as a wheel. Then to generate the Delaunay triangulation, a
 * parallel traversal of the wheel and spokes graph is used to extract
 * triangles from the graph.
 *
 * @warning
 * Delaunay degenerate cases (when more than three points are co-circular)
 * are of theoretical and practical interest in the implementation of
 * Delaunay and Voronoi algorithms. Degenerate cases introduce numerical
 * challenges into algorithms, and extremely important work such as
 * Shewchuk's geometric predicates (https://www.cs.cmu.edu/~quake/robust.html).
 * In this algorithm, degeneracies are treated in two ways. First, Voronoi
 * spokes, which correspond to very short "edges" on a Voronoi tile can be
 * geometrically pruned (i.e., edge deleted). Alternatively, the wheel and
 * spokes data structure can be processed to identify and repair
 * inconsistencies in the resulting mesh, ensuring that a vlid result is
 * produced.
 *
 * @warning
 * Coincident input points will likely produce an invalid tessellation. This
 * is because the Voronoi tessellation requires unique input points. Use a
 * cleaning filter (like vtkStaticCleanPolyData/vtkCleanPolyData) to remove
 * duplicates if necessary.
 *
 * @warning
 * This algorithm is a novel approach which implements embarrassingly
 * parallel algorithms for both the Voronoi tessellation and the generation
 * of the dual Delaunay triangulation. At the core of the algorithm is the
 * use of a spatial locator to determine points close to a specified position
 * (the Voronoi tile generating point). Points are requested in increasing,
 * disjoint annular rings.  Currently, a vtkStaticPointLocator2D is used
 * because it is both threaded (its construction) and supports thread-safe
 * queries. While other locators could be used in principal (and may be added
 * in the future), they must support thread-safe operations and annular point
 * requests.
 *
 * @warning
 * This class has been threaded with vtkSMPTools. Using TBB or other
 * non-sequential type (set in the CMake variable
 * VTK_SMP_IMPLEMENTATION_TYPE) may improve performance significantly.
 *
 * @sa
 * vtkDelaunay2D vtkTransformFilter vtkStaticPointLocator2D
 */

#ifndef vtkVoronoi2D_h
#define vtkVoronoi2D_h

#include "vtkFiltersCoreModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"
#include "vtkAbstractTransform.h" // For transforming input points
#include "vtkSmartPointer.h" // For self-destructing data members
#include "vtkSpheres.h" // For Voronoi Flower
#include "vtkStaticPointLocator2D.h" // For point locator

VTK_ABI_NAMESPACE_BEGIN
class vtkStaticPointLocator2D;
class vtkAbstractTransform;
class vtkPointSet;
class vtkSpheres;

class VTKFILTERSCORE_EXPORT vtkVoronoi2D : public vtkPolyDataAlgorithm
{
public:
  ///@{
  /**
   * Standard methods for instantiation, type information, and printing.
   */
  static vtkVoronoi2D* New();
  vtkTypeMacro(vtkVoronoi2D, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  /**
   * Used to control filter output.
   */
  enum OutputTypeOptions
  {
    VORONOI=0,
    DELAUNAY=1,
    VORONOI_AND_DELAUNAY=2,
  };

  ///@{
  /**
   * Control whether to produce an output Voronoi tessllation and/or an
   * output Delaunay triangulation. (If enabled, the Voronoi tessellation
   * is produced in filter output #0. If enabled, the Delaunay triangulation is
   * produced in output #1.) Note that this OutputType data member just
   * controls what is sent to the filter output--in all cases this filter
   * computes an internal representation of the Voronoi tessellation. However
   * if disabled, then the cost of memory and execution is reduced by not
   * actually instantiating the Voronoi polygonal mesh. This can be useful
   * for example if only the Delaunay triangulation is desired. By default,
   * the filter only produces the Voronoi tessellation. If specified, the
   * Delaunay triangulation is computed by extracting the dual of the
   * Voronoi tessellation. (Note that the extraction process may include
   * topological checks to ensure that the Voronoi tessellation is valid. See
   * the Validate data member for more information.)
   */
  vtkSetMacro(OutputType, int);
  vtkGetMacro(OutputType, int);
  void SetOutputTypeToVoronoi() { this->SetOutputType(VORONOI); }
  void SetOutputTypeToDelaunay() { this->SetOutputType(DELAUNAY); }
  void SetOutputTypeToVoronoiAndDelaunay() { this->SetOutputType(VORONOI_AND_DELAUNAY); }
  //@}

  ///@{
  /**
   * Specify a padding for the bounding box of the points. A >0 padding is
   * necessary in order to create valid Voronoi tiles on the boundary of the
   * tessellation. The padding is specified as a fraction of the diagonal
   * length of the bounding box of the points. Note that changing the padding
   * can affect the resulting tiles and Delaunay triangulation. This is because
   * any Voronoi tessellation will have semi-infinite tiles on the boundary
   * of the tessellation, practically meaning that as the padding is increased,
   * the dual Delaunay triangulation will become more convex (i.e., and as the
   * padding is decreased, the Delaunay triangulation will become less convex).
   */
  vtkSetClampMacro(Padding, double, 0.0001, 0.25);
  vtkGetMacro(Padding, double);
  ///@}

  ///@{
  /**
   * Indicate whether to pass input point data through to the filter outputs.
   * If enabled, input point data is passed through to the Voronoi output #0
   * as cell data; and to the Delaunay output #1 as point data. By default,
   * passing the input point data is enabled.
   */
  vtkSetMacro(PassPointData,vtkTypeBool);
  vtkGetMacro(PassPointData,vtkTypeBool);
  vtkBooleanMacro(PassPointData,vtkTypeBool);
  ///@}

  /**
   * The following enums are used to control the production of auxiliary
   * scalar data associated with each Voronoi tile and/or Delaunay
   * point. These auxiliary scalars can be used to produce additional
   * information useful for debugging or display. This auxiliary scalar data
   * is passed through as both cell data (to the Voronoi output #0) and point
   * data (to the Delaunay output #1). (Recall for each input point, a Voronoi
   * tile is generated, which is also a Delaunay vertex; hence the number of
   * output tiles equals the number of output Delaunay vertices [assuming that
   * RegionIds are not used for culling]).  By default (NONE) no auxiliary
   * scalar data is produced.  Otherwise scalars corresponding to point ids
   * (POINT_IDS), region ids (REGION_IDS), the number of sides for each
   * Voronoi tile (NUMBER_SIDES), or the thread id producing the tile
   * (THREAD_IDS) is also possible. Note that REGION_IDS are only possible
   * when a single-component, point data array is provided on input. Also
   * note that the THREAD_IDS scalars will likey vary between threaded
   * executions.
   */
  enum GenerateScalarsStrategy
  {
    NONE=0,
    POINT_IDS=1,
    REGION_IDS=2,
    NUMBER_SIDES=3,
    THREAD_IDS=4
  };

  ///@{
  /**
   * Indicate whether to create an auxiliary scalar array as part of the
   * Voronoi and/or Delaunay output (if Voronoi output is requested). Point
   * ids, region ids, the number of sides of each Voronoi polygon, or
   * execution thread ids may be output. By default no auxiliary scalars are
   * generated. Note that different algorithm executions likely generate
   * different thread ids as execution order is unpredictable.
   */
  vtkSetClampMacro(GenerateScalars, int, NONE, THREAD_IDS);
  vtkGetMacro(GenerateScalars, int);
  void SetGenerateScalarsToNone() { this->SetGenerateScalars(NONE); }
  void SetGenerateScalarsToPointIds() { this->SetGenerateScalars(POINT_IDS); }
  void SetGenerateScalarsToRegionIds() { this->SetGenerateScalars(REGION_IDS); }
  void SetGenerateScalarsToNumberOfSides()
    { this->SetGenerateScalars(NUMBER_SIDES); }
  void SetGenerateScalarsToThreadIds()
    { this->SetGenerateScalars(THREAD_IDS); }
  ///@}

  ///@{
  /**
   * The following methods - FindTile() and GetTileData() - can be used to
   * locate/query the tile containing a point x (i.e., given that a Voronoi
   * tile Vi is a region of closest proximity to the generating point x).
   * FindTile() returns the tile id/point id of a query location x. If
   * desired, GetTileData() will return the associated convex polygonal tile
   * in the user-supplied vtkPolyData. (GetTileData() requires that the output
   * type is VORONOI or VORONOI_AND_DELAUNAY.) Note that if the query point x is
   * outside of the bounds of the input point set, an id value <0 is
   * returned. Also, these methods are only valid after the filter executes.
   * (The third component of thw query point x[2] should be in the transformed
   * space of the input points.)
   */
  vtkIdType FindTile(double x[3]);
  void GetTileData(vtkIdType tileId, vtkPolyData* tileData);
  ///@}

  ///@{
  /**
   * Specify whether to prune the connections (i.e., spokes) between Voronoi
   * tiles during processing. The spokes are dual connections represented by
   * the edges of each convex Voronoi tile: removing (or pruning) a spoke
   * means eliminating the connection to a tile neighbor, i.e., deleting a
   * tile edge. Small tile edges may occur in degenerate situations where more
   * than three Voronoi tiles meet (i.e., the Delaunay circumcircles are
   * degenerate with more than three points laying on the
   * crcumcircle). Pruning can be useful to prevent degenerate
   * tessellations. By default, pruning is off (it requires extra work and
   * typically slows algorithm execution).
   */
  vtkSetMacro(PruneSpokes, vtkTypeBool);
  vtkGetMacro(PruneSpokes, vtkTypeBool);
  vtkBooleanMacro(PruneSpokes, vtkTypeBool);
  ///@}

  ///@{
  /**
   * If PruneSpokes is enabled, specify a relative tolerance to determine
   * which spokes to prune. The relative tolerance is defined as the ratio of
   * the edge length of a Voronoi tile to the length of the associated spoke
   * connecting two Voronoi tile generator points. If the relative tolerance
   * is <= the PruneTolerance, then the spoke (and associated edge) are
   * pruned.
   */
  vtkSetClampMacro(PruneTolerance, double, 0.0, 0.5);
  vtkGetMacro(PruneTolerance, double);
  ///@}

  ///@{
  /**
   * Enable the validation and repair of the Voronoi tesselation (which also
   * affects the Delaunay triangulation if requested). Enabling validation
   * increases computation time. By default, validation is on. Validation
   * can be disabled but this may result in topological inconsistencies
   * in the output Delaunay triangulation.
   */
  vtkSetMacro(Validate, vtkTypeBool);
  vtkGetMacro(Validate, vtkTypeBool);
  vtkBooleanMacro(Validate, vtkTypeBool);
  ///@}

  ///@{
  /**
   * Set / get the transform which is applied to points to generate a 2D
   * problem.  This maps a 3D dataset into a 2D dataset where triangulation
   * can be done on the XY plane.  The points are then tessellated and the
   * topology of tessellation are used as the output topology.  The output
   * points are the original (untransformed) points.  The transform can be
   * any subclass of vtkAbstractTransform (thus it does not need to be a
   * linear or invertible transform).
   */
  vtkSetSmartPointerMacro(Transform, vtkAbstractTransform);
  vtkGetSmartPointerMacro(Transform, vtkAbstractTransform);
  ///@}

  enum ProjectionPlaneStrategy
  {
    XY_PLANE = 0,
    SPECIFIED_TRANSFORM_PLANE = 1,
    BEST_FITTING_PLANE = 2
  };

  ///@{
  /**
   * Define the method to project the input 3D points into a 2D plane for
   * tessellation. When the VTK_XY_PLANE is set, the z-coordinate is simply
   * ignored. When VTK_SET_TRANSFORM_PLANE is set, then a transform must be
   * supplied and the points are transformed using it. Finally, if
   * VTK_BEST_FITTING_PLANE is set, then the filter computes a best fitting
   * plane and projects the points onto it.
   */
  vtkSetClampMacro(ProjectionPlaneMode, int, XY_PLANE, BEST_FITTING_PLANE);
  vtkGetMacro(ProjectionPlaneMode, int);
  void SetProjectionPlaneModeToXYPlane() { this->SetProjectionPlaneMode(XY_PLANE); }
  void SetProjectionPlaneModeToSpecifiedTransformPlane()
  {
    this->SetProjectionPlaneMode(SPECIFIED_TRANSFORM_PLANE);
  }
  void SetProjectionPlaneModeToBestFittingPlane()
  {
    this->SetProjectionPlaneMode(BEST_FITTING_PLANE);
  }
  ///@}

  ///@{
  /**
   * These methods are for debugging or instructional purposes. When the
   * point of interest is specified (i.e., set to a non-negative number) then
   * the algorithm will only process this single point (whose id is the
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
  vtkStaticPointLocator2D* GetLocator() { return this->Locator; }
  ///@}

  ///@{
  /**
   * These methods are for debugging or instructional purposes. If
   * GenerateVoronoiFlower is on, and the PointOfIntersect is specified, then
   * third and fourth (optional) outputs are populated which contains a
   * representation of the Voronoi flower neighborhood metric (third output)
   * and the single Voronoi tile (corresponding to PointOfInterest) with
   * point scalar values indicating the radii of the Voronoi Flower petals
   * (i.e., circles contributing to the neighborhood metric).
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

  ///@{
  /**
   * Specify the number of input generating points in a batch, where a batch
   * defines a contiguous subset of the input points operated on during
   * threaded execution. Generally this is only used for debugging or
   * performance studies (since batch size affects the thread workload).
   *
   * Default is 1000.
   */
  vtkSetClampMacro(BatchSize, unsigned int, 1, VTK_INT_MAX);
  vtkGetMacro(BatchSize, unsigned int);
  ///@}

  /**
   *  Return the maximum number of sides across all Voronoi tiles. This is
   *  valid only after algorithm execution.
   */
  int GetMaximumNumberOfSides() {return this->MaximumNumberOfSides;}

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
  vtkVoronoi2D();
  ~vtkVoronoi2D() override = default;

  int OutputType;
  vtkTypeBool Validate;
  double Padding;
  vtkTypeBool PassPointData;
  int GenerateScalars;
  int ProjectionPlaneMode; // selects the plane in 3D where the tessellation will be computed
  vtkSmartPointer<vtkStaticPointLocator2D> Locator;
  vtkSmartPointer<vtkAbstractTransform> Transform;
  bool GenerateDelaunayTriangulation;
  vtkIdType PointOfInterest;
  vtkIdType MaximumNumberOfTileClips;
  vtkTypeBool GenerateVoronoiFlower;
  vtkSmartPointer<vtkSpheres> Spheres;
  vtkTypeBool PruneSpokes;
  double PruneTolerance;
  unsigned int BatchSize;
  int MaximumNumberOfSides;
  int NumberOfThreadsUsed;

  // Satisfy pipeline-related API
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  // Specify that the input is of type vtkPointSet
  int FillInputPortInformation(int, vtkInformation*) override;

private:
  vtkVoronoi2D(const vtkVoronoi2D&) = delete;
  void operator=(const vtkVoronoi2D&) = delete;
};

VTK_ABI_NAMESPACE_END
#endif
