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
#include "vtkCommand.h" // for custom events
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

  ///@{
  /**
   * Set/Get the adapter
   */
  virtual void SetAdapter(WGPUAdapter a);
  WGPUAdapter GetAdapter() const;
  ///@}

  enum vtkCustomEvents
  {
    AdapterRequestedEvent = vtkCommand::UserEvent + 100
  };

  enum vtkPowerPreferences
  {
    HIGH_POWER = 0, // default (Discrete GPU)
    LOW_POWER = 1,  // (Integrated GPU)
    CPU = 2
  };

  ///@{
  /**
   * Set/Get the power preference i.e. the device (discrete/integrated GPU, CPU) that webGPU uses
   */
  virtual void SetPowerPreference(int power);
  vtkGetMacro(PowerPreference, int);
  virtual void SetPowerPreferenceToHighPower() { this->SetPowerPreference(HIGH_POWER); }
  virtual void SetPowerPreferenceToLowPower() { this->SetPowerPreference(LOW_POWER); }
  virtual void SetPowerPreferenceToCPU() { this->SetPowerPreference(CPU); }
  ///@}

  /**
   * Report capabilities of the hardware
   */
  const char* ReportCapabilities();

  /**
   * Get the command encoder
   */
  WGPUCommandEncoder GetCommandEncoder();

  /**
   * Get a singleton instance
   */
  static vtkWebGPUInstance* GetInstance();

protected:
  vtkWebGPUInstance();
  ~vtkWebGPUInstance();

  // Helper members
  WGPUInstance Instance = nullptr;
  WGPUAdapter Adapter = nullptr;

  vtkWebGPUDevice* Device = nullptr;
  WGPUCommandEncoder CommandEncoder = nullptr;

  int PowerPreference = HIGH_POWER;
  char* Capabilities = nullptr;

  static void OnAdapterRequested(
    WGPURequestAdapterStatus status, WGPUAdapter adapter, const char* message, void* self);

private:
  vtkWebGPUInstance(const vtkWebGPUInstance&) = delete;
  void operator=(const vtkWebGPUInstance) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUInstance_h
