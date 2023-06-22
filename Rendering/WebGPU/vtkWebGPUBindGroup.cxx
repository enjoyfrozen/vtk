/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUBindGroup.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkWebGPUBindGroup.h"
#include "vtkObjectFactory.h"
#include "vtkWebGPUBindableObject.h"
#include "vtkWebGPUDevice.h"
#include "vtk_wgpu.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
class vtkWebGPUBindGroup::vtkInternal
{
public:
  vtkInternal();
  WGPUBindGroup BindGroup = nullptr;
  WGPUBindGroupDescriptor Descriptor = {};
  WGPUBindGroupLayout BindGroupLayout = nullptr;
  WGPUBindGroupLayoutDescriptor LayoutDescriptor = {};
  std::vector<WGPUBindGroupLayoutEntry> LayoutEntries;
  std::vector<WGPUBindGroupEntry> Entries;
};

//-------------------------------------------------------------------------------------------------
vtkWebGPUBindGroup::vtkInternal::vtkInternal()
{
  this->Descriptor.nextInChain = nullptr;
  this->LayoutDescriptor.nextInChain = nullptr;
  this->LayoutDescriptor.label = "VTKWebGPUBindGroupLayout";
}

//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUBindGroup);

//-------------------------------------------------------------------------------------------------
vtkWebGPUBindGroup::vtkWebGPUBindGroup()
{
  this->Internal = new vtkInternal();
  this->SetLabel("VTKWebGPUBindGroup");
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUBindGroup::~vtkWebGPUBindGroup()
{
  this->Destroy();
  if (this->Internal)
  {
    delete this->Internal;
    this->Internal = nullptr;
  }
}

//-------------------------------------------------------------------------------------------------
void* vtkWebGPUBindGroup::GetHandle()
{
  return reinterpret_cast<void*>(this->Internal->BindGroup);
}

//-------------------------------------------------------------------------------------------------
void* vtkWebGPUBindGroup::GetBindGroupLayout()
{
  return reinterpret_cast<void*>(this->Internal->BindGroupLayout);
}

//-------------------------------------------------------------------------------------------------
WGPUBindGroupDescriptor& vtkWebGPUBindGroup::GetDescriptor()
{
  return this->Internal->Descriptor;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUBindGroup::Create(vtkWebGPUDevice* d)
{
  if (!d)
  {
    vtkErrorMacro(<< "Cannot create a bind group without a device");
    return;
  }
  if (!this->GetNeedToRecreateBindGroup())
  {
    return;
  }

  this->Internal->Entries.clear();
  const std::size_t numEntries = this->Bindables.size();
  this->Internal->Entries.resize(numEntries);
  for (std::size_t i = 0; i < numEntries; ++i)
  {
    WGPUBindGroupEntry* e =
      reinterpret_cast<WGPUBindGroupEntry*>(this->Bindables[i]->GetBindGroupEntry());
    e->binding = i;
    this->Internal->Entries[i] = *e;
  }
  this->Internal->Descriptor.entryCount = numEntries;
  this->Internal->Descriptor.entries = this->Internal->Entries.data();
  this->CreateBindGroupLayout(d);
  this->Internal->Descriptor.layout = this->Internal->BindGroupLayout;
  // All set, create the bind group
  this->Internal->BindGroup =
    wgpuDeviceCreateBindGroup(d->GetHandle(), &this->Internal->Descriptor);
  this->BindGroupTime.Modified();
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUBindGroup::Destroy()
{
  if (this->Internal->BindGroup)
  {
    wgpuBindGroupRelease(this->Internal->BindGroup);
    this->Internal->BindGroup = nullptr;
  }
  this->DestroyBindGroupLayout();
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUBindGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "BindGroupTime = " << this->GetBindGroupTime() << endl;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUBindGroup::SetBindables(std::vector<vtkWebGPUBindableObject*> b)
{
  if (this->Bindables.size() == b.size())
  {
    auto bindablesIter = this->Bindables.cbegin();
    auto bIter = b.cbegin();
    while (bindablesIter != this->Bindables.cend() && bIter != b.cend())
    {
      // Early exit if they don't match
      if (*bindablesIter != *bIter)
      {
        break;
      }
      ++bindablesIter;
      ++bIter;
    }
    if (bindablesIter == this->Bindables.cend())
    {
      // All the elements matched.
      return;
    }
  }
  // Copy
  this->Bindables.clear();
  this->Bindables = b;
  this->Modified();
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUBindGroup::CreateBindGroupLayout(vtkWebGPUDevice* d)
{
  if (this->Bindables.empty())
  {
    this->Internal->LayoutEntries.clear();
    return;
  }
  if (!d)
  {
    return;
  }
  this->Internal->LayoutEntries.clear();
  const std::size_t numEntries = this->Bindables.size();
  this->Internal->LayoutEntries.resize(numEntries);
  for (std::size_t i = 0; i < numEntries; ++i)
  {
    WGPUBindGroupLayoutEntry* e =
      reinterpret_cast<WGPUBindGroupLayoutEntry*>(this->Bindables[i]->GetBindGroupLayoutEntry());
    e->binding = i;
    this->Internal->LayoutEntries[i] = *e;
  }
  this->Internal->LayoutDescriptor.entryCount = numEntries;
  this->Internal->LayoutDescriptor.entries = this->Internal->LayoutEntries.data();
  this->Internal->BindGroupLayout =
    wgpuDeviceCreateBindGroupLayout(d->GetHandle(), &this->Internal->LayoutDescriptor);
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUBindGroup::DestroyBindGroupLayout()
{
  if (this->Internal->BindGroupLayout)
  {
    wgpuBindGroupLayoutRelease(this->Internal->BindGroupLayout);
    this->Internal->BindGroupLayout = nullptr;
  }
}

//-------------------------------------------------------------------------------------------------
vtkMTimeType vtkWebGPUBindGroup::GetBindGroupTime() const
{
  return this->BindGroupTime.GetMTime();
}

//-------------------------------------------------------------------------------------------------
vtkTypeBool vtkWebGPUBindGroup::GetNeedToRecreateBindGroup()
{
  vtkMTimeType mtime = this->GetMTime();
  for (vtkWebGPUBindableObject* i : this->Bindables)
  {
    vtkMTimeType imtime = i->GetBindGroupTime();
    mtime = imtime > mtime ? imtime : mtime;
  }
  if (mtime < this->GetBindGroupTime())
  {
    return false;
  }
  return true;
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
