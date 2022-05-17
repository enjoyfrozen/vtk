/*=========================================================================

  Program:   ParaView
  Module:    vtkSelectArraysExtractBlocks.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSelectArraysExtractBlocks - generate scalars along a specified direction
// .SECTION Description
// vtkSelectArraysExtractBlocks is a filter (...) TODO

#ifndef vtkSelectArraysExtractBlocks_h
#define vtkSelectArraysExtractBlocks_h

#include "vtkFiltersExtractionModule.h" // For export macro

#include <vtkCompositeDataSetAlgorithm.h>

#include <memory>

class vtkDataArraySelection;

class VTKFILTERSEXTRACTION_EXPORT vtkSelectArraysExtractBlocks : public vtkCompositeDataSetAlgorithm
{
public:
  static vtkSelectArraysExtractBlocks* New();
  vtkTypeMacro(vtkSelectArraysExtractBlocks, vtkCompositeDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Enable/disable the internal vtkPassSelectedArrays filter.
   * When disabled, this filter passes all input arrays
   * irrespective of the array selection. Default is `false`.
   */
  void SetPassArraysEnabled(bool enabled);
  bool GetPassArraysEnabled();
  ///@}

  ///@{
  /**
   * Enable/disable the internal vtkExtractBlockUsingDataAssembly filter.
   * When disabled, this filter passes all input blocks
   * irrespective of the block selection. Default is `false`.
   */
  void SetExtractBlocksEnabled(bool enabled);
  bool GetExtractBlocksEnabled();
  ///@}

  ///@{
  /**
   * Convenience methods that call `GetArraySelection` with corresponding
   * association type.
   */
  vtkDataArraySelection* GetPointDataArraySelection();
  vtkDataArraySelection* GetCellDataArraySelection();
  vtkDataArraySelection* GetFieldDataArraySelection();
  vtkDataArraySelection* GetVertexDataArraySelection();
  vtkDataArraySelection* GetEdgeDataArraySelection();
  vtkDataArraySelection* GetRowDataArraySelection();
  ///@}

  ///@{
  /**
   * API to set selectors. Multiple selectors can be added using `AddSelector`.
   * The order in which selectors are specified is not preserved and has no
   * impact on the result.
   *
   * `AddSelector` returns true if the selector was added, false if the selector
   * was already specified and hence not added.
   *
   * @sa vtkDataAssembly::SelectNodes
   */
  bool AddSelector(const char* selector);
  void ClearSelectors();
  ///@}

  ///@{
  /**
   * Get/Set the active assembly to use. The chosen assembly is used
   * in combination with the selectors specified to determine which blocks
   * are to be extracted.
   *
   * By default, this is set to
   * vtkDataAssemblyUtilities::HierarchyName().
   */
  void SetAssemblyName(const char* assemblyName);
  char* GetAssemblyName();
  ///@}

protected:
  vtkSelectArraysExtractBlocks();
  ~vtkSelectArraysExtractBlocks();

  int FillInputPortInformation(int port, vtkInformation* info) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestDataObject(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

private:
  vtkSelectArraysExtractBlocks(const vtkSelectArraysExtractBlocks&) = delete;
  void operator=(const vtkSelectArraysExtractBlocks&) = delete;

  class vtkInternals;
  std::unique_ptr<vtkInternals> Internals;
};

#endif
