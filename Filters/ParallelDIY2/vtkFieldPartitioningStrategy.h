// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @class vtkFieldPartitioningStrategy
 * @brief A partitioning strategy based on a custom `CellData` array to generate the partitions.
 *
 * This strategy uses a custom CellData array to generate the partitions.This strategy will classify
 * each cell depending on its corresponding value in the said array, and place it in the
 * corresponding partition. This array can be selected using the property `FieldArrayName`. It will
 * generate a number of partition equivalent to the number of different values inside the array.
 * Also currently it only supports array with single component tuples.
 */

#ifndef vtkFieldPartitioningStrategy_h
#define vtkFieldPartitioningStrategy_h

#include "vtkPartitioningStrategy.h"
#include "vtkRedistributeDataSetFilter.h"

VTK_ABI_NAMESPACE_BEGIN
class VTKFILTERSPARALLELDIY2_EXPORT vtkFieldPartitioningStrategy final
  : public vtkPartitioningStrategy
{
public:
  static vtkFieldPartitioningStrategy* New();
  vtkTypeMacro(vtkFieldPartitioningStrategy, vtkPartitioningStrategy);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  /**
   * Implementation of parent API
   */
  std::vector<PartitionInformation> ComputePartition(vtkPartitionedDataSetCollection*) override;

  ///@{
  /**
   * Specify the `CellData` array name to use to compute the partitions.
   */
  vtkSetMacro(FieldArrayName, std::string);
  vtkGetMacro(FieldArrayName, std::string);
  /// @}
protected:
  vtkFieldPartitioningStrategy() = default;
  ~vtkFieldPartitioningStrategy() override = default;

private:
  vtkFieldPartitioningStrategy(const vtkFieldPartitioningStrategy&) = delete;
  void operator=(const vtkFieldPartitioningStrategy&) = delete;

  std::string FieldArrayName = "__PartitioningFieldArray__";
  vtkSmartPointer<vtkDataArray> FieldArray;

  void GetFieldKeys(std::map<vtkIdType, int>& fieldSet, vtkDataSet* dS);
};
VTK_ABI_NAMESPACE_END

#endif // vtkFieldPartitioningStrategy_h
