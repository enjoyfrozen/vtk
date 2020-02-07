/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGlyphPackingFilter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkGlyphPackingFilter
 * @brief   pack glyphs for the purpose of tensor visualization
 *
 * vtkGlyphPackingFilter is used to help visualize tensors, for example those
 * produced from diffusion tensor imaging. More generally, it can be used to
 * visualize a mesh sizing field, which is a 3D tensor field characterizing
 * the size and layout of a 3D mesh (typically used for mesh generation). The
 * input to this filter are points with associated (point data) tensors. On
 * output, the filter adjusts the positions of the points to provide a
 * pleasing packing of tensor glyphs (such as those produced by the
 * vtkTensorGlyph filter). While the points can be placed in 3D, more often
 * they are placed on a surface to minimize mutual occlusion and visual
 * clutter.
 *
 * vtkGlyphPackingFilter is often teamed with the vtkMeshSizingWidget
 * to enable interactive visualization and modification of a mesh
 * sizing field.
 *
 * The inspiration for this filter is the paper by Gordon Kindlmann and
 * Carl-Fredrik Westin "Diffusion Tensor Visualization With Glyph Packing."
 *
 * @sa
 * vtkMeshSizingWidget vtkTensorGlyph
 */

#ifndef vtkGlyphPackingFilter_h
#define vtkGlyphPackingFilter_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class VTKFILTERSGENERAL_EXPORT vtkGlyphPackingFilter : public vtkPolyDataAlgorithm
{
public:
  //@{
  /**
   * Standard methods for instantiation, type information, and printing.
   */
  static vtkGlyphPackingFilter* New();
  vtkTypeMacro(vtkGlyphPackingFilter, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  //@}

protected:
  vtkGlyphPackingFilter();
  ~vtkGlyphPackingFilter();

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

private:
  vtkGlyphPackingFilter(const vtkGlyphPackingFilter&) = delete;
  void operator=(const vtkGlyphPackingFilter&) = delete;
};

#endif
