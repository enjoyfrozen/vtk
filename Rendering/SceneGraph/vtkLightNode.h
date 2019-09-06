/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkLightNode
 * @brief   vtkViewNode specialized for vtkLights
 *
 * State storage and graph traversal for vtkLight
*/

#ifndef vtkLightNode_h
#define vtkLightNode_h

#include "vtkRenderingSceneGraphModule.h" // For export macro
#include "vtkViewNode.h"

class VTKRENDERINGSCENEGRAPH_EXPORT vtkLightNode :
  public vtkViewNode
{
public:
  static vtkLightNode* New();
  vtkTypeMacro(vtkLightNode, vtkViewNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkLightNode();
  ~vtkLightNode();

private:
  vtkLightNode(const vtkLightNode&) = delete;
  void operator=(const vtkLightNode&) = delete;
};

#endif
