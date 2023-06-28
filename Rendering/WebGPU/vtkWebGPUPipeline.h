/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUPipeline.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUPipeline
 * @brief Abstract class the represents the WebGPU pipeline
 */

#ifndef vtkWebGPUPipeline_h
#define vtkWebGPUPipeline_h

// vtk includes
#include "vtkRenderingWebGPUModule.h" // for export macro
#include "vtkWebGPUObject.h"

// STL include
#include <vector> // For layouts

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations
class vtkWebGPUBindGroup;

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUPipeline : public vtkWebGPUObject
{
public:
  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUPipeline, vtkWebGPUObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  /**
   * Create the pipeline
   * This base class creates the pipeline layout
   */
  virtual void Create();

  /**
   * Release the pipeline
   * This base class releases the pipeline layout
   */
  virtual void Destroy();

  /**
   * Get access to the pipeline layout
   */
  void* GetPipelineLayout();

  /**
   * Add a bind group to the internal vector of bindgroups
   * The index of the group is used as the group index when adding the bind group to the render pass
   * encoder.
   */
  virtual void AddBindGroup(vtkWebGPUBindGroup*);

  /**
   * Get the bind group layout from group index
   */
  virtual void* GetBindGroupLayout(vtkTypeUInt32 idx);

  /**
   * Get the group index for a bind group (via label)
   */
  virtual vtkTypeUInt32 GetBindGroupIndex(std::string label);

protected:
  vtkWebGPUPipeline();
  ~vtkWebGPUPipeline();

  // Helper members

private:
  class vtkInternal;
  vtkInternal* Internal;

  vtkWebGPUPipeline(const vtkWebGPUPipeline&) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUPipeline_h
