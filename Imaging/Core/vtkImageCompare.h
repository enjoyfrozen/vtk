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

  //@{
  /**
   * Specify the Image to compare the input to.
   */
  void SetImageConnection(vtkAlgorithmOutput* output)
  {
    this->SetInputConnection(1, output);
  }
  void SetImageData(vtkDataObject* image) { this->SetInputData(1, image); }
  vtkImageData* GetImage();
  //@}

  /**
   * Return the total error in comparing the two images.
   */
  double GetError() { return this->Error; }
  void GetError(double* e) { *e = this->GetError(); };

  vtkGetMacro(FuzzyThreshold, double);
  vtkSetClampMacro(FuzzyThreshold, double, 0., 1.);

  vtkGetMacro(ErrorMetric, int);
  vtkSetClampMacro(ErrorMetric, int, MeanSquaredErrorMetric,
    PeakSignalToNoiseRatioErrorMetric);

  vtkGetMacro(ErrorThreshold, double);
  vtkSetClampMacro(ErrorThreshold, double, 0., 1000.);

protected:
  vtkImageCompare();
  ~vtkImageCompare() override {}

  enum MetricType
  {

    MeanSquaredErrorMetric,                // implemented
    PeakSignalToNoiseRatioErrorMetric,     // implemented
    AbsoluteErrorMetric,                   // not implemented
    FuzzErrorMetric,                       // not implemented
    MeanAbsoluteErrorMetric,               // not implemented
    MeanErrorPerPixelErrorMetric,          // not implemented
    NormalizedCrossCorrelationErrorMetric, // not implemented
    PeakAbsoluteErrorMetric,               // not implemented
    PerceptualHashErrorMetric,             // not implemented
    RootMeanSquaredErrorMetric,            // not implemented
    StructuralSimilarityErrorMetric,       // not implemented
    StructuralDissimilarityErrorMetric     // not implemented
  };

  // Parameters
  double ErrorThreshold;
  double FuzzyThreshold;
  int ErrorMetric;

  // Outputs
  double Error;
  bool isEqual;

  int RequestInformation(vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;
  int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;

private:
  vtkImageCompare(const vtkImageCompare&) = delete;
  void operator=(const vtkImageCompare&) = delete;
  void ComputeError(unsigned char* in1Pixels, unsigned char* in2Pixels,
    int* extent, vtkIdType* in1Incs, vtkIdType* in2Incs);

  class ComputeDifferenceImage;
  class ComputeErrorFunctor;
  class ComputeMeanSquaredError;
  class ComputePeakSignalToNoiseRatioError;
};

#endif // VTKIMAGECOMPARE_H
