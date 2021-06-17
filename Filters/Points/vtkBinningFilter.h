/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkBinningFilter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef vtkBinningFilter_h
#define vtkBinningFilter_h

#include "vtkFiltersPointsModule.h" // For export macro
#include "vtkImageAlgorithm.h"
#include "vtkSetGet.h"

class vtkPointSet;

class VTKFILTERSPOINTS_EXPORT vtkBinningFilter : public vtkImageAlgorithm
{
public:
  static vtkBinningFilter* New();
  vtkTypeMacro(vtkBinningFilter, vtkImageAlgorithm);

  //@{
  /**
   * Get / Set the dimensions of the output grid.
   * This is the number of cells in each direction.
   */
  vtkSetVector3Macro(Dimensions, int);
  vtkGetVector3Macro(Dimensions, int);
  //@}

  //@{
  /**
   * Turn on / off the use of input bounding box
   * to initialize the output grid.
   * @sa OutputBounds
   */
  vtkSetMacro(UseInputBounds, bool);
  vtkGetMacro(UseInputBounds, bool);
  vtkBooleanMacro(UseInputBounds, bool);
  //@}

  //@{
  /**
   * Get / Set the bounds of the output grid.
   * Only used if UseInputBounds is false.
   */
  vtkSetVector6Macro(OutputBounds, double);
  vtkGetVector6Macro(OutputBounds, double);
  //@}

  //@{
  /**
   * Get / Set the particule of interest for concentration computation.
   * The Input array to process should be a list of particles types.
   * For each cell, concentration is given by count(ParticleOfInterest) / count(Particles).
   */
  vtkSetMacro(ParticleOfInterest, int);
  vtkGetMacro(ParticleOfInterest, int);
  //@}

protected:
  vtkBinningFilter();
  ~vtkBinningFilter() override = default;

  int FillInputPortInformation(int port, vtkInformation* info) override;
  int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  /**
   * Compute output CellData from input PointData
   * output cell value is the mean of enclosed input points values.
   */
  bool ComputeCellData(vtkInformationVector** inputVector, vtkImageData* output);

  vtkIdType GetCellId(double pts[3]);

  int Dimensions[3];
  double OutputBounds[6];
  bool UseInputBounds;
  int ParticleOfInterest;

private:
  vtkBinningFilter(const vtkBinningFilter&) = delete;
  void operator=(const vtkBinningFilter&) = delete;
};

#endif
