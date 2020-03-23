/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPointSmoothingFilter.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See LICENSE file for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkPointSmoothingFilter
 * @brief   adjust point positions to form a pleasing, packed arrangement
 *
 *
 * vtkPointSmoothingFilter modifies the coordinates of the input points of a
 * vtkPointSet by adjusting their position to create a smooth distribution
 * (and thereby form a pleasing packing of the points). Smoothing is
 * performed by considering the effects of neighboring points on one
 * another. Smoothing in its simplest form is simply a variant of Laplacian
 * smoothing where each point moves towards the average position of its
 * neigboring points. Next uniform smoothing uses a cubic cutoff function to
 * produce higher forces between points that are closer together, but the
 * forces are independent of associated point data attribute
 * values. Smoothing can be further controlled either by a scalar field, by a
 * tensor field, or a frame field (the user can specify the nature of the
 * smoothing operation). If controlled by a scalar field, then each input
 * point is assumed to be surrounded by a isotropic sphere scaled by the
 * scalar field; if controlled by a tensor field, then each input point is
 * assumed to be surrounded by an anisotropic, oriented ellipsoid aligned to
 * the the tensor eigenvectors and scaled by the determinate of the tensor. A
 * frame field also assumes a surrounding, ellipsoidal shape except that the
 * inversion of the ellipsoid tensor is already performed. Typical usage of
 * this filter is to perform a smoothing (also referred to as packing)
 * operation (i.e., first execute this filter) and then combine it with a
 * glyph filter (e.g., vtkTensorGlyph or vtkGlyph3D) to visualize the packed
 * points.
 *
 * Smoothing depends on a local neighborhood of nearby points. In general,
 * the larger the neighborhood size, the greater the reduction in high
 * frequency information. (The memory and/or computational requirements of
 * the algorithm may also significantly increase.)
 *
 * Any vtkPointSet type can be provided as input, and the output will contain
 * the same number of new points each of which is adjusted to a new position.
 *
 * Note that the algorithm requires the use of a spatial point locator. The
 * point locator is used to build a local neighborhood of the points
 * surrounding each point. It is also used to perform interpolation as the
 * point positions are adjusted.
 *
 * The algorithm incrementally adjusts the point positions through an
 * iterative process. Basically points are moved due to the influence of
 * neighboring points. Iterations continue until the specified number of
 * iterations is reached, or convergence occurs. Convergence occurs when the
 * maximum displacement of any point is less than the convergence value. As
 * points move, both the local connectivity and data attributes associated
 * with each point must be updated. Rather than performing these expensive
 * operations after every iteration, a number of sub-iterations Si can be
 * specified. If Si > 1, then the neighborhood and attribute value updates
 * occur only every Si'th iteration. Using sub-iterations can improve
 * performance significantly.
 *
 * @warning
 * This class has been loosely inspired by the paper by Kindlmann and Westin
 * "Diffusion Tensor Visualization with Glyph Packing". However, several
 * computational shortcuts, and generalizations have been used for performance
 * and utility reasons.
 *
 * @warning
 * This class has been threaded with vtkSMPTools. Using TBB or other
 * non-sequential type (set in the CMake variable
 * VTK_SMP_IMPLEMENTATION_TYPE) may improve performance significantly.
 *
 * @sa
 * vtkTensorWidget vtkTensorGlyph vtkSmoothPolyDataFilter
 */

#ifndef vtkPointSmoothingFilter_h
#define vtkPointSmoothingFilter_h

#include "vtkFiltersPointsModule.h" // For export macro
#include "vtkPointSetAlgorithm.h"

class vtkAbstractPointLocator;
class vtkDataArray;


class VTKFILTERSPOINTS_EXPORT vtkPointSmoothingFilter : public vtkPointSetAlgorithm
{
public:
  //@{
  /**
   * Standard methods for instantiation, obtaining type information, and
   * printing information.
   */
  static vtkPointSmoothingFilter* New();
  vtkTypeMacro(vtkPointSmoothingFilter, vtkPointSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  //@}

  //@{
  /**
   * Specify the neighborhood size. This controls the number of surrounding
   * points that can affect a point to be smoothed.
   */
  vtkSetClampMacro(NeighborhoodSize, int, 4, 26);
  vtkGetMacro(NeighborhoodSize, int);
  //@}

  /**
   * Specify how smoothing is to be controlled.
   */
  enum
  {
    DEFAULT_SMOOTHING,
    LAPLACIAN_SMOOTHING,
    UNIFORM_SMOOTHING,
    SCALAR_SMOOTHING,
    TENSOR_SMOOTHING,
    FRAME_FIELD_SMOOTHING
  };

  //@{
  /**
   * Control how smoothing is to be performed. By default, if a point frame
   * field is available then frame field smoothing will be performed; then if
   * point tensors are available then anisotropic tensor smoothing will be
   * used; the next choice is to use isotropic scalar smoothing; and finally
   * if no frame field, tensors, or scalars are available, uniform smoothing
   * will be used. If both scalars, tensors, and /or a frame field are
   * present, the user can specifiy which to use; or to use uniform or
   * geometric smoothing.
   */
  vtkSetClampMacro(SmoothingMode, int, DEFAULT_SMOOTHING, FRAME_FIELD_SMOOTHING);
  vtkGetMacro(SmoothingMode, int);
  void SetSmoothingModeToDefault() { this->SetSmoothingMode(DEFAULT_SMOOTHING); }
  void SetSmoothingModeToLaplacian() { this->SetSmoothingMode(LAPLACIAN_SMOOTHING); }
  void SetSmoothingModeToUniform() { this->SetSmoothingMode(UNIFORM_SMOOTHING); }
  void SetSmoothingModeToScalars() { this->SetSmoothingMode(SCALAR_SMOOTHING); }
  void SetSmoothingModeToTensors() { this->SetSmoothingMode(TENSOR_SMOOTHING); }
  void SetSmoothingModeToFrameField() { this->SetSmoothingMode(FRAME_FIELD_SMOOTHING); }
  //@}

  //@{
  /**
   * Specify the name of the frame field to use for smoothing. This
   * information is only necessary if a frame field smoothing is enabled.
   */
  virtual void SetFrameFieldArray(vtkDataArray*);
  vtkGetObjectMacro(FrameFieldArray, vtkDataArray);
  //@}

  //@{
  /**
   * Specify the number of smoothing iterations.
   */
  vtkSetClampMacro(NumberOfIterations, int, 0, VTK_INT_MAX);
  vtkGetMacro(NumberOfIterations, int);
  //@}

  //@{
  /**
   * Specify the number of smoothing subiterations. This specifies the
   * frequency of connectivity and data attribute updates.
   */
  vtkSetClampMacro(NumberOfSubIterations, int, 1, VTK_INT_MAX);
  vtkGetMacro(NumberOfSubIterations, int);
  //@}

  //@{
  /**
   * Specify a convergence criterion for the iteration
   * process. Smaller numbers result in more smoothing iterations.
   */
  vtkSetClampMacro(Convergence, double, 0.0, 1.0);
  vtkGetMacro(Convergence, double);
  //@}

  //@{
  /**
   * Specify the relaxation factor for smoothing iterations. The relexation
   * factor controls the speed (across multiple iterations) which points
   * move. As in all iterative methods, the stability of the process is
   * sensitive to this parameter. In general, small relaxation factors and
   * large numbers of iterations are more stable than larger relaxation
   * factors and smaller numbers of iterations.
   */
  vtkSetClampMacro(RelaxationFactor, double, 0.0, 1.0);
  vtkGetMacro(RelaxationFactor, double);
  //@}

  //@{
  /**
   * Specify a point locator. By default a vtkStaticPointLocator is
   * used. The locator performs efficient searches to locate points
   * around a sample point.
   */
  void SetLocator(vtkAbstractPointLocator* locator);
  vtkGetObjectMacro(Locator, vtkAbstractPointLocator);
  //@}

protected:
  vtkPointSmoothingFilter();
  ~vtkPointSmoothingFilter() override;

  // Control the smoothing
  int NeighborhoodSize;
  int SmoothingMode;
  double Convergence;
  int NumberOfIterations;
  int NumberOfSubIterations;
  double RelaxationFactor;
  vtkDataArray* FrameFieldArray;

  // Support the algorithm
  vtkAbstractPointLocator* Locator;

  // Pipeline support
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int FillInputPortInformation(int port, vtkInformation* info) override;

private:
  vtkPointSmoothingFilter(const vtkPointSmoothingFilter&) = delete;
  void operator=(const vtkPointSmoothingFilter&) = delete;
};

#endif
