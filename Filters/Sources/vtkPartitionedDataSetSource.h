/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPartitionedDataSetSource.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkPartitionedDataSetSource
 * @brief a source that produces a vtkPartitionedDataSet.
 *
 * vtkPartitionedDataSet generates a vtkPartitionedDataSet. It uses a
 * given vtkParametricFunctionSource internally to generate a surfaces.
 * The resulting partitioned dataset is split among ranks in an even fashion
 * by default.
 */

#ifndef vtkPartitionedDataSetSource_h
#define vtkPartitionedDataSetSource_h

#include "vtkFiltersSourcesModule.h" // For export macro
#include "vtkParametricFunction.h"
#include "vtkPartitionedDataSetAlgorithm.h"
#include "vtkSmartPointer.h"

#include <map>

class vtkPartitionedDataSet;

class VTKFILTERSSOURCES_EXPORT vtkPartitionedDataSetSource : public vtkPartitionedDataSetAlgorithm
{
public:
  static vtkPartitionedDataSetSource* New();
  vtkTypeMacro(vtkPartitionedDataSetSource, vtkAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Enable/Disable ranks.
   * By default every rank is enabled.
   */
  void EnableRank(int rank);
  void DisableRank(int rank);
  bool IsEnabledRank(int rank);
  ///@}

  ///@{
  /**
   * Set/Get the number of partitions of the resulting PartitionedDataSet.
   * If not specified, the number of partitions will be the number of enabled
   * ranks.
   */
  vtkSetMacro(NumberOfPartitions, int);
  vtkGetMacro(NumberOfPartitions, int);
  ///@}

  ///@{
  /**
   * Set the Parametric function to be used for this source
   */
  void SetParametricFunction(vtkSmartPointer<vtkParametricFunction>);
  ///@}

protected:
  vtkPartitionedDataSetSource();
  ~vtkPartitionedDataSetSource() override;

  int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestData(vtkInformation* info, vtkInformationVector** input, vtkInformationVector* output) override;

private:
  vtkPartitionedDataSetSource(const vtkPartitionedDataSetSource&) = delete;
  void operator=(const vtkPartitionedDataSetSource&) = delete;

  std::map<int, int> Allocations;
  int NumberOfPartitions = 0;
  vtkSmartPointer<vtkParametricFunction> ParametricFunction;
};

#endif
