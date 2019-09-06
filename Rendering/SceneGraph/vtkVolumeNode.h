/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkVolumeNode
 * @brief   vtkViewNode specialized for vtkActors
 *
 * State storage and graph traversal for vtkActor/Mapper and Property
 * Made a choice to merge actor, mapper and property together. If there
 * is a compelling reason to separate them we can.
*/

#ifndef vtkVolumeNode_h
#define vtkVolumeNode_h

#include "vtkRenderingSceneGraphModule.h" // For export macro
#include "vtkViewNode.h"

class VTKRENDERINGSCENEGRAPH_EXPORT vtkVolumeNode :
  public vtkViewNode
{
public:
  static vtkVolumeNode* New();
  vtkTypeMacro(vtkVolumeNode, vtkViewNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Build containers for our child nodes.
   */
  virtual void Build(bool prepass) override;

protected:
  vtkVolumeNode();
  ~vtkVolumeNode();

private:
  vtkVolumeNode(const vtkVolumeNode&) = delete;
  void operator=(const vtkVolumeNode&) = delete;
};

#endif
