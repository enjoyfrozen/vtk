/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkExtractGhostCells.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkExtractGhostCells
 * @brief Extracts ghost cells from the input and untag them (they become visible).
 *
 * This filter takes a `vtkDataSet` or a `vtkDataObjectTree` as input, removes any non-ghost cell,
 * and removes the ghost cell and ghost point arrays in the output so they become visible.
 *
 * The ghost array in the output is `_vtkGhostType`. It can be retrieved through the static method
 * `OutputGhostArrayName()`.
 */

#ifndef vtkExtractGhostCells_h
#define vtkExtractGhostCells_h

#include "vtkFiltersGeneralModule.h" // for export macros
#include "vtkPassInputTypeAlgorithm.h"

class VTKFILTERSGENERAL_EXPORT vtkExtractGhostCells : public vtkPassInputTypeAlgorithm
{
public:
  static vtkExtractGhostCells* New();
  vtkTypeMacro(vtkExtractGhostCells, vtkPassInputTypeAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static const char* OutputGhostArrayName() { return "_vtkGhostType"; };

protected:
  vtkExtractGhostCells() = default;
  ~vtkExtractGhostCells() override = default;

  int FillInputPortInformation(int port, vtkInformation* info) override;

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestDataObject(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

private:
  vtkExtractGhostCells(const vtkExtractGhostCells&) = delete;
  void operator=(const vtkExtractGhostCells&) = delete;
};

#endif
