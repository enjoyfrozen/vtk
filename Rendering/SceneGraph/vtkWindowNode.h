/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkWindowNode
 * @brief   vtkViewNode specialized for vtkRenderWindows
 *
 * State storage and graph traversal for vtkRenderWindow
*/

#ifndef vtkWindowNode_h
#define vtkWindowNode_h

#include "vtkRenderingSceneGraphModule.h" // For export macro
#include "vtkViewNode.h"

class vtkUnsignedCharArray;
class vtkFloatArray;

class VTKRENDERINGSCENEGRAPH_EXPORT vtkWindowNode :
  public vtkViewNode
{
public:
  static vtkWindowNode* New();
  vtkTypeMacro(vtkWindowNode, vtkViewNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Build containers for our child nodes.
   */
  virtual void Build(bool prepass) override;

  /**
   * Get state of my renderable.
   */
  virtual void Synchronize(bool prepass) override;

  /**
   * Return the size of the last rendered image
   */
  virtual int *GetSize() {
    return this->Size; }

  /**
   * Get the most recent color buffer RGBA
   */
  virtual vtkUnsignedCharArray *GetColorBuffer()
    { return this->ColorBuffer; }

  /**
   * Get the most recent zbuffer buffer
   */
  virtual vtkFloatArray *GetZBuffer()
    { return this->ZBuffer; }

protected:
  vtkWindowNode();
  ~vtkWindowNode();

  //TODO: use a map with string keys being renderable's member name
  //state
  int Size[2];

  // stores the results of a render
  vtkUnsignedCharArray*ColorBuffer;
  vtkFloatArray *ZBuffer;

private:
  vtkWindowNode(const vtkWindowNode&) = delete;
  void operator=(const vtkWindowNode&) = delete;
};

#endif
