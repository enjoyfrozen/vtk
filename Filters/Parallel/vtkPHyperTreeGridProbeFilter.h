// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @class   vtkPHyperTreeGridProbeFilter
 * @brief   probe a vtkHyperTreeGrid in parallel
 *
 * Heavily modeled after the vtkPProbeFilter and vtkProbeFilter, this class
 * is meant to be used to probe vtkHyperTreeGrid objects in parallel.
 *
 * This filter works correctly only if the whole geometry dataset
 * (that specify the point locations used to probe input) is present on all
 * nodes.
 *
 * Probing will be executed on each node with the whole geometry dataset
 * on different parts (extents) of the source HTG.
 * Then, a reduction of each result will happen on rank 0. So result
 * should be retrieved from rank 0, other ranks will contain an empty output.
 *
 * @warning UseImplicitArrays option will lead to unexpected results
 * with the vtkPHyperTreeGridProbeFilter, until vtkHyperTreeGrids
 * are able to generate global IDs. UseImplicitArrays is forced
 * to false with vtkPHyperTreeGridProbeFilter instances until then.
 *
 * @sa vtkHyperTreeGridProbeFilter
 *
 * Possible optimizations:
 * - Enrich the parallelism logic allowing distributed input/outputs support
 *   with distributed HTG sources
 */

#ifndef vtkPHyperTreeGridProbeFilter_h
#define vtkPHyperTreeGridProbeFilter_h

#include "vtkFiltersParallelModule.h" //For export Macro
#include "vtkHyperTreeGridProbeFilter.h"
#include "vtkSmartPointer.h" //For Locator member

VTK_ABI_NAMESPACE_BEGIN
class vtkMultiProcessController;
class vtkIdList;
class vtkDataSet;
class vtkHyperTreeGrid;
class vtkHyperTreeGridLocator;

class VTKFILTERSPARALLEL_EXPORT vtkPHyperTreeGridProbeFilter : public vtkHyperTreeGridProbeFilter
{
public:
  vtkTypeMacro(vtkPHyperTreeGridProbeFilter, vtkHyperTreeGridProbeFilter);

  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkPHyperTreeGridProbeFilter* New();

  ///@{
  /**
   * Set and get the controller.
   */
  virtual void SetController(vtkMultiProcessController*);
  vtkGetObjectMacro(Controller, vtkMultiProcessController);
  ///@}

  /**
   * Overriden to ensure UseImplicitArrays is set to false.
   * Should be removed once vtkPHyperTreeGridProbeFilter can handle this mode.
   */
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  ///@{
  /**
   * Get/Set wether or not the filter should use implicit arrays.
   * If set to true, probed values will not be copied to the output
   * but retrieved from the source through indexation (thanks to indexed arrays).
   * This can lower the memory consumption, especially if the probed source contains
   * a lot of data arrays. Note that it will also increase the computation time.
   *
   * @warning This option will lead to unexpected results in distributed
   * until vtkHyperTreeGrids are able to generate global IDs
   * (and vtkHyperTreeGridLocator able to use them). UseImplicitArrays is forced
   * to false with vtkPHyperTreeGridProbeFilter instances until then.
   *
   * @sa vtkHyperTreeGridProbeFilter
   */
  void SetUseImplicitArrays(bool useImplicitArrays) override;
  ///@}

protected:
  ///@{
  /**
   * Construction methods
   */
  vtkPHyperTreeGridProbeFilter();
  ~vtkPHyperTreeGridProbeFilter() override;
  ///@}

  ///@{
  /**
   * Overridden here because it is important that the input be updated on all processes
   */
  int RequestUpdateExtent(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  ///@}

  ///@{
  /**
   * Helper method for reducing the distributed data to the master process
   */
  bool Reduce(vtkHyperTreeGrid* source, vtkDataSet* output, vtkIdList* localPointIds) override;
  ///@}

  enum
  {
    HYPERTREEGRID_PROBE_COMMUNICATION_TAG = 4242
  };

  vtkMultiProcessController* Controller = nullptr;

private:
  vtkPHyperTreeGridProbeFilter(const vtkPHyperTreeGridProbeFilter&) = delete;
  void operator=(const vtkPHyperTreeGridProbeFilter&) = delete;

}; // vtkPHyperTreeGridProbeFilter

VTK_ABI_NAMESPACE_END
#endif // vtkPHyperTreeGridProbeFilter_h
