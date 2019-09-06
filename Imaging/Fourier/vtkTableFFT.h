/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
// -*- c++ -*-


/**
 * @class   vtkTableFFT
 * @brief   FFT for table columns
 *
 *
 *
 * vtkTableFFT performs the Fast Fourier Transform on the columns of a table.
 * Internally, it shoves each column into an image data and then uses
 * vtkImageFFT to perform the actual FFT.
 *
 *
 * @sa
 * vtkImageFFT
 *
*/

#ifndef vtkTableFFT_h
#define vtkTableFFT_h

#include "vtkTableAlgorithm.h"
#include "vtkImagingFourierModule.h" // For export macro
#include "vtkSmartPointer.h"    // For internal method.

class VTKIMAGINGFOURIER_EXPORT vtkTableFFT : public vtkTableAlgorithm
{
public:
  vtkTypeMacro(vtkTableFFT, vtkTableAlgorithm);
  static vtkTableFFT *New();
  void PrintSelf(ostream &os, vtkIndent indent) override;

protected:
  vtkTableFFT();
  ~vtkTableFFT() override;

  int RequestData(vtkInformation *request,
                          vtkInformationVector **inputVector,
                          vtkInformationVector *outputVector) override;

  /**
   * Perform the FFT on the given data array.
   */
  virtual vtkSmartPointer<vtkDataArray> DoFFT(vtkDataArray *input);

private:
  vtkTableFFT(const vtkTableFFT &) = delete;
  void operator=(const vtkTableFFT &) = delete;
};


#endif //vtkTableFFT_h
