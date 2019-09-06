/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkVolumeMapperNode
 * @brief   vtkViewNode specialized for vtkVolumeMappers
 *
 * State storage and graph traversal for vtkVolumeMapper/PolyDataMapper and Property
 * Made a choice to merge PolyDataMapper, PolyDataMapper and property together. If there
 * is a compelling reason to separate them we can.
*/

#ifndef vtkVolumeMapperNode_h
#define vtkVolumeMapperNode_h

#include "vtkRenderingSceneGraphModule.h" // For export macro
#include "vtkMapperNode.h"

#include <vector> //for results

class vtkActor;
class vtkVolumeMapper;
class vtkPolyData;

class VTKRENDERINGSCENEGRAPH_EXPORT vtkVolumeMapperNode :
  public vtkMapperNode
{
public:
  static vtkVolumeMapperNode* New();
  vtkTypeMacro(vtkVolumeMapperNode, vtkMapperNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkVolumeMapperNode();
  ~vtkVolumeMapperNode();

 private:
  vtkVolumeMapperNode(const vtkVolumeMapperNode&) = delete;
  void operator=(const vtkVolumeMapperNode&) = delete;
};

#endif
