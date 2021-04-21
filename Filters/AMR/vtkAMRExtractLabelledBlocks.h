/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkAMRExtractLabelledBlocks.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkAMRExtractLabelledBlocks
 * @brief Generates a multiblock of unstructured grids from an AMR using an input array
 * mapping input cells to an output block id.
 *
 * This filter takes a `vtkAMRUniformGrid` input and generates a multiblock of unstructured grid
 * composed of cells from the input. Cells are attributes to the output block mapped by the input
 * array `PartId`.
 *
 * In addition to this array, the user has to tell if cells should be skipped by providing a
 * `LevelMask` array.
 *
 * This filter is multi-threaded.
 */

#ifndef vtkAMRExtractLabelledBlocks_h
#define vtkAMRExtractLabelledBlocks_h

#include "vtkFiltersAMRModule.h" // for export macros
#include "vtkMultiBlockDataSetAlgorithm.h"

class VTKFILTERSAMR_EXPORT vtkAMRExtractLabelledBlocks : public vtkMultiBlockDataSetAlgorithm
{
public:
  static vtkAMRExtractLabelledBlocks* New();
  vtkTypeMacro(vtkAMRExtractLabelledBlocks, vtkMultiBlockDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set / Get the name of the part id array. This array is maps input cells to output block ids.
   */
  vtkSetStringMacro(PartIdArrayName);
  vtkGetStringMacro(PartIdArrayName);
  //@}

  //@{
  /**
   * Set / Get the name of the level mask array. Cells for which this array maps to zero
   * are not copied into the output.
   */
  vtkSetStringMacro(LevelMaskArrayName);
  vtkGetStringMacro(LevelMaskArrayName);
  //@}

protected:
  vtkAMRExtractLabelledBlocks();
  ~vtkAMRExtractLabelledBlocks() override;

  int FillInputPortInformation(int port, vtkInformation* info) override;

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  char* PartIdArrayName = nullptr;
  char* LevelMaskArrayName = nullptr;

private:
  vtkAMRExtractLabelledBlocks(const vtkAMRExtractLabelledBlocks&) = delete;
  void operator=(const vtkAMRExtractLabelledBlocks&) = delete;
};

#endif
