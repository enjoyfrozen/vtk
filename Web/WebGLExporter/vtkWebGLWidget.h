/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkWebGLWidget
 *
 * Widget representation for WebGL.
*/

#ifndef vtkWebGLWidget_h
#define vtkWebGLWidget_h

#include "vtkWebGLObject.h"
#include "vtkWebGLExporterModule.h" // needed for export macro

#include <vector> // Needed to store colors

class vtkActor2D;

class VTKWEBGLEXPORTER_EXPORT vtkWebGLWidget : public vtkWebGLObject
{
public:
  static vtkWebGLWidget* New();
  vtkTypeMacro(vtkWebGLWidget, vtkWebGLObject);
  void PrintSelf(ostream &os, vtkIndent indent) override;

  void GenerateBinaryData() override;
  unsigned char* GetBinaryData(int part) override;
  int GetBinarySize(int part) override;
  int GetNumberOfParts() override;

  void GetDataFromColorMap(vtkActor2D* actor);

protected:
    vtkWebGLWidget();
    ~vtkWebGLWidget() override;

    unsigned char* binaryData;
    int binarySize;
    int orientation;
    char* title;
    char* textFormat;
    int textPosition;
    float position[2];
    float size[2];
    int numberOfLabels;
    std::vector <double*>colors;      //x, r, g, b

private:
  vtkWebGLWidget(const vtkWebGLWidget&) = delete;
  void operator=(const vtkWebGLWidget&) = delete;

};

#endif
