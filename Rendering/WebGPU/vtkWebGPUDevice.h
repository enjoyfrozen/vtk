/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUDevice.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUDevice
 * @brief WebGPU device manager
 *
 */

#ifndef vtkWebGPUDevice_h
#define vtkWebGPUDevice_h

// VTK includes
#include "vtkObject.h"
#include "vtkRenderingWebGPUModule.h" // for export macro

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUDevice : public vtkObject
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUDevice* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUDevice, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

protected:
  vtkWebGPUDevice();
  ~vtkWebGPUDevice();

  // Helper members

private:
  vtkWebGPUDevice(const vtkWebGPUDevice&) = delete;
  void operator=(const vtkWebGPUDevice) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUDevice_h
