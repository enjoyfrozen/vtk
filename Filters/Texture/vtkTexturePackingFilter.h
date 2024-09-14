// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#ifndef vtk_aeva_ext_vtkTexturePackingFilter_h
#define vtk_aeva_ext_vtkTexturePackingFilter_h

#include "vtkFiltersTextureModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

VTK_ABI_NAMESPACE_BEGIN

class VTKFILTERSTEXTURE_EXPORT vtkTexturePackingFilter : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkTexturePackingFilter, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkTexturePackingFilter(const vtkTexturePackingFilter&) = delete;
  vtkTexturePackingFilter& operator=(const vtkTexturePackingFilter&) = delete;

  /**
   * Construct object with
   */
  static vtkTexturePackingFilter* New();

  /// Used to index output datasets
  enum InputPorts
  {
    ATLAS = 0,   //!< The input texture atlas.
    BOUNDARY = 1 //!< Shared input chart boundaries.
  };

  /// Used to index output datasets
  enum OutputPorts
  {
    UNIATLAS = 0, //!< The output texture atlas.
    HORIZON = 1   //!< Global horizon after packing.
  };

  //@{
  /**
   * Specify the number of texels to pad charts in all directions in the parameterization
   * space so that texel colors from one chart do not bleed into neighboring charts.
   */
  vtkSetMacro(BoundaryTexel, int);
  vtkGetMacro(BoundaryTexel, int);
  //@}

  //@{
  /**
   * Specify the texel size while discretizing PolyData into texels.
   */
  vtkSetMacro(TexelSize, double);
  vtkGetMacro(TexelSize, double);
  //@}

  //@{
  /**
   * Specify the width of the packed texture map in number of texels.
   */
  vtkSetMacro(TextureMapWidth, int);
  vtkGetMacro(TextureMapWidth, int);
  //@}

  //@{
  /**
   * Specify step size for searching optimal packing position per chart.
   */
  vtkSetMacro(StepSize, int);
  vtkGetMacro(StepSize, int);
  //@}

protected:
  vtkTexturePackingFilter();
  ~vtkTexturePackingFilter() override = default;

  int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;
  int FillInputPortInformation(int port, vtkInformation* info) override;

private:
  int BoundaryTexel = 1;
  int StepSize = 1;
  double TexelSize = 1;
  int TextureMapWidth = 580;
  // int TextureMapHeight = 100;
};

VTK_ABI_NAMESPACE_END
#endif
