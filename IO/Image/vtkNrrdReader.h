/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
// -*- c++ -*-

/**
 * @class   vtkNrrdReader
 * @brief   Read nrrd files file system
 *
 *
 *
 *
 * @bug
 * There are several limitations on what type of nrrd files we can read.  This
 * reader only supports nrrd files in raw or ascii format.  Other encodings
 * like hex will result in errors.  When reading in detached headers, this only
 * supports reading one file that is detached.
 *
*/

#ifndef vtkNrrdReader_h
#define vtkNrrdReader_h

#include "vtkIOImageModule.h" // For export macro
#include "vtkImageReader.h"

class vtkCharArray;

class VTKIOIMAGE_EXPORT vtkNrrdReader : public vtkImageReader
{
public:
  vtkTypeMacro(vtkNrrdReader, vtkImageReader);
  static vtkNrrdReader *New();
  void PrintSelf(ostream &os, vtkIndent indent) override;

  int CanReadFile(const char *filename) override;

protected:
  vtkNrrdReader();
  ~vtkNrrdReader() override;

  int RequestInformation(vtkInformation *request,
                                 vtkInformationVector **inputVector,
                                 vtkInformationVector *outputVector) override;

  int RequestData(vtkInformation *request,
                          vtkInformationVector **inputVector,
                          vtkInformationVector *outputVector) override;

  int ReadHeaderInternal(vtkCharArray *headerBuffer);
  virtual int ReadHeader();
  virtual int ReadHeader(vtkCharArray *headerBuffer);

  virtual int ReadDataAscii(vtkImageData *output);

  vtkStringArray *DataFiles;

  enum {
    ENCODING_RAW,
    ENCODING_ASCII
  };

  int Encoding;

private:
  vtkNrrdReader(const vtkNrrdReader &) = delete;
  void operator=(const vtkNrrdReader &) = delete;
};

#endif //vtkNrrdReader_h
