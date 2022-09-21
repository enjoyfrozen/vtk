/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkExtractSelectedIds.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkExtractSelectedIds
 * @brief   extract a list of cells from a dataset
 *
 * vtkExtractSelectedIds extracts a set of cells and points from within a
 * vtkDataSet. The set of ids to extract are listed within a vtkSelection.
 * This filter adds a scalar array called vtkOriginalCellIds that says what
 * input cell produced each output cell. This is an example of a Pedigree ID
 * which helps to trace back results. Depending on whether the selection has
 * GLOBALIDS, VALUES or INDICES, the selection will use the contents of the
 * array named in the GLOBALIDS DataSetAttribute, and arbitrary array, or the
 * position (tuple id or number) within the cell or point array.
 *
 * @sa
 * vtkSelection vtkExtractSelection
 *
 * @deprecated vtkExtractSelectedIds is deprecated in VTK 9.2 and will be removed.
 * Use `vtkExtractSelection` instead of `vtkExtractSelectedIds`.
 *
 * Example using vtkExtractSelectedIds:
 *
 * vtkNew<vtkExtractSelectedIds> selFilter;
 * selFilter->SetInputConnection(0, sphereSource->GetOutputPort());
 * selFilter->SetInputConnection(1, selectionSource->GetOutputPort());
 *
 * Example using vtkExtractSelection:
 *
 * vtkNew<vtkExtractSelection> selFilter;
 * selFilter->SetInputConnection(0, sphereSource->GetOutputPort());
 * selFilter->SetInputConnection(1, selectionSource->GetOutputPort());
 */

#ifndef vtkExtractSelectedIds_h
#define vtkExtractSelectedIds_h

#include "vtkDeprecation.h" // For VTK_DEPRECATED_IN_9_2_0
#include "vtkExtractSelectionBase.h"
#include "vtkFiltersExtractionModule.h" // For export macro
#include "vtkType.h"                    // For OutputDataSetType

class vtkSelection;
class vtkSelectionNode;

class VTK_DEPRECATED_IN_9_2_0("Use vtkExtractSelection instead of vtkExtractSelectedIds.")
  VTKFILTERSEXTRACTION_EXPORT vtkExtractSelectedIds : public vtkExtractSelectionBase
{
public:
  static vtkExtractSelectedIds* New();
  vtkTypeMacro(vtkExtractSelectedIds, vtkExtractSelectionBase);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Get/Set the output type produced by this filter. When filtering
   * points, setting VTK_POLY_DATA can keep both input and output
   * data as polydata. This allows filtering without creating the cells
   * introduced by creating an unstructured grid and converting back
   * with a vtkGeometryFilter.
   *
   * Valid values are VTK_DATA_OBJECT, VTK_POLY_DATA, and
   * VTK_UNSTRUCTURED_GRID defined in vtkType.h
   *
   * Defaults to VTK_DATA_OBJECT.
   */
  vtkSetMacro(OutputDataSetType, int);
  vtkGetMacro(OutputDataSetType, int);
  //@}

protected:
  vtkExtractSelectedIds();
  ~vtkExtractSelectedIds() override;

  int FillOutputPortInformation(int port, vtkInformation* info) override;
  // Overridden to indicate that the input must be a vtkDataSet.
  int FillInputPortInformation(int port, vtkInformation* info) override;

  // Usual data generation method
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  int ExtractCells(vtkSelectionNode* sel, vtkDataSet* input, vtkDataSet* output);
  int ExtractPoints(vtkSelectionNode* sel, vtkDataSet* input, vtkDataSet* output);

private:
  vtkExtractSelectedIds(const vtkExtractSelectedIds&) = delete;
  void operator=(const vtkExtractSelectedIds&) = delete;

  int OutputDataSetType = VTK_DATA_OBJECT;
};

#endif

// VTK-HeaderTest-Exclude: vtkExtractSelectedIds.h
