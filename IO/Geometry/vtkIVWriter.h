/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkIVWriter
 * @brief   export polydata into OpenInventor 2.0 format.
 *
 * vtkIVWriter is a concrete subclass of vtkWriter that writes OpenInventor 2.0
 * files.
 *
 * @sa
 * vtkPolyDataWriter
*/

#ifndef vtkIVWriter_h
#define vtkIVWriter_h

#include "vtkIOGeometryModule.h" // For export macro
#include "vtkWriter.h"

class vtkPolyData;

class VTKIOGEOMETRY_EXPORT vtkIVWriter : public vtkWriter
{
public:
  static vtkIVWriter *New();
  vtkTypeMacro(vtkIVWriter,vtkWriter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Get the input to this writer.
   */
  vtkPolyData* GetInput();
  vtkPolyData* GetInput(int port);
  //@}

  //@{
  /**
   * Specify file name of vtk polygon data file to write.
   */
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  //@}

protected:
  vtkIVWriter()
  {
    this->FileName = nullptr;
  }

  ~vtkIVWriter() override
  {
    delete[] this->FileName;
  }

  void WriteData() override;
  void WritePolyData(vtkPolyData *polyData, FILE *fp);

  char *FileName;

  int FillInputPortInformation(int port, vtkInformation *info) override;

private:
  vtkIVWriter(const vtkIVWriter&) = delete;
  void operator=(const vtkIVWriter&) = delete;
};

#endif

