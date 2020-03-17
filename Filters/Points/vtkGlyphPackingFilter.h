/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGlyphPackingFilter.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See LICENSE file for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkGlyphPackingFilter
 * @brief   adjust point positions to form a pleasing, packed arrangement
 *
 *
 * vtkGlyphPackingFilter modifies the coordinates of the input points of a
 * vtkPointSet by adjusting their position to form a pleasing packing of the
 * points. The packing is controlled either by a scalar field, or by a tensor
 * field (one of either scalars or tensors is required; if both are available
 * then the user can specify which to use). If controlled by a scalar field,
 * then each input point is assumed to be surrounded by a isotropic sphere
 * scaled by the scalar field; if controlled by a tensor field, then each
 * input point is assumed to be surrounded by an anisotropic, oriented
 * ellipsoid aligned to the the tensor eigenvectors and scaled by the
 * determinate of the tensor. Typical usage of this filter is to perform the
 * packing operation (i.e., first execute this filter) and then use a glyph
 * filter (e.g., vtkTensorGlyph or vtkGlyph3D) to visualize the packed
 * points.
 *
 * Any vtkPointSet type can be provided as input, and the output will contain
 * the same number of new points each of which is adjusted to a new position.
 *
 * Note that the algorithm requires the use of a spatial point locator. The
 * point locator is used to build a local neighborhood of the points
 * surrounding each point. It is also used to perform interpolation as the
 * point positions are adjusted.
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
 * vtkTensorWidget vtkTensorGlyph
 */

#ifndef vtkGlyphPackingFilter_h
#define vtkGlyphPackingFilter_h

#include "vtkFiltersPointsModule.h" // For export macro
#include "vtkPointSetAlgorithm.h"


class VTKFILTERSPOINTS_EXPORT vtkGlyphPackingFilter : public vtkPointSetAlgorithm
{
public:
  //@{
  /**
   * Standard methods for instantiation, obtaining type information, and
   * printing information.
   */
  static vtkGlyphPackingFilter* New();
  vtkTypeMacro(vtkGlyphPackingFilter, vtkPointSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  //@}

  /**
   * Specify how packing is to be controlled.
   */
  enum
  {
    DEFAULT_PACKING,
    SCALAR_PACKING,
    TENSOR_PACKING
  };

  //@{
  /**
   * Control how packing is to be performed. By default, if point tensors are
   * available then anisotropic tensor packing will be used; otherwise
   * isotropic scalar packing will be used. If both scalars and tensors are
   * present, the user can specifiy which to use. (One of either point scalars
   * or point vectors must be available in the input.)
   */
  vtkSetClampMacro(PackingMode, int, DEFAULT_PACKING, TENSOR_PACKING);
  vtkGetMacro(PackingMode, int);
  void SetPackingModeToDefault() { this->SetPackingMode(DEFAULT_PACKING); }
  void SetPackingModeToScalars() { this->SetPackingMode(SCALAR_PACKING); }
  void SetPackingModeToTensors() { this->SetPackingMode(TENSOR_PACKING); }
  //@}

protected:
  vtkGlyphPackingFilter();
  ~vtkGlyphPackingFilter() override;

  // Control the packing
  int PackingMode;

  // Pipeline support
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int FillInputPortInformation(int port, vtkInformation* info) override;

private:
  vtkGlyphPackingFilter(const vtkGlyphPackingFilter&) = delete;
  void operator=(const vtkGlyphPackingFilter&) = delete;
};

#endif
