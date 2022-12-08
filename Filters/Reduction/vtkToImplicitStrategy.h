/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkToImplicitStrategy.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef vtkToImplicitStrategy_h
#define vtkToImplicitStrategy_h

#include "vtkFiltersReductionModule.h" // for export
#include "vtkObject.h"

VTK_ABI_NAMESPACE_BEGIN
/**
 * @class vtkToImplicitStrategy
 *
 * Pure interface for strategies to transform explicit arrays into implicit arrays. The interface
 * has two main components: an `EstimateReduction` method which estimates by how much this strategy
 * can reduce the memory usage of the array and a `Reduce` method which returns a reduced array.
 */
class vtkDataArray;
class VTKFILTERSREDUCTION_EXPORT vtkToImplicitStrategy : public vtkObject
{
public:
  vtkTypeMacro(vtkToImplicitStrategy, vtkObject);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Getter/Setter for tolerance parameter
   */
  vtkSetMacro(Tolerance, double);
  vtkGetMacro(Tolerance, double);
  ///@}

  /**
   * Estimate the reduction (if possible) that can be obtained on the array using this strategy
   * - if not possible: the return Optional.IsSome member will be false
   * - if possible: the return Option.IsSome member will be true with Optional.Value begin the
   * reduction factor
   */
  struct Optional;
  virtual Optional EstimateReduction(vtkDataArray*) = 0;

  /**
   * Return a reduced version of the input array
   */
  virtual vtkSmartPointer<vtkDataArray> Reduce(vtkDataArray*) = 0;

  /**
   * Destroy any cached variables present in the object (useful for storing calculation results
   * in-between the estimation and reduction phases)
   */
  virtual void ClearCache(){};

protected:
  vtkToImplicitStrategy() = default;
  ~vtkToImplicitStrategy() override = default;

  double Tolerance = 0.001;

private:
  vtkToImplicitStrategy(const vtkToImplicitStrategy&) = delete;
  void operator=(const vtkToImplicitStrategy&) = delete;
};

//-------------------------------------------------------------------------
struct vtkToImplicitStrategy::Optional
{
  bool IsSome = false;
  double Value;

  Optional()
    : IsSome(false)
  {
  }

  Optional(double val)
    : IsSome(true)
    , Value(val)
  {
  }
};
VTK_ABI_NAMESPACE_END

#endif // vtkToImplicitStrategy_h
