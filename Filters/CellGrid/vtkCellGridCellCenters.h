// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @class   vtkCellGridCellCenters
 * @brief   Adds vertex cells at the center of each input cell.
 *
 * All attributes should be interpolated to each output vertex.
 *
 * Because the query is simple, it is simply a child class of
 * the algorithm.
 */

#ifndef vtkCellGridCellCenters_h
#define vtkCellGridCellCenters_h

#include "vtkCellGridAlgorithm.h"
#include "vtkFiltersCellGridModule.h" // For export macro
#include "vtkNew.h"                   // for ivar

VTK_ABI_NAMESPACE_BEGIN

class VTKFILTERSCELLGRID_EXPORT vtkCellGridCellCenters : public vtkCellGridAlgorithm
{
public:
  /// A query corresponding to this algorithm.
  class Query : public vtkCellGridQuery
  {
  public:
    static Query* New();
    vtkTypeMacro(vtkCellGridCellCenters::Query, vtkCellGridQuery);
    void PrintSelf(ostream& os, vtkIndent indent) override;

    bool Initialize() override;
    bool Finalize() override;

    vtkCellGrid* GetOutput() const { return this->Output; }

  protected:
    friend class vtkCellGridCellCenters;
    Query() = default;
    ~Query() override = default;

    vtkCellGrid* Output{ nullptr };

  private:
    Query(const Query&) = delete;
    void operator=(const Query&) = delete;
  };

  static vtkCellGridCellCenters* New();
  vtkTypeMacro(vtkCellGridCellCenters, vtkCellGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkCellGridCellCenters() = default;
  ~vtkCellGridCellCenters() override = default;

  int RequestData(
    vtkInformation* request, vtkInformationVector** inInfo, vtkInformationVector* ouInfo) override;

  vtkNew<Query> Request;

private:
  vtkCellGridCellCenters(const vtkCellGridCellCenters&) = delete;
  void operator=(const vtkCellGridCellCenters&) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkCellGridCellCenters_h
