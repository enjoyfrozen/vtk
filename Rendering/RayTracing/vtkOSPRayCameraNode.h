/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkOSPRayCameraNode
 * @brief   links vtkCamera to OSPRay
 *
 * Translates vtkCamera state into OSPRay rendering calls
*/

#ifndef vtkOSPRayCameraNode_h
#define vtkOSPRayCameraNode_h

#include "vtkRenderingRayTracingModule.h" // For export macro
#include "vtkCameraNode.h"

class vtkInformationIntegerKey;
class vtkCamera;

class VTKRENDERINGRAYTRACING_EXPORT vtkOSPRayCameraNode :
  public vtkCameraNode
{
public:
  static vtkOSPRayCameraNode* New();
  vtkTypeMacro(vtkOSPRayCameraNode, vtkCameraNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Make ospray calls to render me.
   */
  virtual void Render(bool prepass) override;

protected:
  vtkOSPRayCameraNode();
  ~vtkOSPRayCameraNode();

private:
  vtkOSPRayCameraNode(const vtkOSPRayCameraNode&) = delete;
  void operator=(const vtkOSPRayCameraNode&) = delete;
};

#endif
