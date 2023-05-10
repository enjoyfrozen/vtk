/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUForwardPass.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUForwardPass
 * @brief A forward rendering pipeline for webgpu
 */

#ifndef vtkWebGPUForwardPass_h
#define vtkWebGPUForwardPass_h

// vtk includes
#include "vtkRenderingWebGPUModule.h" // for export macro
#include "vtkSceneGraphRenderPass.h"

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUForwardPass : public vtkSceneGraphRenderPass
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUForwardPass* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUForwardPass, vtkSceneGraphRenderPass);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  /**
   * Override to implement WebGPU specific render calls
   */
  void Traverse(vtkViewNode* vn, vtkRenderPass* parent = nullptr) override;

  ///@{
  /**
   * Increment actor counts.
   * \note This is considered internal API and should be called by VTK's scenegraph nodes during a
   * query pass to count different types of actors.
   */
  virtual void IncrementOpaqueActorCount();
  virtual void IncrementTranslucentActorCount();
  virtual void IncrementVolumeCount();
  vtkGetMacro(OpaqueActorCount, int);
  vtkGetMacro(TranslucentActorCount, int);
  vtkGetMacro(VolumeCount, int);
  ///@}

protected:
  vtkWebGPUForwardPass();
  ~vtkWebGPUForwardPass();

  // Helper members
  unsigned int OpaqueActorCount = 0;
  unsigned int TranslucentActorCount = 0;
  unsigned int VolumeCount = 0;

private:
  vtkWebGPUForwardPass(const vtkWebGPUForwardPass&) = delete;
  void operator=(const vtkWebGPUForwardPass) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUForwardPass_h
