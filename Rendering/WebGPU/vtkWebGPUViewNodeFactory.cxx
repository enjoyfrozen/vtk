/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUViewNodeFactory.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// vtk includes
#include "vtkWebGPUViewNodeFactory.h"
#include "vtkObjectFactory.h"

// #include "vtkWebGPUActorNode.h"
// #include "vtkWebGPUPolyDataMapperNode.h"
#include "vtkWebGPURendererNode.h"

VTK_ABI_NAMESPACE_BEGIN

namespace vtkwgpu
{
vtkViewNode* ren_maker()
{
  vtkWebGPURendererNode* vn = vtkWebGPURendererNode::New();
  return vn;
}

// vtkViewNode* act_maker()
// {
//   vtkWebGPUActorNode* vn = vtkWebGPUActorNode::New();
//   return vn;
// }

// vtkViewNode* pd_maker()
// {
//   vtkWebGPUPolyDataMapperNode* vn = vtkWebGPUPolyDataMapperNode::New();
//   return vn;
// }

}

//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUViewNodeFactory);

//-------------------------------------------------------------------------------------------------
vtkWebGPUViewNodeFactory::vtkWebGPUViewNodeFactory()
{
  // see vtkRenderWindow::GetRenderLibrary
  this->RegisterOverride("vtkRenderer", vtkwgpu::ren_maker);
  // this->RegisterOverride("vtkActor", vtkwgpu::act_maker);
  // this->RegisterOverride("vtkPolyDataMapper", vtkwgpu::pd_maker);
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUViewNodeFactory::~vtkWebGPUViewNodeFactory() = default;

//-------------------------------------------------------------------------------------------------
void vtkWebGPUViewNodeFactory::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
VTK_ABI_NAMESPACE_END
