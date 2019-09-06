/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkOpenGLPropItem
 * @brief   Sync Context2D state with vtk camera.
 *
 *
 * The vtkContext2D framework modifies the GL state directly, while some actors
 * and mappers rely on the modelview/projection matrices from vtkCamera. This
 * class is a layer between the two that updates the camera with the current
 * OpenGL state.
*/

#ifndef vtkOpenGLPropItem_h
#define vtkOpenGLPropItem_h

#include "vtkRenderingContextOpenGL2Module.h" // For export macro
#include "vtkPropItem.h"
#include "vtkNew.h" // for vtkNew

class vtkCamera;

class VTKRENDERINGCONTEXTOPENGL2_EXPORT vtkOpenGLPropItem: public vtkPropItem
{
public:
  static vtkOpenGLPropItem *New();
  vtkTypeMacro(vtkOpenGLPropItem, vtkPropItem)

  bool Paint(vtkContext2D *painter) override;

protected:
  vtkOpenGLPropItem();
  ~vtkOpenGLPropItem() override;

  // Sync the active vtkCamera with the GL state set by the painter.
  void UpdateTransforms() override;

  // Restore the vtkCamera state.
  void ResetTransforms() override;

private:
  vtkNew<vtkCamera> CameraCache;
  vtkContext2D *Painter;

  vtkOpenGLPropItem(const vtkOpenGLPropItem &) = delete;
  void operator=(const vtkOpenGLPropItem &) = delete;
};

#endif //vtkOpenGLPropItem_h
