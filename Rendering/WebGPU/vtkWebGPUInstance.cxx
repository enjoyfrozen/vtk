/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUInstance.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkWebGPUInstance.h"

// VTK includes
#include "vtkObjectFactory.h"
#include "vtkWebGPUDevice.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUInstance);

//-------------------------------------------------------------------------------------------------
vtkCxxSetObjectMacro(vtkWebGPUInstance, Device, vtkWebGPUDevice);

//-------------------------------------------------------------------------------------------------
vtkWebGPUInstance::vtkWebGPUInstance() {}

//-------------------------------------------------------------------------------------------------
vtkWebGPUInstance::~vtkWebGPUInstance()
{
  if (this->Device)
  {
    this->Device->Delete();
    this->Device = nullptr;
  }
  if (this->Instance)
  {
    this->Destroy();
  }
}

//------------------------------------------------------------------------------------------------
void vtkWebGPUInstance::PrintSelf(ostream& os, vtkIndent indent) {}

//------------------------------------------------------------------------------------------------
void vtkWebGPUInstance::Create()
{
  if (this->Instance)
  {
    return;
  }
  WGPUInstanceDescriptor desc = {};
  desc.nextInChain = nullptr;
  this->Instance = wgpuCreateInstance(&desc);
  if (!this->Instance)
  {
    vtkErrorMacro(<< "Could not create webgpu instance");
  }

  // At this point, go ahead and request the adapter
  auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, const char* message, void * pUseData
  WGPURequestAdapterOptions options;
  options.powerPreference = WGPUPowerPreference_HighPerformance;
  this->Adapter = WGPUInstanceRequestAdapter(this->Instance, options);
}

//------------------------------------------------------------------------------------------------
void vtkWebGPUInstance::Destroy()
{
  if (!this->Instance)
  {
    return;
  }
  // wgpuInstanceRelease(this->Instance);
  this->Instance = nullptr;
}

//------------------------------------------------------------------------------------------------
bool vtkWebGPUInstance::IsValid()
{
  return (this->Instance != nullptr);
}

VTK_ABI_NAMESPACE_END
