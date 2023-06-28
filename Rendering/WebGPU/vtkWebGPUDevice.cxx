/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUDevice.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkWebGPUDevice.h"
#include "vtkObjectFactory.h"
#include "vtkWebGPUCommandEncoder.h"
#include "vtkWebGPUPipeline.h"

// STL includes
#include <sstream>
#include <vector>

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUDevice);

//-------------------------------------------------------------------------------------------------
vtkWebGPUDevice::vtkWebGPUDevice()
{
  this->SetLabel("VTK WebGPU Device");
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUDevice::~vtkWebGPUDevice()
{
  this->Destroy();

  this->Pipelines.clear();

  delete[] this->Capabilities;
  this->Capabilities = nullptr;

  delete[] this->Label;
  this->Label = nullptr;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  // os << indent << " = " << this-> << endl;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUDevice::Create(WGPUAdapter a)
{
  if (this->Device)
  {
    return;
  }

  if (a == nullptr)
  {
    vtkErrorMacro(<< "Cannot request webgpu device without an adapter");
    return;
  }

  WGPUDeviceDescriptor deviceOpts = {};
  deviceOpts.nextInChain = nullptr;
  deviceOpts.label = this->Label;
  // No specific feature needed for now
  deviceOpts.requiredFeaturesCount = 0;
  // No specific limit needed for now
  deviceOpts.requiredLimits = nullptr;
  deviceOpts.defaultQueue.nextInChain = nullptr;
  deviceOpts.defaultQueue.label = "VTK WebGPU Default Queue";
  wgpuAdapterRequestDevice(a, &deviceOpts, &vtkWebGPUDevice::OnDeviceRequested, this);
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUDevice::Destroy()
{
  if (this->CommandEncoder)
  {
    this->CommandEncoder->Delete();
    this->CommandEncoder = nullptr;
  }
  if (this->Device)
  {
    wgpuDeviceRelease(this->Device);
    this->Device = nullptr;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUDevice::SetHandle(WGPUDevice d)
{
  vtkDebugMacro(<< " setting Device to " << d);
  if (this->Device != d)
  {
    WGPUDevice tempD = this->Device;
    this->Device = d;
    if (this->Device != nullptr)
    {
      this->InvokeEvent(DeviceRequestedEvent);
      // Register the callbacks
      wgpuDeviceSetUncapturedErrorCallback(this->Device, &vtkWebGPUDevice::OnDeviceError, this);
      wgpuDeviceSetDeviceLostCallback(this->Device, &vtkWebGPUDevice::OnDeviceLost, this);
      WGPUQueue workQueue = wgpuDeviceGetQueue(this->Device);
#ifdef VTK_WEBGPU_USE_DAWN
      wgpuQueueOnSubmittedWorkDone(workQueue, 0, &vtkWebGPUDevice::OnSubmittedWorkDoneEvent, this);
#elif VTK_WEBGPU_USE_WGPU
      wgpuQueueOnSubmittedWorkDone(workQueue, &vtkWebGPUDevice::OnSubmittedWorkDoneEvent, this);
#endif
    }
    if (tempD != nullptr)
    {
      wgpuDeviceRelease(tempD);
      tempD = nullptr;
    }
    this->Modified();
  }
}

//-------------------------------------------------------------------------------------------------
WGPUDevice vtkWebGPUDevice::GetHandle()
{
  return this->Device;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUDevice::OnDeviceRequested(
  WGPURequestDeviceStatus status, WGPUDevice device, const char* message, void* self)
{
  if (status != WGPURequestDeviceStatus_Success)
  {
    const char* device_request_failed_reason = "";
    switch (status)
    {
      case WGPURequestDeviceStatus_Error:
        device_request_failed_reason = "WGPURequestDeviceStatus_Error";
        break;
      default:
      case WGPURequestDeviceStatus_Unknown:
        device_request_failed_reason = "WGPURequestDeviceStatus_Unknown";
        break;
    }
    vtkErrorWithObjectMacro(reinterpret_cast<vtkWebGPUDevice*>(self),
      << "RequestDevice failed: " << device_request_failed_reason << "\n\t" << message);
  }
  else
  {
    vtkWebGPUDevice* d = reinterpret_cast<vtkWebGPUDevice*>(self);
    if (d)
    {
      d->SetHandle(device);
    }
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUDevice::OnDeviceError(WGPUErrorType e, const char* message, void* self)
{
  vtkWebGPUDevice* d = reinterpret_cast<vtkWebGPUDevice*>(self);
  if (!d)
  {
    return;
  }
  const char* error_type = "";
  switch (e)
  {
    case WGPUErrorType_Validation:
      error_type = "Validation";
      break;
    case WGPUErrorType_OutOfMemory:
      error_type = "Out of Memory";
      break;
    case WGPUErrorType_DeviceLost:
      error_type = "Device lost";
      break;
    case WGPUErrorType_Internal:
      error_type = "Internal";
      break;
    default:
    case WGPUErrorType_Unknown:
      error_type = "Unknown";
      break;
  }
  vtkErrorWithObjectMacro(d, << error_type << "\n\t" << message);
  d->InvokeEvent(DeviceErrorEvent);
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUDevice::OnDeviceLost(WGPUDeviceLostReason e, const char* message, void* self)
{
  vtkWebGPUDevice* d = reinterpret_cast<vtkWebGPUDevice*>(self);
  if (!d)
  {
    return;
  }
  const char* error_type = "";
  switch (e)
  {
    case WGPUDeviceLostReason_Destroyed:
      error_type = "Destroyed";
      break;
    default:
    case WGPUDeviceLostReason_Undefined:
      error_type = "Undefined";
      break;
  }
  vtkErrorWithObjectMacro(d, << error_type << "\n\t" << message);
  d->InvokeEvent(DeviceLostEvent);
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUDevice::OnSubmittedWorkDoneEvent(WGPUQueueWorkDoneStatus status, void* self)
{
  vtkWebGPUDevice* d = reinterpret_cast<vtkWebGPUDevice*>(self);
  if (!d)
  {
    return;
  }

  if (status != WGPUQueueWorkDoneStatus_Success)
  {
    const char* status_type = "";
    switch (status)
    {
      case WGPUQueueWorkDoneStatus_Error:
        status_type = "Error";
        break;
      case WGPUQueueWorkDoneStatus_DeviceLost:
        status_type = "Device lost";
        break;
      default:
      case WGPUQueueWorkDoneStatus_Unknown:
        status_type = "Unknown";
        break;
    }
    vtkErrorWithObjectMacro(d, << status_type);
  }
  d->InvokeEvent(QueueWorkDoneEvent);
}

//-------------------------------------------------------------------------------------------------
const char* vtkWebGPUDevice::ReportCapabilities()
{
  std::ostringstream strm;
  if (this->Device)
  {
    std::vector<WGPUFeatureName> features;
    size_t featureCount = wgpuDeviceEnumerateFeatures(this->Device, nullptr);
    features.resize(featureCount);
    wgpuDeviceEnumerateFeatures(this->Device, features.data());

    strm << "Device features:" << endl;
    for (auto f : features)
    {
      strm << " - " << f << endl;
    }

    WGPUSupportedLimits limits = {};
    limits.nextInChain = nullptr;
    bool success = wgpuDeviceGetLimits(this->Device, &limits);
    if (success)
    {
      strm << "Device limits:" << endl;
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

//-------------------------------------------------------------------------------------------------
vtkWebGPUCommandEncoder* vtkWebGPUDevice::GetCommandEncoder()
{
  if (this->GetHandle() && !this->CommandEncoder)
  {
    this->CommandEncoder = vtkWebGPUCommandEncoder::New();
    this->CommandEncoder->Create();
  }
  return this->CommandEncoder;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUDevice::CreatePipeline(std::string pHash, vtkWebGPUPipeline* pipeline)
{
  if (!pipeline || pHash.empty())
  {
    return;
  }
  pipeline->SetLabel(pHash.c_str());
  pipeline->Create();
  this->Pipelines[pHash] = pipeline;
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUPipeline* vtkWebGPUDevice::GetPipeline(std::string pHash)
{
  return this->Pipelines[pHash];
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
