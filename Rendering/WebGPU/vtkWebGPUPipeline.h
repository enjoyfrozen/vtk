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

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations

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
