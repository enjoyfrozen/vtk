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

class VTKFILTERSPOINTS_EXPORT vtkBinningFilter : public vtkImageAlgorithm
{
public:
  static vtkBinningFilter* New();
  vtkTypeMacro(vtkBinningFilter, vtkImageAlgorithm);

  //@{
  /**
   * Get / Set the dimensions of the output grid.
   */
  vtkSetVector3Macro(Dimensions, int);
  vtkGetVector3Macro(Dimensions, int);
  //@}

protected:
  vtkBinningFilter();
  ~vtkBinningFilter() override;

  int FillInputPortInformation(int port, vtkInformation* info) override;
  int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  int Dimensions[3];

  class Internals;
  Internals* Internal;

private:
  vtkBinningFilter(const vtkBinningFilter&) = delete;
  void operator=(const vtkBinningFilter&) = delete;
};

#endif
