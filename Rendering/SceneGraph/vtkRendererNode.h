/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkRendererNode
 * @brief   vtkViewNode specialized for vtkRenderers
 *
 * State storage and graph traversal for vtkRenderer
*/

#ifndef vtkRendererNode_h
#define vtkRendererNode_h

#include "vtkRenderingSceneGraphModule.h" // For export macro
#include "vtkViewNode.h"

class  vtkCollection;

class VTKRENDERINGSCENEGRAPH_EXPORT vtkRendererNode :
  public vtkViewNode
{
public:
  static vtkRendererNode* New();
  vtkTypeMacro(vtkRendererNode, vtkViewNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Build containers for our child nodes.
   */
  virtual void Build(bool prepass) override;

  /**
   * Synchronize our state
   */
  virtual void Synchronize(bool prepass) override;

protected:
  vtkRendererNode();
  ~vtkRendererNode();

  int Size[2];

private:
  vtkRendererNode(const vtkRendererNode&) = delete;
  void operator=(const vtkRendererNode&) = delete;
};

#endif
