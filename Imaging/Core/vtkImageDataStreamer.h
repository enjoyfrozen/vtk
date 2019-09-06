/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImageDataStreamer
 * @brief   Initiates streaming on image data.
 *
 * To satisfy a request, this filter calls update on its input
 * many times with smaller update extents.  All processing up stream
 * streams smaller pieces.
*/

#ifndef vtkImageDataStreamer_h
#define vtkImageDataStreamer_h

#include "vtkImagingCoreModule.h" // For export macro
#include "vtkImageAlgorithm.h"

class vtkExtentTranslator;

class VTKIMAGINGCORE_EXPORT vtkImageDataStreamer : public vtkImageAlgorithm
{
public:
  static vtkImageDataStreamer *New();
  vtkTypeMacro(vtkImageDataStreamer,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set how many pieces to divide the input into.
   * void SetNumberOfStreamDivisions(int num);
   * int GetNumberOfStreamDivisions();
   */
  vtkSetMacro(NumberOfStreamDivisions,int);
  vtkGetMacro(NumberOfStreamDivisions,int);
  //@}

  //@{
  /**
   * Get the extent translator that will be used to split the requests
   */
  virtual void SetExtentTranslator(vtkExtentTranslator*);
  vtkGetObjectMacro(ExtentTranslator,vtkExtentTranslator);
  //@}

  // See the vtkAlgorithm for a description of what these do
  int ProcessRequest(vtkInformation*,
                     vtkInformationVector**,
                     vtkInformationVector*) override;

protected:
  vtkImageDataStreamer();
  ~vtkImageDataStreamer() override;

  vtkExtentTranslator *ExtentTranslator;
  int            NumberOfStreamDivisions;
  int            CurrentDivision;
private:
  vtkImageDataStreamer(const vtkImageDataStreamer&) = delete;
  void operator=(const vtkImageDataStreamer&) = delete;
};

#endif



