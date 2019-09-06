/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImageStencilAlgorithm
 * @brief   producer of vtkImageStencilData
 *
 * vtkImageStencilAlgorithm is a superclass for filters that generate
 * the special vtkImageStencilData type.  This data type is a special
 * representation of a binary image that can be used as a mask by
 * several imaging filters.
 * @sa
 * vtkImageStencilData vtkImageStencilSource
*/

#ifndef vtkImageStencilAlgorithm_h
#define vtkImageStencilAlgorithm_h


#include "vtkImagingCoreModule.h" // For export macro
#include "vtkAlgorithm.h"

class vtkImageStencilData;

class VTKIMAGINGCORE_EXPORT vtkImageStencilAlgorithm : public vtkAlgorithm
{
public:
  static vtkImageStencilAlgorithm *New();
  vtkTypeMacro(vtkImageStencilAlgorithm, vtkAlgorithm);

  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Get or set the output for this source.
   */
  void SetOutput(vtkImageStencilData *output);
  vtkImageStencilData *GetOutput();
  //@}

  /**
   * see vtkAlgorithm for details
   */
  int ProcessRequest(vtkInformation*,
                             vtkInformationVector**,
                             vtkInformationVector*) override;

protected:
  vtkImageStencilAlgorithm();
  ~vtkImageStencilAlgorithm() override;

  virtual int RequestData(vtkInformation *, vtkInformationVector **,
                  vtkInformationVector *);
  virtual int RequestInformation(vtkInformation *, vtkInformationVector **,
                                 vtkInformationVector *);
  virtual int RequestUpdateExtent(vtkInformation *, vtkInformationVector **,
                                  vtkInformationVector *);
  vtkImageStencilData *AllocateOutputData(vtkDataObject *out, int* updateExt);

  int FillOutputPortInformation(int, vtkInformation*) override;

private:
  vtkImageStencilAlgorithm(const vtkImageStencilAlgorithm&) = delete;
  void operator=(const vtkImageStencilAlgorithm&) = delete;
};

#endif
