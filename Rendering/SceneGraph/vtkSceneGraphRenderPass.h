/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSceneGraphRenderPass.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkSceneGraphRenderPass
 * @brief Abstract class that provides API for performing custom rendering operations on the
 * scenegraph. It will traverse all of its preDelegateOperations, then call its delegate render
 * passes and then traverse all of its postDelegateOperations.
 *
 */

#ifndef vtkSceneGraphRenderPass_h
#define vtkSceneGraphRenderPass_h

// vtk includes
#include "vtkRenderPass.h"
#include "vtkRenderingSceneGraphModule.h" // For export macro

// STL includes
#include <vector>

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations
class vtkRenderPassCollection;
class vtkRenderState;
class vtkViewNode;

class VTKRENDERINGSCENEGRAPH_EXPORT vtkSceneGraphRenderPass : public vtkRenderPass
{
public:
  /**
   * Instantiate the class.
   */
  static vtkSceneGraphRenderPass* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkSceneGraphRenderPass, vtkRenderPass);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  ///@{
  /**
   * Subclasses must implement the following methods
   */
  vtkGetObjectMacro(DelegatePasses, vtkRenderPassCollection);
  virtual void AddDelegatePass(vtkRenderPass*);
  ///@}

  /**
   * Overridden to allow traversal over passes
   */
  void Render(const vtkRenderState* s) override;

  /**
   * Traverse a viewNode and apply the pre operations, delegate passes and post operations
   */
  virtual void Traverse(vtkViewNode* vn, vtkRenderPass* parent = nullptr);

protected:
  vtkSceneGraphRenderPass();
  ~vtkSceneGraphRenderPass();

  // Helper members
  vtkRenderPassCollection* DelegatePasses;
  std::vector<int> PreOperations;
  std::vector<int> PostOperations;
  vtkRenderPass* CurrentParent = nullptr;

private:
  vtkSceneGraphRenderPass(const vtkSceneGraphRenderPass&) = delete;
  void operator=(const vtkSceneGraphRenderPass) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkSceneGraphRenderPass_h
