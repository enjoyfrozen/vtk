/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUBuffer.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// vtk includes
#include "vtkWebGPUBuffer.h"
#include "vtkDataArray.h"
#include "vtkObjectFactory.h"
#include "vtkWebGPUDevice.h"
#include "vtkWebGPUInstance.h"
#include "vtk_wgpu.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUBuffer);

//-------------------------------------------------------------------------------------------------
class vtkWebGPUBuffer::vtkInternal
{
public:
  WGPUBuffer Buffer;
  WGPUBufferDescriptor Descriptor = {};
  WGPUBindGroupEntry BindGroupEntry = {};
  WGPUBindGroupLayoutEntry BindGroupLayoutEntry = {};
  vtkInternal();
};

//-------------------------------------------------------------------------------------------------
vtkWebGPUBuffer::vtkInternal::vtkInternal()
{
  this->Descriptor.nextInChain = nullptr;
  this->BindGroupEntry.nextInChain = nullptr;
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUBuffer::vtkWebGPUBuffer()
{
  this->Internal = new vtkInternal();
  this->SetLabel("VTKWebGPUBuffer");
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUBuffer::~vtkWebGPUBuffer()
{
  if (this->Internal)
  {
    delete this->Internal;
    this->Internal = nullptr;
  }
}

//-------------------------------------------------------------------------------------------------
void* vtkWebGPUBuffer::GetHandle()
{
  return reinterpret_cast<void*>(this->Internal->Buffer);
}

//-------------------------------------------------------------------------------------------------
WGPUBufferDescriptor& vtkWebGPUBuffer::GetDescriptor()
{
  return this->Internal->Descriptor;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUBuffer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << " SizeInBytes = " << this->SizeInBytes << endl;
  os << indent << " MappedAtCreation = " << (this->MappedAtCreation ? "True" : "False") << endl;
  os << indent << " UsageFlags = " << this->UsageFlags << endl;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUBuffer::Create()
{
  vtkWebGPUInstance* inst = vtkWebGPUInstance::GetInstance();
  if (!inst->IsValid())
  {
    return;
  }
  this->Internal->Descriptor.usage = static_cast<WGPUBufferUsageFlags>(this->GetUsageFlags());
  this->Internal->Descriptor.size = this->GetSizeInBytes();
  this->Internal->Descriptor.mappedAtCreation = this->GetMappedAtCreation();
  this->Internal->Descriptor.label = this->GetLabel();
  this->Internal->Buffer =
    wgpuDeviceCreateBuffer(inst->GetDevice()->GetHandle(), &this->Internal->Descriptor);
  this->BindGroupTime.Modified();
  this->Modified();
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUBuffer::Destroy()
{
  if (!this->Internal->Buffer)
  {
    return;
  }
  // wgpuBufferRelease(this->Internal->Buffer);
  wgpuBufferDestroy(this->Internal->Buffer);
  this->Internal->Buffer = nullptr;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUBuffer::Write(vtkDataArray* a)
{
  this->Write(a->GetVoidPointer(0), a->GetDataSize() * a->GetDataTypeSize());
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUBuffer::Write(void* data, vtkTypeUInt64 size)
{
  if (!this->Internal->Buffer || !data)
  {
    return;
  }
  vtkWebGPUInstance* inst = vtkWebGPUInstance::GetInstance();
  if (!inst)
  {
    return;
  }
  WGPUQueue q = wgpuDeviceGetQueue(inst->GetDevice()->GetHandle());
  wgpuQueueWriteBuffer(q, this->Internal->Buffer, 0, data, size);
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUBuffer::CreateAndWrite(void* data, vtkTypeUInt64 size, vtkTypeUInt64 usage)
{
  if (this->Internal->Buffer)
  {
    if (this->GetSizeInBytes() != size || this->GetUsageFlags() != usage)
    {
      this->Destroy();
    }
  }
  this->SetSizeInBytes(size);
  this->SetUsageFlags(usage);
  this->SetMappedAtCreation(true);
  this->Create();
  this->Write(data, size);
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUBuffer::BufferMapState vtkWebGPUBuffer::GetMappedState()
{
  if (this->Internal->Buffer)
  {
    vtkWebGPUInstance* inst = vtkWebGPUInstance::GetInstance();
    if (!inst)
    {
      return vtkWebGPUBuffer::Unmapped;
    }
    return static_cast<vtkWebGPUBuffer::BufferMapState>(
      wgpuBufferGetMapState(this->Internal->Buffer));
  }
  return vtkWebGPUBuffer::Unmapped;
}

//-------------------------------------------------------------------------------------------------
void* vtkWebGPUBuffer::GetBindGroupEntry()
{
  if (!this->GetHandle())
  {
    this->Create();
  }
  this->Internal->BindGroupEntry.buffer = this->Internal->Buffer;
  return reinterpret_cast<void*>(&this->Internal->BindGroupEntry);
}

//-------------------------------------------------------------------------------------------------
void* vtkWebGPUBuffer::GetBindGroupLayoutEntry()
{
  return reinterpret_cast<void*>(&this->Internal->BindGroupLayoutEntry);
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
