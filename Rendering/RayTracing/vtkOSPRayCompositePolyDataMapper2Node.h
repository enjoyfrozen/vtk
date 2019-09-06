/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkOSPRayCompositePolyDataMapper2Node
 * @brief   links vtkActor and vtkMapper to OSPRay
 *
 * Translates vtkActor/Mapper state into OSPRay rendering calls
*/

#ifndef vtkOSPRayCompositePolyDataMapper2Node_h
#define vtkOSPRayCompositePolyDataMapper2Node_h

#include "vtkRenderingRayTracingModule.h" // For export macro
#include "vtkOSPRayPolyDataMapperNode.h"
#include "vtkColor.h" // used for ivars
#include <stack> // used for ivars

class vtkDataObject;
class vtkCompositePolyDataMapper2;
class vtkOSPRayRendererNode;

class VTKRENDERINGRAYTRACING_EXPORT vtkOSPRayCompositePolyDataMapper2Node :
  public vtkOSPRayPolyDataMapperNode
{
public:
  static vtkOSPRayCompositePolyDataMapper2Node* New();
  vtkTypeMacro(vtkOSPRayCompositePolyDataMapper2Node, vtkOSPRayPolyDataMapperNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Make ospray calls to render me.
   */
  virtual void Render(bool prepass) override;

  /**
   * Invalidates cached rendering data.
   */
  virtual void Invalidate(bool prepass) override;

protected:
  vtkOSPRayCompositePolyDataMapper2Node();
  ~vtkOSPRayCompositePolyDataMapper2Node();

    class RenderBlockState
    {
    public:
      std::stack<bool> Visibility;
      std::stack<double> Opacity;
      std::stack<vtkColor3d> AmbientColor;
      std::stack<vtkColor3d> DiffuseColor;
      std::stack<vtkColor3d> SpecularColor;
      std::stack<std::string> Material;
    };

  RenderBlockState BlockState;
  void RenderBlock(vtkOSPRayRendererNode *orn,
                   vtkCompositePolyDataMapper2 *cpdm,
                   vtkActor *actor,
                   vtkDataObject *dobj,
                   unsigned int &flat_index);


private:
  vtkOSPRayCompositePolyDataMapper2Node(const vtkOSPRayCompositePolyDataMapper2Node&) = delete;
  void operator=(const vtkOSPRayCompositePolyDataMapper2Node&) = delete;
};
#endif
