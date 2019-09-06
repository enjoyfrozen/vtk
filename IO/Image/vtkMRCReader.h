/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
/**
 * @class   vtkMRCReader
 * @brief   read MRC image files
 *
 *
 * A reader to load MRC images.  See http://bio3d.colorado.edu/imod/doc/mrc_format.txt
 * for the file format specification.
*/

#ifndef vtkMRCReader_h
#define vtkMRCReader_h

#include "vtkImageAlgorithm.h"
#include "vtkIOImageModule.h" // For export macro

class vtkInformation;
class vtkInformationVector;

class VTKIOIMAGE_EXPORT vtkMRCReader : public vtkImageAlgorithm
{
public:
  static vtkMRCReader* New();
  vtkTypeMacro(vtkMRCReader, vtkImageAlgorithm)

  void PrintSelf(ostream& stream, vtkIndent indent) override;

  // .Description
  // Get/Set the file to read
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

protected:
  vtkMRCReader();
  ~vtkMRCReader() override;

  int RequestInformation(vtkInformation* request,
                                 vtkInformationVector** inputVector,
                                 vtkInformationVector* outputVector) override;
  void ExecuteDataWithInformation(vtkDataObject *output,
                                          vtkInformation* outInfo) override;

  char* FileName;

private:
  vtkMRCReader(const vtkMRCReader&) = delete;
  void operator=(const vtkMRCReader&) = delete;
  class vtkInternal;
  vtkInternal* Internals;

};

#endif
