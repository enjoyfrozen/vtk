/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkPNMWriter
 * @brief   Writes PNM (portable any map)  files.
 *
 * vtkPNMWriter writes PNM file. The data type
 * of the file is unsigned char regardless of the input type.
*/

#ifndef vtkPNMWriter_h
#define vtkPNMWriter_h

#include "vtkIOImageModule.h" // For export macro
#include "vtkImageWriter.h"

class VTKIOIMAGE_EXPORT vtkPNMWriter : public vtkImageWriter
{
public:
  static vtkPNMWriter *New();
  vtkTypeMacro(vtkPNMWriter,vtkImageWriter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkPNMWriter() {}
  ~vtkPNMWriter() override {}

  void WriteFile(
    ostream *file, vtkImageData *data, int extent[6], int wExt[6]) override;
  void WriteFileHeader(
    ostream *, vtkImageData *, int wExt[6]) override;
private:
  vtkPNMWriter(const vtkPNMWriter&) = delete;
  void operator=(const vtkPNMWriter&) = delete;
};

#endif


