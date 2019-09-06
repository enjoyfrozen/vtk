/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkLightsPass
 * @brief   Implement the lights render pass.
 *
 * Render the lights.
 *
 * This pass expects an initialized camera.
 * It disables all the lights, apply transformations for lights following the
 * camera, and turn on the enables lights.
 *
 * @sa
 * vtkRenderPass
*/

#ifndef vtkLightsPass_h
#define vtkLightsPass_h

#include "vtkRenderingOpenGL2Module.h" // For export macro
#include "vtkRenderPass.h"

class vtkOpenGLRenderWindow;

class VTKRENDERINGOPENGL2_EXPORT vtkLightsPass : public vtkRenderPass
{
public:
  static vtkLightsPass *New();
  vtkTypeMacro(vtkLightsPass,vtkRenderPass);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Perform rendering according to a render state \p s.
   * \pre s_exists: s!=0
   */
  void Render(const vtkRenderState *s) override;

 protected:
  /**
   * Default constructor.
   */
  vtkLightsPass();

  /**
   * Destructor.
   */
  ~vtkLightsPass() override;

 private:
  vtkLightsPass(const vtkLightsPass&) = delete;
  void operator=(const vtkLightsPass&) = delete;
};

#endif
