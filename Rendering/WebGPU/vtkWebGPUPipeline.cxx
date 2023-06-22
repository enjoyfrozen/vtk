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
#include "vtkWebGPUDevice.h"
#include "vtkWebGPUInstance.h"
#include "vtk_wgpu.h"

VTK_ABI_NAMESPACE_BEGIN

//-------------------------------------------------------------------------------------------------
class vtkWebGPUPipeline::vtkInternal
{
public:
  WGPUPipelineLayoutDescriptor LayoutDescriptor = {};
  WGPUPipelineLayout Layout;
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
VTK_ABI_NAMESPACE_END
