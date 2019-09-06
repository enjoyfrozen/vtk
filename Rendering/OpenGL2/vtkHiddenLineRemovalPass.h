/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkHiddenLineRemovalPass
 * @brief   RenderPass for HLR.
 *
 *
 * This render pass renders wireframe polydata such that only the front
 * wireframe surface is drawn.
*/

#ifndef vtkHiddenLineRemovalPass_h
#define vtkHiddenLineRemovalPass_h

#include "vtkRenderingOpenGL2Module.h" // For export macro
#include "vtkOpenGLRenderPass.h"

#include <vector> // For std::vector!

class vtkProp;
class vtkViewport;

class VTKRENDERINGOPENGL2_EXPORT vtkHiddenLineRemovalPass :
    public vtkOpenGLRenderPass
{
public:
  static vtkHiddenLineRemovalPass* New();
  vtkTypeMacro(vtkHiddenLineRemovalPass, vtkOpenGLRenderPass)
  void PrintSelf(ostream &os, vtkIndent indent) override;

  void Render(const vtkRenderState *s) override;

  /**
   * Returns true if any of the nProps in propArray are rendered as wireframe.
   */
  static bool WireframePropsExist(vtkProp **propArray, int nProps);

protected:
  vtkHiddenLineRemovalPass();
  ~vtkHiddenLineRemovalPass() override;

  void SetRepresentation(std::vector<vtkProp*> &props, int repr);
  int RenderProps(std::vector<vtkProp*> &props, vtkViewport *vp);

private:
  vtkHiddenLineRemovalPass(const vtkHiddenLineRemovalPass&) = delete;
  void operator=(const vtkHiddenLineRemovalPass&) = delete;
};

#endif // vtkHiddenLineRemovalPass_h
