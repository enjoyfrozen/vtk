/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkEmbreePointLocator.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkEmbreePointLocator
 * @brief   Uses Intel Embree's BVH to quickly locate points in 3-space
 *
 */

#ifndef vtkEmbreePointLocator_h
#define vtkEmbreePointLocator_h

#include "vtkAbstractPointLocator.h"
#include "vtkFiltersSolarModule.h" // For export macro

class VTKFILTERSSOLAR_EXPORT vtkEmbreePointLocator : public vtkAbstractPointLocator
{
public:
  static vtkEmbreePointLocator* New();

  ///@{
  /**
   * Standard methods for type management and printing.
   */
  vtkTypeMacro(vtkEmbreePointLocator, vtkAbstractPointLocator);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  /** overridden to provide implementation */
  virtual void BuildLocator() override;

  /** overridden to provide implementation */
  virtual void FreeSearchStructure() override;

  /** overridden to provide implementation */
  virtual void GenerateRepresentation(int level, vtkPolyData* pd) override;

  /** overridden to provide implementation */
  virtual vtkIdType FindClosestPoint(const double x[3]) override;

  /** overridden to provide implementation */
  virtual vtkIdType FindClosestPointWithinRadius(
    double radius, const double x[3], double& dist2) override;

  /** overridden to provide implementation */
  virtual void FindClosestNPoints(int N, const double x[3], vtkIdList* result) override;

  /** overridden to provide implementation */
  virtual void FindPointsWithinRadius(double R, const double x[3], vtkIdList* result) override;

  // PIMPL
  class vtkInternals;
  vtkInternals* Internals;

protected:
  vtkEmbreePointLocator();
  ~vtkEmbreePointLocator() override;

private:
  vtkEmbreePointLocator(const vtkEmbreePointLocator&) = delete;
  void operator=(const vtkEmbreePointLocator&) = delete;
};

#endif
