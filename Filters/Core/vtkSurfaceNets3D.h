/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSurfaceNets3D.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkSurfaceNets3D
 * @brief   generate smoothed isocontours from segmented 3D image data (i.e., "label maps")
 *
 * vtkSurfaceNets3D creates boundary/isocontour surfaces from a label map
 * (e.g., a segmented image) using a threaded, 3D version of the multiple
 * regions/labels Surface Nets algorithm. The input is a 3D image (i.e.,
 * volume) where each voxel is labeled (integer labels are preferred to real
 * values), and the output data is a polygonal mesh separating labeled
 * regions / objects.  (Note that on output each region [corresponding to a
 * different segmented object] will share points/edges on a common boundary,
 * i.e., two adjacent objects will share the boundary that separates them.)
 * This threaded implementation uses concepts from Flying Edges to achieve
 * high performance and scalability.
 *
 * The filter implements a contouring operation over a non-continuous scalar
 * field. In comparison, classic contouring methods (like Flying Edges or
 * Marching Cubes) presume a continuous scalar field. In comparision, this
 * method processes non-continuous label maps, which corresponds to discrete
 * regions in an input 3D image (i.e., volume). With a non-continuous scalar
 * function, the usual data interpolation across a continuous function (e.g.,
 * interpolation along cell edges) is not possible. Instead, when the edge
 * endpoint voxels are labeled in differing regions, the edge is split and
 * transected by a polygon that connects the center points of the voxels on
 * either side of the edge. Later, using a energy minimization smoothing
 * process, the resulting polygonal mesh is adjusted to produce a smoother
 * result. (Constraints on smoothing displacements may be specified to
 * prevent excessive shrinkage and/or object distortion.)
 *
 * The smoothing process is controlled by setting a convergence measure, the
 * number of smoothing iterations, the step size, and the allowed
 * (constraint) distance that points may move.  These can be adjusted to
 * provide the desired result. This class provides a method to access an
 * internal instance of vtkConstrainedSmoothingFilter, through which these
 * smoothing parameters may be specified, and which actually performs the
 * smoothing operation. (Note: it is possible to skip the smoothing process
 * altogether by disabling smoothing (e.g., invoking SmoothingOff()) or
 * setting the number of smoothing iterations to zero. This can be useful
 * when using a different smoothing filter like
 * vtkWindowedSincPolyDataFilter; or if an unsmoothed, aliased output is
 * desired. The reason the smoothing is built in to this filter is to remain
 * faithful to the published literature describing the surface nets
 * algorithm.)
 *
 * The SurfaceNets algorithm was first proposed by Sarah Frisken.  Two
 * important papers include the description of surface nets for binary
 * objects (i.e., extracting just one segmented object from a volume) and
 * multi-label (multiple object extraction).
 *
 * S. Frisken (Gibson), “Constrained Elastic SurfaceNets: Generating Smooth
 * Surfaces from Binary Segmented Data”, Proc. MICCAI, 1998, pp. 888-898.
 *
 * S. Frisken, “SurfaceNets for Multi-Label Segmentations with Preservation
 * of Sharp Boundaries”, J. Computer Graphics Techniques, 2022.
 *
 * Note that one nice feature of this filter is that algorithm execution
 * occurs only once no matter the number of object labels / contour
 * values. In many contouring-like algorithms, each separate contour value
 * requires an additional algorithm execution with a new contour value. So in
 * this filter large numbers of contour values do not significantly affect
 * overall speed. The user can specify which objects (i.e., labels) are to be
 * output to the filter. (Unspecified labels are treated as background and
 * not output.)
 *
 * Besides output geometry, the filter outputs a two-component, cell data
 * array indicating the labels/regions on either side of the polygons
 * composing the output vtkPolyData. (This can be used for advanced
 * operations like extracting shared/contacting boundaries between two
 * objects. The name of this cell data array is "BoundaryLabels".)
 *
 * Note also that the content of the filter's output can be controlled by
 * specifying the OutputStyle.  This produces different output which
 * may better serve a particular workflow. For example, it is possible
 * to produce texture coordinates
 *
 * Implementation note: For performance reasons, this filter is internally
 * implemented quite differently than described in the literature.  The main
 * difference is that concepts from the Flying Edges parallel isocontouring
 * algorithm are used. Namely, parallel, edge-by-edge processing is used to
 * define cell cases, generate smoothing stencils, and produce points and
 * output polygons. The smoothing process is also threaded using a
 * double-buffering approach. For more information on Flying Edges see the
 * paper:
 *
 * "Flying Edges: A High-Performance Scalable Isocontouring Algorithm" by
 * Schroeder, Maynard, Geveci. Proc. of LDAV 2015. Chicago, IL.
 *
 * or visit VTK's FE implementation vtkFlyingEdges3D.
 *
 * @warning
 * This filter is specialized to 3D images.
 *
 * @warning
 * The output of this filter is a polygonal mesh. By default when smoothing
 * is disabled, the output is quad polygons. However, once smoothing is
 * enabled, the quads are typically decomposed into triangles since the quads
 * are typically no longer planar. A filter option is available to force the
 * type of output polygonal mesh (quads, or triangles).
 *
 * @warning
 * Subtle differences in the output may result when the number of objects /
 * labels extracted changes. This is because the smoothing operation operates
 * on all of the boundaries simultaneously. If the boundaries change due to a
 * difference in the number of extracted regions / labels, then the smoothing
 * operation can produce different results.
 *
 * @warning
 * The filters vtkDiscreteMarchingCubes, vtkDiscreteFlyingEdges3D,
 * vtkDiscreteFlyingEdges3D, and vtkDiscreteFlyingEdgesClipper3D also perform
 * contouring of label maps. However these filters produce output that may
 * not share common boundary cells, or may produce "gaps" between segmented
 * regions. For example, vtkDiscreteMarchingCubes will share points between
 * adjacent regions, but not triangle cells (which will be coincident). Also,
 * no center point is inserted into voxels, meaning that intermittent gaps
 * may form between regions. This Surface Nets implementation fully shares
 * the boundary between adjacent objects.
 *
 * @warning
 * This class has been threaded with vtkSMPTools. Using TBB or other
 * non-sequential type (set in the CMake variable
 * VTK_SMP_IMPLEMENTATION_TYPE) may improve performance significantly.  Note
 * that for "small" volumes, serial execution may be faster due to the cost
 * of managing threads. To force serial execution set VTK_IMPLEMENTATION_TYPE
 * to "Sequential".
 *
 * @sa
 * vtkSurfaceNets2D vtkDiscreteFlyingEdges3D vtkDiscreteFlyingEdgesClipper3D
 * vtkConstrainedSmoothingFilter vtkFlyingEdges3D vtkWindowedSincPolyDataFilter
 */

#ifndef vtkSurfaceNets3D_h
#define vtkSurfaceNets3D_h

#include "vtkContourValues.h"     // Needed for direct access to ContourValues
#include "vtkFiltersCoreModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"
#include "vtkConstrainedSmoothingFilter.h" // Perform mesh smoothing
#include "vtkPolyData.h" // To support data caching

VTK_ABI_NAMESPACE_BEGIN

class vtkImageData;

class VTKFILTERSCORE_EXPORT vtkSurfaceNets3D : public vtkPolyDataAlgorithm
{
public:
  ///@{
  /**
   * Standard methods for instantiation, printing, and obtaining type
   * information.
   */
  static vtkSurfaceNets3D* New();
  vtkTypeMacro(vtkSurfaceNets3D, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  /**
   * The modified time is also a function of the label values and
   * the smoothing filter.
   */
  vtkMTimeType GetMTime() override;

  ///@{
  /**
   * Set a particular label value at label number i. The index i ranges
   * between (0 <= i < NumberOfLabels). (Note: while labels values are
   * expressed as doubles, the underlying scalar data may be a different
   * type. During execution the label values are cast to the type of the
   * scalar data.)  Note the use of "Value" and "Label" when specifying
   * regions to extract. The use of "Value" is consistent with other VTK
   * continuous-scalar field isocontouring algorithms; however the term
   * "Label" is more consistent with label maps.  Warning: make sure that the
   * value of the background label (see definition below) is different than
   * any of the specified labels, otherwise the generated cell scalars may be
   * incorrect.
   */
  void SetValue(int i, double value) { this->Labels->SetValue(i, value); }
  void SetLabel(int i, double value) { this->Labels->SetValue(i, value); }
  ///@}

  ///@{
  /**
   * Get the ith label value.
   */
  double GetValue(int i) { return this->Labels->GetValue(i); }
  double GetLabel(int i) { return this->Labels->GetValue(i); }
  ///@}

  ///@{
  /**
   * Get a pointer to an array of labels. There will be
   * GetNumberOfLabels() values in the list.
   */
  double* GetValues() { return this->Labels->GetValues(); }
  double* GetLabels() { return this->Labels->GetValues(); }
  ///@}

  ///@{
  /**
   * Fill a supplied list with label values. There will be
   * GetNumberOfLabels() values in the list. Make sure you allocate enough
   * memory to hold the list.
   */
  void GetValues(double* contourValues) { this->Labels->GetValues(contourValues); }
  void GetLabels(double* contourValues) { this->Labels->GetValues(contourValues); }
  ///@}

  ///@{
  /**
   * Set the number of labels to place into the list. You only really need to
   * use this method to reduce list size. The method SetValue() will
   * automatically increase list size as needed. Note that for consistency
   * with other isocountoring-related algorithms, some methods use
   * "Labels" and "Contours" interchangeably.
   */
  void SetNumberOfLabels(int number) { this->Labels->SetNumberOfContours(number); }
  void SetNumberOfContours(int number) { this->Labels->SetNumberOfContours(number); }
  ///@}

  ///@{
  /**
   * Get the number of labels in the list of label values.
   */
  vtkIdType GetNumberOfLabels() { return this->Labels->GetNumberOfContours(); }
  vtkIdType GetNumberOfContours() { return this->Labels->GetNumberOfContours(); }
  ///@}

  ///@{
  /**
   * Generate numLabels equally spaced labels between the specified
   * range. The labels will include the min/max range values.
   */
  void GenerateLabels(int numLabels, double range[2])
  {
    this->Labels->GenerateValues(numLabels, range);
  }
  void GenerateValues(int numContours, double range[2])
  {
    this->Labels->GenerateValues(numContours, range);
  }
  void GenerateLabels(int numLabels, double rangeStart, double rangeEnd)
  {
    this->Labels->GenerateValues(numLabels, rangeStart, rangeEnd);
  }
  void GenerateValues(int numContours, double rangeStart, double rangeEnd)
  {
    this->Labels->GenerateValues(numContours, rangeStart, rangeEnd);
  }
  ///@}

  ///@{
  /**
   * This value specifies the label value to use when referencing the
   * backround region outside of any of the specified regions. (This value is
   * used when producing cell scalars.) By default this value is zero. Be
   * very careful of the value being used here, it should not overlap an
   * extracted label value, and because it is the same type as the input
   * image scalars, make sure the value can be properly represented (e.g., if
   * the input scalars are an unsigned type, then BackgroundLabel should not
   * be negative).
   */
  vtkSetMacro(BackgroundLabel, double);
  vtkGetMacro(BackgroundLabel, double);
  ///@}

  ///@{
  /**
   * Set/get which component of a input multi-component scalar array to
   * contour with; defaults to component 0.
   */
  vtkSetMacro(ArrayComponent, int);
  vtkGetMacro(ArrayComponent, int);
  ///@}

  /**
   * This enum is used to control the type of the output polygonal mesh.
   */
  enum MeshType
  {
    MESH_TYPE_DEFAULT = 0,
    MESH_TYPE_TRIANGLES,
    MESH_TYPE_QUADS
  };

  ///@{
  /**
   * Control the type of output mesh. By default, if smoothing is off, the
   * output mesh is a polygonal mesh consisting of quadrilaterals
   * (quads). However, if smoothing is enabled, then the output mesh type is
   * a polygonal mesh consisting of triangles. It is possible to force the
   * output mesh type to be of a certain type (triangles, or quads)
   * regardless whether smoothing is enabled or not. Note that if an output
   * mesh is forced to be quads, and smoothing is enabled, the resulting
   * quads may not be planar.
   */
  vtkSetClampMacro(OutputMeshType,int,MESH_TYPE_DEFAULT,MESH_TYPE_QUADS);
  vtkGetMacro(OutputMeshType,int);
  void SetOutputMeshTypeToDefault() {this->SetOutputMeshType(MESH_TYPE_DEFAULT);}
  void SetOutputMeshTypeToTriangles() {this->SetOutputMeshType(MESH_TYPE_TRIANGLES);}
  void SetOutputMeshTypeToQuadrilaterals() {this->SetOutputMeshType(MESH_TYPE_QUADS);}
  ///@}

  ///@{
  /**
   * Indicate whether smoothing should be enabled. By default, after the
   * isosurface is extracted, smoothing occurs using the built-in smoother.
   * To disable smoothing, invoke SmoothingOff().
   */
  vtkSetMacro(Smoothing, bool);
  vtkGetMacro(Smoothing, bool);
  vtkBooleanMacro(Smoothing, bool);
  ///@}

  ///@{
  /**
   * Get the instance of vtkConstrainedSmoothingFilter used to smooth the
   * extracted surface net. To control smoothing, access this instance and
   * specify its parameters such as number of smoothing iterations and
   * constraint distance. If you wish to disable smoothing, set
   * SmoothingOff().
   */
  vtkGetSmartPointerMacro(Smoother, vtkConstrainedSmoothingFilter);
  ///@}

  ///@{
  /**
   * Enable caching of intermediate data. A common workflow using this filter
   * requires extracting object boundaries (i.e., the isocontour), and then
   * repeatedly rerunning the smoothing process with different parameters. To
   * improve performance by avoiding repeated extraction of the boundary, the
   * filter can cache intermediate data prior to the smoothing process. In
   * this way, the boundary is only extracted once, and as long as only the
   * internal constrained smoothing filter is modified, then boundary
   * extraction will not be reexecuted. By default this is enabled.
   */
  vtkSetMacro(DataCaching,bool);
  vtkGetMacro(DataCaching,bool);
  vtkBooleanMacro(DataCaching,bool);
  ///@}

protected:
  vtkSurfaceNets3D();
  ~vtkSurfaceNets3D() override = default;

  // Support visualization pipeline operations.
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int FillInputPortInformation(int port, vtkInformation* info) override;

  // Support the contouring operation.
  vtkSmartPointer<vtkContourValues> Labels;
  bool ComputeScalars;
  double BackgroundLabel;
  int ArrayComponent;
  int OutputMeshType;

  // Support smoothing.
  bool Smoothing;
  vtkSmartPointer<vtkConstrainedSmoothingFilter> Smoother;

  // Support data caching of the extracted surface nets. This is used to
  // avoid repeated surface extraction when only smoothing filter
  // parameters are modified.
  bool DataCaching;
  vtkSmartPointer<vtkPolyData> GeometryCache;
  vtkSmartPointer<vtkCellArray> StencilsCache;
  vtkTimeStamp SmoothingTime;
  bool IsCacheEmpty();
  void CacheData(vtkPolyData* pd, vtkCellArray* ca);

private:
  vtkSurfaceNets3D(const vtkSurfaceNets3D&) = delete;
  void operator=(const vtkSurfaceNets3D&) = delete;
};

VTK_ABI_NAMESPACE_END
#endif
