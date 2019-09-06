/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkOSPRayWindowNode
 * @brief   links vtkRendererWindows to OSPRay
 *
 * Translates vtkRenderWindow state into OSPRay rendering calls
*/

#ifndef vtkOSPRayWindowNode_h
#define vtkOSPRayWindowNode_h

#include "vtkRenderingRayTracingModule.h" // For export macro
#include "vtkWindowNode.h"

class VTKRENDERINGRAYTRACING_EXPORT vtkOSPRayWindowNode :
  public vtkWindowNode
{
public:
  static vtkOSPRayWindowNode* New();
  vtkTypeMacro(vtkOSPRayWindowNode, vtkWindowNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Make ospray calls to render me.
   */
  virtual void Render(bool prepass) override;

protected:
  vtkOSPRayWindowNode();
  ~vtkOSPRayWindowNode();

private:
  vtkOSPRayWindowNode(const vtkOSPRayWindowNode&) = delete;
  void operator=(const vtkOSPRayWindowNode&) = delete;
};

#endif
