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
#include "vtkCommand.h" // for custom events
#include "vtkObject.h"
#include "vtkRenderingWebGPUModule.h" // for export macro
#include "vtk_wgpu.h"                 // for webgpu

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

  ///@{
  /**
   * Create/Destroy an instance of the webgpu context with an adapter
   * Ideally, this should be called only once by vtkWebGPUInstance::Create.
   */
  void Create(WGPUAdapter a);
  void Destroy();
  ///@}

  ///@{
  /**
   * Set/Get a handle to the webgpu device
   */
  void SetHandle(WGPUDevice d);
  WGPUDevice GetHandle();
  ///@}

  ///@{
  /**
   * Set/Get the device label
   */
  vtkSetStringMacro(Label);
  vtkGetStringMacro(Label);
  ///@}

  enum vtkCustomEvents
  {
    DeviceRequestedEvent = vtkCommand::UserEvent + 101,
    DeviceErrorEvent,
    DeviceLostEvent,
    QueueWorkDoneEvent
  };

  /**
   * Report capabilities of the webgpu context
   */
  const char* ReportCapabilities();

protected:
  vtkWebGPUDevice();
  ~vtkWebGPUDevice();

  // Helper members
  WGPUDevice Device = nullptr;
  char* Label = nullptr;
  char* Capabilities = nullptr;

  // Callbacks
  static void OnDeviceRequested(
    WGPURequestDeviceStatus status, WGPUDevice device, const char* message, void* self);
  static void OnDeviceError(WGPUErrorType e, const char* message, void* self);
  static void OnDeviceLost(WGPUDeviceLostReason e, const char* message, void* self);
  static void OnSubmittedWorkDoneEvent(WGPUQueueWorkDoneStatus status, void* self);

private:
  vtkWebGPUDevice(const vtkWebGPUDevice&) = delete;
  void operator=(const vtkWebGPUDevice) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUDevice_h
