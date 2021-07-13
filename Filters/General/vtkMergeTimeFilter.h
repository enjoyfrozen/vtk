/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMergeTimeFilter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkMergeTimeFilter
 * @brief   Create a multiblock containing one block per input, with unified timestep list.
 *
 * vtkMergeTimeFilter creates a multiblock dataset containing one block per input dataset.
 * It merges the inputs timesteps lists, either computing the union or the intersection.
 * In both case, duplicates are removed, dependending on a Tolerance either absolute or relative.
 */

#ifndef vtkMergeTimeFilter_h
#define vtkMergeTimeFilter_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkMultiBlockDataSetAlgorithm.h"

#include <vector> // Use of dynamically allocated array

class VTKFILTERSGENERAL_EXPORT vtkMergeTimeFilter : public vtkMultiBlockDataSetAlgorithm
{
public:
  static vtkMergeTimeFilter* New();
  vtkTypeMacro(vtkMergeTimeFilter, vtkMultiBlockDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set/Get the tolerance for comparing time step
   * values to see if they are close enough to be considered
   * identical. Default is 0.00001
   */
  vtkSetClampMacro(Tolerance, double, 0, VTK_DOUBLE_MAX);
  vtkGetMacro(Tolerance, double);
  //@}

  //@{
  /**
   * Set/Get if the tolerance is relative or absolute. Default is absolute.
   */
  vtkSetMacro(UseRelativeTolerance, bool);
  vtkGetMacro(UseRelativeTolerance, bool);
  vtkBooleanMacro(UseRelativeTolerance, bool);
  //@}

  //@{
  /**
   * Set/Get if the merge use intersection instead of union.
   * Default False (union is used).
   */
  vtkSetMacro(UseIntersection, bool);
  vtkGetMacro(UseIntersection, bool);
  vtkBooleanMacro(UseIntersection, bool);
  //@}

protected:
  vtkMergeTimeFilter() = default;
  ~vtkMergeTimeFilter() override = default;

  /**
   * Compute global extent and timesteps list, depending on Tolerance.
   */
  int RequestInformation(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;

  /**
   * Append contribution from each input.
   */
  int RequestUpdateExtent(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  /**
   * Override to allow multiple inputs.
   */
  int FillInputPortInformation(int port, vtkInformation* info) override;

  /**
   * Create an ordered combination of given timesteps and current OutputTimeSteps list.
   * Avoid duplicate (use Tolerance).
   */
  void MergeTimeSteps(const std::vector<double>& timesteps);

  /**
   * Look for an input time inside Tolerance.
   * If not found, return outputTime.
   */
  double MapToInputTime(int input, double outputTime);

  /**
   * Return true if t1 and t2 are inside Tolerance.
   */
  bool AreTimesInTolerance(double t1, double t2);

  double Tolerance = 0.00001;
  bool UseRelativeTolerance = false;
  bool UseIntersection = false;

  double RequestedTimeValue = 0.;
  std::vector<double> OutputTimeSteps;
  std::vector<std::vector<double>> InputsTimeSteps;

private:
  vtkMergeTimeFilter(const vtkMergeTimeFilter&) = delete;
  void operator=(const vtkMergeTimeFilter&) = delete;
};

#endif
