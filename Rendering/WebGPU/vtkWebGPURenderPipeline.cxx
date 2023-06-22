/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPURenderPipeline.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// vtk includes
#include "vtkWebGPURenderPipeline.h"
#include "vtkObjectFactory.h"
#include "vtkWebGPUDevice.h"
#include "vtkWebGPUInstance.h"
#include "vtk_wgpu.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPURenderPipeline);

//-------------------------------------------------------------------------------------------------
class vtkWebGPURenderPipeline::vtkInternal
{
public:
  WGPURenderPipelineDescriptor Descriptor = {};
  WGPURenderPipeline Pipeline;
  vtkInternal();
};

//-------------------------------------------------------------------------------------------------
vtkWebGPURenderPipeline::vtkInternal::vtkInternal()
{
  this->Descriptor.nextInChain = nullptr;
}

//-------------------------------------------------------------------------------------------------
vtkWebGPURenderPipeline::vtkWebGPURenderPipeline()
{
  this->Internal = new vtkInternal();
  this->SetLabel("vtkWebGPURenderPipeline");
}

//-------------------------------------------------------------------------------------------------
vtkWebGPURenderPipeline::~vtkWebGPURenderPipeline()
{
  this->Destroy();
  if (this->Internal)
  {
    delete this->Internal;
    this->Internal = nullptr;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURenderPipeline::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  // os << indent << " = " << this-> << endl;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURenderPipeline::Create()
{
  if (this->Internal->Pipeline)
  {
    return;
  }

  vtkWebGPUDevice* device = vtkWebGPUInstance::GetInstance()->GetDevice();
  if (!device)
  {
    vtkErrorMacro(<< "Cannot create a pipeline without a valid device");
    return;
  }

  // Create the pipeline layout
  this->Superclass::Create();

  WGPUPipelineLayout* layout = reinterpret_cast<WGPUPipelineLayout*>(this->GetPipelineLayout());
  if (!layout)
  {
    vtkErrorMacro(<< "Could not create pipeline layout");
    return;
  }

  // Do not set anything on the descriptor.
  // It should have been configured at this point.
  this->Internal->Pipeline =
    wgpuDeviceCreateRenderPipeline(device->GetHandle(), &this->Internal->Descriptor);
  this->Modified();
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURenderPipeline::Destroy()
{
  if (this->Internal->Pipeline)
  {
    wgpuRenderPipelineRelease(this->Internal->Pipeline);
    this->Internal->Pipeline = nullptr;
  }
  // Invoke the superclass destroy to destroy the pipeline layout
  this->Superclass::Destroy();
}

//-------------------------------------------------------------------------------------------------
void* vtkWebGPURenderPipeline::GetHandle()
{
  return reinterpret_cast<void*>(&this->Internal->Pipeline);
}

//-------------------------------------------------------------------------------------------------
WGPURenderPipelineDescriptor* vtkWebGPURenderPipeline::GetDescriptor()
{
  return &this->Internal->Descriptor;
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
