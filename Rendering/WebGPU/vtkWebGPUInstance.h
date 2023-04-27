/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUInstance.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUInstance
 * @brief Class to manage the wgpu instance in vtk
 */

#ifndef vtkWebGPUInstance_h
#define vtkWebGPUInstance_h

// VTK includes
#include "vtkObject.h"
#include "vtkRenderingWebGPUModule.h" // for export macro
#include "vtk_wgpu.h"                 // for webgpu

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations
class vtkWebGPUDevice;

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUInstance : public vtkObject
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUInstance* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUInstance, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  ///@{
  /**
   * Create/Destroy an instance
   */
  void Create();
  void Destroy();
  ///@}

  /**
   * Get whether the instance is valid
   */
  bool IsValid();

  ///@{
  /**
   * Set/Get the device
   */
  virtual void SetDevice(vtkWebGPUDevice*);
  vtkGetObjectMacro(Device, vtkWebGPUDevice);
  ///@}

protected:
  vtkWebGPUInstance();
  ~vtkWebGPUInstance();

  // Helper members
  WGPUInstance Instance = nullptr;
  WGPUAdapter Adapter = nullptr;

  vtkWebGPUDevice* Device = nullptr;

private:
  vtkWebGPUInstance(const vtkWebGPUInstance&) = delete;
  void operator=(const vtkWebGPUInstance) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUInstance_h
