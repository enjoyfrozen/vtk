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
#include "vtkSmartPointer.h"

// STL includes
#include <sstream>
#include <vector>
#include <mutex>

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
std::mutex WebGPUInstanceLock; // XXX(c++17): use a `shared_mutex`
vtkSmartPointer<vtkWebGPUInstance> vtkWebGPUInstanceGlobalInstance;
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

  // Here, we should wait for the adapter to be available and ready. However, the previous function
  // `wgpuInstanceRequestAdapter` only returns when its callback has been called, i.e. this->Adapter
  // is valid at this point unless something went wrong.
  if (!this->Adapter)
  {
    vtkErrorMacro(<< "Could not request adapter");
    return;
  }

  // Proceed with creating the device
  if (!this->Device)
  {
    this->Device = vtkWebGPUDevice::New();
  }
  this->Device->Create(this->Adapter);

  // Again, the previous function only returns when its callback has been called, i.e. the device
  // handle should be available at this point unless something went wrong.
  if (!this->Device->GetHandle())
  {
    vtkErrorMacro(<< "Could not request device");
    return;
  }
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
    this->Device->Destroy();
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

  if (this->Device)
  {
    strm << this->Device->ReportCapabilities() << endl;
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

//------------------------------------------------------------------------------------------------
WGPUCommandEncoder vtkWebGPUInstance::GetCommandEncoder()
{
  if (!this->IsValid())
  {
    return nullptr;
  }
  if (!this->CommandEncoder)
  {
    WGPUCommandEncoderDescriptor desc = {};
    desc.nextInChain = nullptr;
    desc.label = "VTKWebGPU Command Encoder";
    this->CommandEncoder = wgpuDeviceCreateCommandEncoder(this->Device->GetHandle(), &desc);
  }
  return this->CommandEncoder;
}

//------------------------------------------------------------------------------------------------
vtkWebGPUInstance* vtkWebGPUInstance::GetInstance()
{
  // Check if we have an instance already.
  {
    std::unique_lock<std::mutex> lock(WebGPUInstanceLock);
    // std::shared_lock lock(InstanceLock); // XXX(c++17)
    (void)lock;

    if (vtkWebGPUInstanceGlobalInstance)
    {
      return vtkWebGPUInstanceGlobalInstance;
    }
  }

  {
    std::unique_lock<std::mutex> lock(WebGPUInstanceLock);
    (void)lock;

    // Another thread may have raced us here; if it already exists, use it.
    if (vtkWebGPUInstanceGlobalInstance)
    {
      return vtkWebGPUInstanceGlobalInstance;
    }

    vtkWebGPUInstanceGlobalInstance = vtkSmartPointer<vtkWebGPUInstance>::New();
  }

  // return the instance
  return vtkWebGPUInstanceGlobalInstance;
}

//------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
