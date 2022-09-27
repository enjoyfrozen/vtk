/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkCylindricalGridSource.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class  vtkCylindricalGridSource
 * @brief  Builds cylindrical cells around the z axis
 *
 * vtkCylindricalGridSource is a source to build cylindrical cells into an unstructured grid. These
 * cells have a radial curvature, up to and including a complete ring about the z axis. Cells are
 * represented by a cartesian radius, polar theta rotation angle about the origin, and a cartesian z
 * height.
 *
 * @sa
 * vtkUnstructuredGrid
 */

#ifndef vtkCylindricalGridSource_h
#define vtkCylindricalGridSource_h

#include "vtkFiltersSourcesModule.h" // For export macro
#include "vtkObject.h"
#include "vtkSmartPointer.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridAlgorithm.h"

class vtkUnstructuredGrid;

class VTKFILTERSSOURCES_EXPORT vtkCylindricalGridSource : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkCylindricalGridSource* New();
  vtkTypeMacro(vtkCylindricalGridSource, vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * The maximum angle which can be rendered without inserting intermediate points.
   *
   * If a cell's polar angle becomes too large, intermediate points will be inserted into the inner
   * and outer curved surfaces of the cell. This preserves the visual rendering of the cell's
   * curvature.
   *
   * Default: 361 degrees (360 + margin to prevent artifacting in 360 degree rings)
   *
   * @warning
   * A polygonal representation of a cylindrical cell is built as soon as the cell is added, so this
   * value should be set beforehand.
   */
  void SetMaximumAngle(double maxAngle);
  double GetMaximumAngle();
  //@}

  //@{
  /**
   * Should any radial coordinates be represented in degrees (true) or radians (false)?
   *
   * Default value is true(1).
   */
  vtkSetMacro(UseDegrees, bool);
  vtkGetMacro(UseDegrees, bool);
  vtkBooleanMacro(UseDegrees, bool);
  //@}

  //@{
  /**
   * Create a new cylindrically shaped polygonal cell.
   * Defined by an inner and outer cartesian radius, starting and ending polar rotation, and
   * cartesian z height. In 2D or 3D depending on how many z coordinates are supplied.
   */
  void InsertNextCylindricalCell(double r1, double r2, double p1, double p2, double z1);
  void InsertNextCylindricalCell(double r1, double r2, double p1, double p2, double z1, double z2);
  //@}

protected:
  vtkCylindricalGridSource();

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  vtkSmartPointer<vtkUnstructuredGrid> Grid;
  double MaximumAngle;
  bool UseDegrees;

private:
  class Impl;

  vtkCylindricalGridSource(const vtkCylindricalGridSource&) = delete;
  void operator=(const vtkCylindricalGridSource&) = delete;
};

#endif // vtkCylindricalGridSource_h

// VTK-HeaderTest-Exclude: vtkCylindricalGridSource.h
