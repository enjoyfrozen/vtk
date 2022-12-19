/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPartitioningStrategy.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef vtkPartitioningStrategy_h
#define vtkPartitioningStrategy_h

#include "vtkFiltersParallelDIY2Module.h" // for export macro
#include "vtkObject.h"
#include "vtkSmartPointer.h" // for member variables

/**
 * @class vtkPartitioningStrategy
 * @brief A strategy interface for partitioning meshes
 *
 * This pure virtual class is meant to act as an interface for homogeneizing the use of different
 * graph partitioning algorithms in the vtkRedistributeDataSetFilter.
 *
 * @sa
 * vtkRedistributeDataSetFilter
 */
namespace diy
{
namespace mpi
{
class communicator;
}
}
VTK_ABI_NAMESPACE_BEGIN
class vtkIdTypeArray;
class vtkMultiProcessController;
class vtkPartitionedDataSetCollection;
class VTKFILTERSPARALLELDIY2_EXPORT vtkPartitioningStrategy : public vtkObject
{
public:
  vtkTypeMacro(vtkPartitioningStrategy, vtkObject);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  /**
   * An enum defining which principal entity is being partitioned
   */
  enum PartitionedEntity
  {
    POINTS = 0,
    CELLS = 1
  };

  /**
   * Encapsulation structure for describing the result of a partitioning calculation
   */
  struct PartitionInformation
  {
  public:
    /**
     * Principal entity on which the partitioning is defined
     */
    PartitionedEntity TargetEntity = CELLS;
    /**
     * An array defining on which rank each currently local principal entity should be shipped to
     */
    vtkNew<vtkIdTypeArray> TargetPartitions;
    /**
     * An 2 component array defining boundary entity indexes and their neighbor partition index
     */
    vtkNew<vtkIdTypeArray> BoundaryNeighborPartitions;
    /**
     * The total number of partitions
     */
    vtkIdType NumberOfPartitions = 0;
  };

  /**
   * Main method for subclasses to implement in order to define their partitioning method (should be
   * called on all ranks due to distributed communication)
   *
   * Returns a vector of PartitionInformation structures (one for each block partition data set in
   * the collection)
   */
  virtual std::vector<PartitionInformation> ComputePartition(vtkPartitionedDataSetCollection*) = 0;

  ///@{
  /**
   * Get/Set the controller to use. By default
   * vtkMultiProcessController::GlobalController will be used.
   */
  void SetController(vtkMultiProcessController*);
  vtkGetObjectMacro(Controller, vtkMultiProcessController);
  ///@}

  ///@{
  /**
   * Get/Set number of partitions (if 0 use number of MPI ranks)
   */
  vtkGetMacro(NumberOfPartitions, vtkIdType);
  vtkSetMacro(NumberOfPartitions, vtkIdType);
  ///@}

protected:
  vtkPartitioningStrategy();
  ~vtkPartitioningStrategy() override;

  vtkMultiProcessController* Controller = nullptr;

  vtkIdType NumberOfPartitions = 0;

private:
  vtkPartitioningStrategy(const vtkPartitioningStrategy&) = delete;
  void operator=(const vtkPartitioningStrategy&) = delete;
};
VTK_ABI_NAMESPACE_END

#endif // vtkPartitioningStrategy_h
