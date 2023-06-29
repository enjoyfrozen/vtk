/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUEncoder.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUEncoder
 * @brief Abstract base class for webgpu render and compute encoders
 *
 */

#ifndef vtkWebGPUEncoder_h
#define vtkWebGPUEncoder_h

// vtk includes
#include "vtkWebGPUObject.h"
#include "vtkRenderingWebGPUModule.h" // for export macro

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations
class vtkWebGPUPipeline;

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUEncoder : public vtkWebGPUObject
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUEncoder* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUEncoder, vtkWebGPUObject);
  void PrintSelf(ostream &os, vtkIndent indent) override;
  ///@}

  ///@{
  /**
   * Set/Get the pipeline
   */
  virtual void SetPipeline(vtkWebGPUPipeline*);
  vtkGetObjectMacro(Pipeline, vtkWebGPUPipeline);
  ///@}

  ///@{
  /**
   * Begin/end the encode pass
   */
  virtual void Begin() = 0;
  virtual void End() = 0;
  ///@}

protected:
  vtkWebGPUEncoder();
  ~vtkWebGPUEncoder();

  // Helper members
  vtkWebGPUPipeline* Pipeline = nullptr;

private:
  vtkWebGPUEncoder(const vtkWebGPUEncoder&) = delete;
};

VTK_ABI_NAMESPACE_END
#endif //vtkWebGPUEncoder_h
