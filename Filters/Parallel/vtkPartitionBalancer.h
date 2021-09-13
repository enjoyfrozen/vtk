/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPartitionBalancer.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkPartitionBalancer
 * @brief Balances input partitioned data sets so each rank have the same number of data sets.
 *
 * This filter can be applied on `vtkPartitionedDataSet` or `vtkPartitionedDataSetCollection`.
 *
 * * Given an input `vtkPartitionedDataSet`, this filter adds
 * `nullptr` instances in the output `vtkPartitionedDataSet` following a pattern specified
 * as parameter. The output partitioned data set should have the same number of partitions across
 * all ranks.
 * * Given an input `vtkPartitionedDataSetCollection`, this filter is applied on each partitioned
 * data set separately, and is producing a `vtkPartitioneDataSetCollection`.
 *
 * If some input partitions are `nullptr`, the output will see this partition squeezed out.
 * The filter will treat the input partitioned data set as if this `nullptr` partition was non
 * existent.
 *
 * The way the output is laid out is driven by the parameter `Mode`;
 * * `Mode::Expand` generates, per partitioned data set, as many partitions as there are partitions
 * in the input across all ranks.
 * Given a valid partition (not `nullptr`) in the output partitioned data set at index
 * n in rank i, all partitioned data set of all ranks but i have a `nullptr` instance as index n.
 * Output partitions are sorted by rank number. i.e., for i < j, partition at rank i are indexed
 * before partitions of rank j. Here is an example. of what would be generated for a
 * given input. PDC holds for Partitioned Dataset Collection, and PD holds for Partitioned Dataset.
 * @code
 * Input:
 * rank 0: PDC [ PD (DS0, DS1,     DS2) ] [PD (nullptr, DS100) ]
 * rank 1: PDC [ PD (DS3, nullptr, DS4) ] [PD ()               ]
 *
 * Output:
 * rank 0: PDC [ PD (DS0,     DS1,     DS2,     nullptr, nullptr) ] [PD (DS100)   ]
 * rank 1: PDC [ PD (nullptr, nullptr, nullptr, DS3,     DS4)     ] [PD (nullptr) ]
 * @endcode
 * * `Mode::Squash` generates, per input partitioned data set, the minimum number of partitions
 * possible, appending `nullptr` in ranks lacking partitions. Using the same example as above:
 * @code
 * Input:
 * rank 0: PDC [ PD (DS0, DS1,     DS2) ] [PD (nullptr, DS100) ]
 * rank 1: PDC [ PD (DS3, nullptr, DS4) ] [PD ()               ]
 *
 * Output:
 * rank 0: PDC [ PD (DS0, DS1, DS2)     ] [PD (DS100)   ]
 * rank 1: PDC [ PD (DS3, DS4, nullptr) ] [PD (nullptr) ]
 * @endcode
 */

#ifndef vtkPartitionBalancer_h
#define vtkPartitionBalancer_h

#include "vtkFiltersParallelModule.h" // for export macros
#include "vtkPassInputTypeAlgorithm.h"

class vtkMultiProcessController;

class VTKFILTERSPARALLEL_EXPORT vtkPartitionBalancer : public vtkPassInputTypeAlgorithm
{
public:
  static vtkPartitionBalancer* New();
  vtkTypeMacro(vtkPartitionBalancer, vtkPassInputTypeAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Get/Set the controller to use. By default
   * vtkMultiProcessController::GlobalController will be used.
   */
  void SetController(vtkMultiProcessController*);
  vtkGetObjectMacro(Controller, vtkMultiProcessController);
  ///@}

  /**
   * Modes defining the layout of the output.
   */
  enum Mode
  {
    Expand,
    Squash
  };

  ///@{
  /**
   * Set / Get current layout of the output.
   */
  vtkSetMacro(Mode, int);
  vtkGetMacro(Mode, int);
  ///@}

  /**
   * Sets filter to expand mode. See example below.
   *
   * @code
   * Input:
   * rank 0: PDC [ PD (DS0, DS1,     DS2) ] [PD (nullptr, DS100) ]
   * rank 1: PDC [ PD (DS3, nullptr, DS4) ] [PD ()               ]
   *
   * Output:
   * rank 0: PDC [ PD (DS0,     DS1,     DS2,     nullptr, nullptr) ] [PD (DS100)   ]
   * rank 1: PDC [ PD (nullptr, nullptr, nullptr, DS3,     DS4)     ] [PD (nullptr) ]
   * @endcode
   */
  void SetModeToExpand() { this->SetMode(Mode::Expand); }

  /**
   * Sets filter to squash mode. See example below.
   *
   * @code
   * Input:
   * rank 0: PDC [ PD (DS0, DS1,     DS2) ] [PD (nullptr, DS100) ]
   * rank 1: PDC [ PD (DS3, nullptr, DS4) ] [PD ()               ]
   *
   * Output:
   * rank 0: PDC [ PD (DS0, DS1, DS2)     ] [PD (DS100)   ]
   * rank 1: PDC [ PD (DS3, DS4, nullptr) ] [PD (nullptr) ]
   * @endcode
   */
  void SetModeToSquash() { this->SetMode(Mode::Squash); }

protected:
  vtkPartitionBalancer();
  ~vtkPartitionBalancer() override;

  int FillInputPortInformation(int port, vtkInformation* info) override;

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  /**
   * Local controller.
   */
  vtkMultiProcessController* Controller;

  int Mode;

private:
  vtkPartitionBalancer(const vtkPartitionBalancer&) = delete;
  void operator=(const vtkPartitionBalancer&) = delete;
};

#endif
