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
 * points.
 *
 * Note that any vtkPointSet type can be provided as input, and the output
 * will contain new points each of which is adjusted in a new position.
 *
 * @warning
 * This class has been threaded with vtkSMPTools. Using TBB or other
 * non-sequential type (set in the CMake variable
 * VTK_SMP_IMPLEMENTATION_TYPE) may improve performance significantly.
 *
 * @sa
 * vtkTensorSizingWidget vtkTensorGlyphFilter
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

  // Pipeline support
  int RequestDataObject(vtkInformation* request, vtkInformationVector** inputVector,
                        vtkInformationVector* outputVector) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int FillInputPortInformation(int port, vtkInformation* info) override;

protected:
  vtkGlyphPackingFilter();
  ~vtkGlyphPackingFilter() override;

private:
  vtkGlyphPackingFilter(const vtkGlyphPackingFilter&) = delete;
  void operator=(const vtkGlyphPackingFilter&) = delete;
};

#endif
