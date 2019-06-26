/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkImageCompare.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef VTKIMAGECOMPARE_H
#define VTKIMAGECOMPARE_H

#include "vtkImageAlgorithm.h"
#include "vtkImagingCoreModule.h" // For export macro

class VTKIMAGINGCORE_EXPORT vtkImageCompare : public vtkImageAlgorithm
{
public:
  static vtkImageCompare* New();
  vtkTypeMacro(vtkImageCompare, vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override {}

  vtkSetClampMacro(FuzzyThreshold, double, 0., 1000.);
  vtkGetMacro(FuzzyThreshold, double);

protected:
  vtkImageCompare();
  ~vtkImageCompare() override {}

  enum MetricType
  {
    UndefinedErrorMetric,
    AbsoluteErrorMetric,
    FuzzErrorMetric,
    MeanAbsoluteErrorMetric,
    MeanErrorPerPixelErrorMetric,
    MeanSquaredErrorMetric,
    NormalizedCrossCorrelationErrorMetric,
    PeakAbsoluteErrorMetric,
    PeakSignalToNoiseRatioErrorMetric,
    PerceptualHashErrorMetric,
    RootMeanSquaredErrorMetric,
    StructuralSimilarityErrorMetric,
    StructuralDissimilarityErrorMetric
  };
  double ErrorThreshold;
  double FuzzyThreshold;
  int ErrorMetric;

  // Outputs
  double Error;
  bool isEqual;

  int RequestInformation(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;
  int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;

private:
  vtkImageCompare(const vtkImageCompare&) = delete;
  void operator=(const vtkImageCompare&) = delete;
};

#endif // VTKIMAGECOMPARE_H
