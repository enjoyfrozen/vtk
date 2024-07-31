// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @class   vtkDGTranscribeCellGridCells
 * @brief   Respond to a query on one particular type of cell.
 *
 * This is pure virtual base class that all responder types must inherit.
 */

#ifndef vtkDGTranscribeCellGridCells_h
#define vtkDGTranscribeCellGridCells_h

#include "vtkFiltersCellGridModule.h" // for export macro

#include "vtkCellGridToUnstructuredGrid.h" // for inheritance
#include "vtkCellGridResponder.h"

VTK_ABI_NAMESPACE_BEGIN
class vtkCellMetadata;
class vtkDGCell;

class VTKFILTERSCELLGRID_EXPORT vtkDGTranscribeCellGridCells
  : public vtkCellGridResponder<vtkCellGridToUnstructuredGrid::Query>
{
public:
  using TranscribeQuery = vtkCellGridToUnstructuredGrid::Query;
  static vtkDGTranscribeCellGridCells* New();
  vtkTypeMacro(vtkDGTranscribeCellGridCells, vtkCellGridResponder<vtkCellGridToUnstructuredGrid::Query>);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  bool Query(TranscribeQuery* query, vtkCellMetadata* cellType,
    vtkCellGridResponders* caches) override;

protected:
  vtkDGTranscribeCellGridCells() = default;
  ~vtkDGTranscribeCellGridCells() override = default;

  void GenerateConnectivity(TranscribeQuery* request, vtkDGCell* cellType);
  void GeneratePointData(TranscribeQuery* request, vtkDGCell* cellType);

private:
  vtkDGTranscribeCellGridCells(const vtkDGTranscribeCellGridCells&) = delete;
  void operator=(const vtkDGTranscribeCellGridCells&) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkDGTranscribeCellGridCells_h
// VTK-HeaderTest-Exclude: vtkDGTranscribeCellGridCells.h
