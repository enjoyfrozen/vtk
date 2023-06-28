/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUPipeline.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// vtk includes
#include "vtkWebGPUPipeline.h"
#include "vtkObjectFactory.h"
#include "vtkWebGPUBindGroup.h"
#include "vtkWebGPUDevice.h"
#include "vtkWebGPUInstance.h"
#include "vtk_wgpu.h"

// STL includes
#include <algorithm>
#include <cassert>
#include <iterator>

VTK_ABI_NAMESPACE_BEGIN

//-------------------------------------------------------------------------------------------------
class vtkWebGPUPipeline::vtkInternal
{
public:
  WGPUPipelineLayoutDescriptor LayoutDescriptor = {};
  WGPUPipelineLayout Layout;
  std::vector<vtkWebGPUBindGroup*> BindGroups;
  std::vector<WGPUBindGroupLayout> BindGroupLayouts;
  vtkInternal();
};

//-------------------------------------------------------------------------------------------------
vtkWebGPUPipeline::vtkInternal::vtkInternal()
{
  this->LayoutDescriptor.nextInChain = nullptr;
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUPipeline::vtkWebGPUPipeline()
{
  this->Internal = new vtkInternal();
  this->SetLabel("VTKWebGPUPipeline");
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUPipeline::~vtkWebGPUPipeline()
{
  this->Destroy();
  this->Internal->BindGroups.clear();
  if (this->Internal)
  {
    delete this->Internal;
    this->Internal = nullptr;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUPipeline::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  // os << indent << " = " << this-> << endl;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUPipeline::Create()
{
  vtkWebGPUDevice* device = vtkWebGPUInstance::GetInstance()->GetDevice();
  if (!device)
  {
    vtkErrorMacro(<< "Cannot create a pipeline without a valid device");
    return;
  }

  if (!this->Internal->Layout)
  {
    std::string l = this->GetLabel();
    l += "_PipelineLayout";
    this->Internal->LayoutDescriptor.label = l.c_str();
    // Add the bindgroup layouts
    this->Internal->LayoutDescriptor.bindGroupLayoutCount = this->Internal->BindGroupLayouts.size();
    this->Internal->LayoutDescriptor.bindGroupLayouts = this->Internal->BindGroupLayouts.data();
    this->Internal->Layout =
      wgpuDeviceCreatePipelineLayout(device->GetHandle(), &this->Internal->LayoutDescriptor);
    this->Modified();
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUPipeline::Destroy()
{
  if (!this->Internal->Layout)
  {
    return;
  }
  wgpuPipelineLayoutRelease(this->Internal->Layout);
  this->Internal->Layout = nullptr;
}

//-------------------------------------------------------------------------------------------------
void* vtkWebGPUPipeline::GetPipelineLayout()
{
  return reinterpret_cast<void*>(&this->Internal->Layout);
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUPipeline::AddBindGroup(vtkWebGPUBindGroup* bg)
{
  if (!bg)
  {
    return;
  }
  this->Internal->BindGroups.push_back(bg);
  this->Internal->BindGroupLayouts.push_back(
    reinterpret_cast<WGPUBindGroupLayout>(bg->GetBindGroupLayout()));
  assert(this->Internal->BindGroups.size() == this->Internal->BindGroupLayouts.size());
}

//-------------------------------------------------------------------------------------------------
void* vtkWebGPUPipeline::GetBindGroupLayout(vtkTypeUInt32 idx)
{
  if (idx >= this->Internal->BindGroupLayouts.size())
  {
    vtkErrorMacro(<< "Requested bind group layout at out-of-bounds index");
    return nullptr;
  }

  return reinterpret_cast<void*>(this->Internal->BindGroupLayouts[idx]);
}

//-------------------------------------------------------------------------------------------------
vtkTypeUInt32 vtkWebGPUPipeline::GetBindGroupIndex(std::string label)
{
  auto bg = this->Internal->BindGroups;
  auto const it = std::find_if(
    bg.cbegin(), bg.cend(), [=](vtkWebGPUBindGroup* b) { return b->GetLabel() == label; });
  return std::distance(bg.cbegin(), it);
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
