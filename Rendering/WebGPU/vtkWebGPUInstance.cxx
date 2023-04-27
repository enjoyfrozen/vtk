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

#include "vtkObjectFactory.h"
#include "vtkWebGPUDevice.h"

// STL includes
#include <sstream>
#include <vector>

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
  this->Destroy();

  delete[] this->Capabilities;
  this->Capabilities = nullptr;
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
  WGPURequestAdapterOptions adapterOpts = {};
  adapterOpts.nextInChain = nullptr;
  adapterOpts.powerPreference = WGPUPowerPreference_HighPerformance;
  wgpuInstanceRequestAdapter(
    this->Instance, &adapterOpts, &vtkWebGPUInstance::OnAdapterRequested, this);
}

//------------------------------------------------------------------------------------------------
void vtkWebGPUInstance::Destroy()
{
  if (!this->Instance)
  {
    return;
  }

  if (this->Device)
  {
    this->Device->Delete();
    this->Device = nullptr;
  }

  if (this->Adapter)
  {
    wgpuAdapterRelease(this->Adapter);
    this->Adapter = nullptr;
  }

  wgpuInstanceRelease(this->Instance);
  this->Instance = nullptr;
}

//------------------------------------------------------------------------------------------------
bool vtkWebGPUInstance::IsValid()
{
  return (this->Instance != nullptr);
}

//------------------------------------------------------------------------------------------------
void vtkWebGPUInstance::SetAdapter(WGPUAdapter a)
{
  vtkDebugMacro(<< " setting Adapter to " << a);
  if (this->Adapter != a)
  {
    WGPUAdapter tempA = this->Adapter;
    this->Adapter = a;
    if (this->Adapter != nullptr)
    {
      this->InvokeEvent(AdapterRequestedEvent);
    }
    if (tempA != nullptr)
    {
      wgpuAdapterRelease(tempA);
      tempA = nullptr;
    }
    this->Modified();
  }
}

//------------------------------------------------------------------------------------------------
WGPUAdapter vtkWebGPUInstance::GetAdapter() const
{
  return this->Adapter;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUInstance::OnAdapterRequested(
  WGPURequestAdapterStatus status, WGPUAdapter adapter, const char* message, void* self)
{
  if (status != WGPURequestAdapterStatus_Success)
  {
    const char* adapter_request_failed_reason = "";
    switch (status)
    {
      case WGPURequestAdapterStatus_Unavailable:
        adapter_request_failed_reason = "WGPURequestAdapterStatus_Unavailable";
        break;
      case WGPURequestAdapterStatus_Error:
        adapter_request_failed_reason = "WGPURequestAdapterStatus_Error";
        break;
      default:
      case WGPURequestAdapterStatus_Unknown:
        adapter_request_failed_reason = "WGPURequestAdapterStatus_Unknown";
        break;
    }
    vtkErrorWithObjectMacro(reinterpret_cast<vtkWebGPUInstance*>(self),
      << "RequestAdapter failed: " << adapter_request_failed_reason << "\n\t" << message);
  }
  else
  {
    vtkWebGPUInstance* i = reinterpret_cast<vtkWebGPUInstance*>(self);
    if (i)
    {
      i->SetAdapter(adapter);
      i->InvokeEvent(vtkWebGPUInstance::AdapterRequestedEvent);
    }
  }
}

//-------------------------------------------------------------------------------------------------
const char* vtkWebGPUInstance::ReportCapabilities()
{
  std::ostringstream strm;
  if (this->Adapter)
  {
    // Document the adapter properties first
    WGPUAdapterProperties properties = {};
    properties.nextInChain = nullptr;
    wgpuAdapterGetProperties(this->Adapter, &properties);
    strm << "Adapter properties:" << endl;
    strm << " - vendorID: " << properties.vendorID << endl;
    strm << " - deviceID: " << properties.deviceID << endl;
    strm << " - name: " << properties.name << endl;
    if (properties.driverDescription)
    {
      strm << " - driverDescription: " << properties.driverDescription << endl;
    }
    strm << " - adapterType: " << properties.adapterType << endl;
    strm << " - backendType: " << properties.backendType << endl;

    std::vector<WGPUFeatureName> features;
    size_t featureCount = wgpuAdapterEnumerateFeatures(this->Adapter, nullptr);
    features.resize(featureCount);
    wgpuAdapterEnumerateFeatures(this->Adapter, features.data());

    strm << "Adapter features:" << endl;
    for (auto f : features)
    {
      strm << " - " << f << endl;
    }

    WGPUSupportedLimits limits = {};
    limits.nextInChain = nullptr;
    bool success = wgpuAdapterGetLimits(this->Adapter, &limits);
    if (success)
    {
      strm << "Adapter limits:" << endl;
      strm << " - maxTextureDimension1D: " << limits.limits.maxTextureDimension1D << endl;
      strm << " - maxTextureDimension2D: " << limits.limits.maxTextureDimension2D << endl;
      strm << " - maxTextureDimension3D: " << limits.limits.maxTextureDimension3D << endl;
      strm << " - maxTextureArrayLayers: " << limits.limits.maxTextureArrayLayers << endl;
      strm << " - maxBindGroups: " << limits.limits.maxBindGroups << endl;
      strm << " - maxDynamicUniformBuffersPerPipelineLayout: "
           << limits.limits.maxDynamicUniformBuffersPerPipelineLayout << endl;
      strm << " - maxDynamicStorageBuffersPerPipelineLayout: "
           << limits.limits.maxDynamicStorageBuffersPerPipelineLayout << endl;
      strm << " - maxSampledTexturesPerShaderStage: "
           << limits.limits.maxSampledTexturesPerShaderStage << endl;
      strm << " - maxSamplersPerShaderStage: " << limits.limits.maxSamplersPerShaderStage << endl;
      strm << " - maxStorageBuffersPerShaderStage: "
           << limits.limits.maxStorageBuffersPerShaderStage << endl;
      strm << " - maxStorageTexturesPerShaderStage: "
           << limits.limits.maxStorageTexturesPerShaderStage << endl;
      strm << " - maxUniformBuffersPerShaderStage: "
           << limits.limits.maxUniformBuffersPerShaderStage << endl;
      strm << " - maxUniformBufferBindingSize: " << limits.limits.maxUniformBufferBindingSize
           << endl;
      strm << " - maxStorageBufferBindingSize: " << limits.limits.maxStorageBufferBindingSize
           << endl;
      strm << " - minUniformBufferOffsetAlignment: "
           << limits.limits.minUniformBufferOffsetAlignment << endl;
      strm << " - minStorageBufferOffsetAlignment: "
           << limits.limits.minStorageBufferOffsetAlignment << endl;
      strm << " - maxVertexBuffers: " << limits.limits.maxVertexBuffers << endl;
      strm << " - maxVertexAttributes: " << limits.limits.maxVertexAttributes << endl;
      strm << " - maxVertexBufferArrayStride: " << limits.limits.maxVertexBufferArrayStride << endl;
      strm << " - maxInterStageShaderComponents: " << limits.limits.maxInterStageShaderComponents
           << endl;
      strm << " - maxComputeWorkgroupStorageSize: " << limits.limits.maxComputeWorkgroupStorageSize
           << endl;
      strm << " - maxComputeInvocationsPerWorkgroup: "
           << limits.limits.maxComputeInvocationsPerWorkgroup << endl;
      strm << " - maxComputeWorkgroupSizeX: " << limits.limits.maxComputeWorkgroupSizeX << endl;
      strm << " - maxComputeWorkgroupSizeY: " << limits.limits.maxComputeWorkgroupSizeY << endl;
      strm << " - maxComputeWorkgroupSizeZ: " << limits.limits.maxComputeWorkgroupSizeZ << endl;
      strm << " - maxComputeWorkgroupsPerDimension: "
           << limits.limits.maxComputeWorkgroupsPerDimension << endl;
    }
  }
  delete[] this->Capabilities;

  size_t len = strm.str().length() + 1;
  this->Capabilities = new char[len];
  strncpy(this->Capabilities, strm.str().c_str(), len);

  return this->Capabilities;
}

//------------------------------------------------------------------------------
void vtkWebGPUInstance::SetPowerPreference(int power)
{
  int clampedPower = (power < HIGH_POWER ? HIGH_POWER : (power > CPU ? CPU : power));
  if (this->PowerPreference != clampedPower)
  {
    this->PowerPreference = clampedPower;
    this->Destroy();
    this->Modified();
  }
}

VTK_ABI_NAMESPACE_END
