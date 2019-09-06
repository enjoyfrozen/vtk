/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkOpenGLGL2PSExporter
 * @brief   OpenGL2 implementation of GL2PS exporter.
 *
 *
 * Implementation of vtkGL2PSExporter for the OpenGL2 backend.
*/

#ifndef vtkOpenGLGL2PSExporter_h
#define vtkOpenGLGL2PSExporter_h

#include "vtkIOExportGL2PSModule.h" // For export macro
#include "vtkGL2PSExporter.h"

class vtkImageData;

class VTKIOEXPORTGL2PS_EXPORT vtkOpenGLGL2PSExporter: public vtkGL2PSExporter
{
public:
  static vtkOpenGLGL2PSExporter *New();
  vtkTypeMacro(vtkOpenGLGL2PSExporter, vtkGL2PSExporter)
  void PrintSelf(ostream &os, vtkIndent indent) override;

protected:
  vtkOpenGLGL2PSExporter();
  ~vtkOpenGLGL2PSExporter() override;

  void WriteData() override;

  bool RasterizeBackground(vtkImageData *image);
  bool CaptureVectorProps();

private:
  vtkOpenGLGL2PSExporter(const vtkOpenGLGL2PSExporter &) = delete;
  void operator=(const vtkOpenGLGL2PSExporter &) = delete;
};

#endif // vtkOpenGLGL2PSExporter_h
