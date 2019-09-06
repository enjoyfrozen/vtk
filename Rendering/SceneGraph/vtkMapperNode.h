/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
/**
 * @class   vtkMapperNode
 * @brief   vtkViewNode specialized for vtkMappers
 *
 * State storage and graph traversal for vtkMapper
*/

#ifndef vtkMapperNode_h
#define vtkMapperNode_h

#include "vtkRenderingSceneGraphModule.h" // For export macro
#include "vtkViewNode.h"

#include <vector> //for results

class vtkAbstractArray;
class vtkDataSet;
class vtkMapper;
class vtkPolyData;

class VTKRENDERINGSCENEGRAPH_EXPORT vtkMapperNode :
  public vtkViewNode
{
public:
  static vtkMapperNode* New();
  vtkTypeMacro(vtkMapperNode, vtkViewNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkMapperNode();
  ~vtkMapperNode();

  vtkAbstractArray *GetArrayToProcess
    (vtkDataSet* input, int& association);

private:
  vtkMapperNode(const vtkMapperNode&) = delete;
  void operator=(const vtkMapperNode&) = delete;
};

#endif
