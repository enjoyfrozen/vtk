/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkHoudiniPolyDataWriter
 * @brief   write vtk polygonal data to Houdini file.
 *
 *
 * vtkHoudiniPolyDataWriter is a source object that writes VTK polygonal data
 * files in ASCII Houdini format (see
 * http://www.sidefx.com/docs/houdini15.0/io/formats/geo).
*/

#ifndef vtkHoudiniPolyDataWriter_h
#define vtkHoudiniPolyDataWriter_h

#include "vtkIOGeometryModule.h" // For export macro
#include "vtkWriter.h"

class vtkPolyData;

class VTKIOGEOMETRY_EXPORT vtkHoudiniPolyDataWriter : public vtkWriter
{
public:
  static vtkHoudiniPolyDataWriter* New();
  vtkTypeMacro(vtkHoudiniPolyDataWriter, vtkWriter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Specifies the delimited text file to be loaded.
   */
  vtkGetStringMacro(FileName);
  vtkSetStringMacro(FileName);
  //@}

protected:
  vtkHoudiniPolyDataWriter();
  ~vtkHoudiniPolyDataWriter() override;

  void WriteData() override;

  int FillInputPortInformation(int port, vtkInformation *info) override;

  char* FileName;

private:
  vtkHoudiniPolyDataWriter(const vtkHoudiniPolyDataWriter&) = delete;
  void operator=(const vtkHoudiniPolyDataWriter&) = delete;

};

#endif
