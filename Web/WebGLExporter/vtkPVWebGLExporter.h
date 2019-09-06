/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#ifndef vtkPVWebGLExporter_h
#define vtkPVWebGLExporter_h

#include "vtkExporter.h"
#include "vtkWebGLExporterModule.h" // needed for export macro

class VTKWEBGLEXPORTER_EXPORT vtkPVWebGLExporter : public vtkExporter
{
public:
  static vtkPVWebGLExporter *New();
  vtkTypeMacro(vtkPVWebGLExporter,vtkExporter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  // Description:
  // Specify the name of the VRML file to write.
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

protected:
  vtkPVWebGLExporter();
  ~vtkPVWebGLExporter() override;

  void WriteData() override;

  char *FileName;

private:
  vtkPVWebGLExporter(const vtkPVWebGLExporter&) = delete;
  void operator=(const vtkPVWebGLExporter&) = delete;
};

#endif
