/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkCameraNode
 * @brief   vtkViewNode specialized for vtkCameras
 *
 * State storage and graph traversal for vtkCamera
*/

#ifndef vtkCameraNode_h
#define vtkCameraNode_h

#include "vtkRenderingSceneGraphModule.h" // For export macro
#include "vtkViewNode.h"

class VTKRENDERINGSCENEGRAPH_EXPORT vtkCameraNode :
  public vtkViewNode
{
public:
  static vtkCameraNode* New();
  vtkTypeMacro(vtkCameraNode, vtkViewNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkCameraNode();
  ~vtkCameraNode();

private:
  vtkCameraNode(const vtkCameraNode&) = delete;
  void operator=(const vtkCameraNode&) = delete;
};

#endif
