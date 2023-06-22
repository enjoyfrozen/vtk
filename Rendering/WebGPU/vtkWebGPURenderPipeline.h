/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPURenderPipeline.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPURenderPipeline
 * @brief Concrete implementation class for the vtkWebGPUPipeline that represents the WebGPU render
 * pipeline
 *
 */

#ifndef vtkWebGPURenderPipeline_h
#define vtkWebGPURenderPipeline_h

// vtk includes
#include "vtkRenderingWebGPUModule.h" // for export macro
#include "vtkWebGPUPipeline.h"

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations
struct WGPURenderPipelineDescriptor;

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPURenderPipeline : public vtkWebGPUPipeline
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPURenderPipeline* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPURenderPipeline, vtkWebGPUPipeline);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  /**
   * Create the pipeline
   *
   * Make sure that the descriptor is set up before creating pipeline
   */
  void Create() override;

  /**
   * Release the pipeline
   */
  void Destroy() override;

  /**
   * Get access to the pipeline handle
   */
  void* GetHandle() override;

  /**
   * Get access to the pipeline descriptor
   */
  WGPURenderPipelineDescriptor* GetDescriptor();

protected:
  vtkWebGPURenderPipeline();
  ~vtkWebGPURenderPipeline();

  // Helper members

private:
  class vtkInternal;
  vtkInternal* Internal;

  vtkWebGPURenderPipeline(const vtkWebGPURenderPipeline&) = delete;
  void operator=(const vtkWebGPURenderPipeline) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPURenderPipeline_h
