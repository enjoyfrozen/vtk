/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUWindowNode.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// vtk includes
#include "vtkWebGPUWindowNode.h"

#include "vtkHardwareWindow.h"
#include "vtkObjectFactory.h"
#include "vtkRenderPassCollection.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderingUIConfigure.h"
#include "vtkWebGPUForwardPass.h"
#include "vtkWebGPUInstance.h"
#include "vtkWebGPURenderWindow.h"
#include "vtkWebGPUViewNodeFactory.h"

#include "vtk_wgpu.h"

#if defined(__WIN32__)
#include "vtkWin32HardwareWindow.h"
#elif defined(__APPLE__)
#include "vtkCocoaHardwareWindow.h"
#elif defined(VTK_USE_X)
#include "vtkXlibHardwareWindow.h"
#endif

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkCxxSetObjectMacro(vtkWebGPUWindowNode, Instance, vtkWebGPUInstance);
vtkCxxSetObjectMacro(vtkWebGPUWindowNode, Interactor, vtkRenderWindowInteractor);
vtkCxxSetObjectMacro(vtkWebGPUWindowNode, HardwareWindow, vtkHardwareWindow);

//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUWindowNode);

//-------------------------------------------------------------------------------------------------
class vtkWebGPUWindowNode::vtkInternal
{
public:
  WGPUSurface Surface;
};

//-------------------------------------------------------------------------------------------------
vtkWebGPUWindowNode::vtkWebGPUWindowNode()
{
  this->Internal = new vtkInternal();

  // Create the view node factory
  vtkWebGPUViewNodeFactory* fac = vtkWebGPUViewNodeFactory::New();
  this->SetMyFactory(fac);
  fac->Delete();

  this->RenderPasses = vtkRenderPassCollection::New();
  // Create the forward rendering pass
  vtkWebGPUForwardPass* fwPass = vtkWebGPUForwardPass::New();
  this->RenderPasses->AddItem(fwPass);
  fwPass->Delete();
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUWindowNode::~vtkWebGPUWindowNode()
{
  if (this->Instance)
  {
    this->Instance->Delete();
    this->Instance = nullptr;
  }
  this->RenderPasses->Delete();
  this->RenderPasses = nullptr;

  delete this->Internal;
  this->Internal = nullptr;
}

//------------------------------------------------------------------------------------------------
void vtkWebGPUWindowNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "WebGPUInstance:";
  if (this->Instance)
  {
    os << endl;
    this->Instance->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << " (null)" << endl;
  }
  os << indent << "Interactor:";
  if (this->Interactor)
  {
    os << endl;
    this->Interactor->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << " (null)" << endl;
  }
  os << indent << "HardwareWindow:";
  if (this->HardwareWindow)
  {
    os << endl;
    this->HardwareWindow->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << " (null)" << endl;
  }
  os << indent << "RenderPasses:";
  if (this->RenderPasses)
  {
    os << endl;
    this->RenderPasses->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << " (null)" << endl;
  }
}

//------------------------------------------------------------------------------------------------
void vtkWebGPUWindowNode::SetRenderable(vtkObject* obj)
{
  if (this->Renderable == obj)
  {
    return;
  }

  vtkWebGPURenderWindow* renWin = vtkWebGPURenderWindow::SafeDownCast(obj);
  if (!renWin)
  {
    vtkErrorMacro(<< "Renderable must be of type vtkWebGPURenderWindow or its subclass."
                  << "Instead, found " << obj->GetClassName());
    return;
  }
  this->Superclass::SetRenderable(obj);
}

//------------------------------------------------------------------------------------------------
void vtkWebGPUWindowNode::AddRenderPass(vtkSceneGraphRenderPass* pass)
{
  this->RenderPasses->AddItem(pass);
}

//------------------------------------------------------------------------------------------------
void vtkWebGPUWindowNode::TraverseAllPasses()
{
  // Traverse the scenegraph via the registered render passes
  if (this->RenderPasses)
  {
    vtkCollectionSimpleIterator iter;
    this->RenderPasses->InitTraversal(iter);
    vtkRenderPass* rp = nullptr;
    while ((rp = this->RenderPasses->GetNextRenderPass(iter)))
    {
      vtkSceneGraphRenderPass* scPass = vtkSceneGraphRenderPass::SafeDownCast(rp);
      if (scPass)
      {
        scPass->Traverse(this, nullptr);
      }
    }
  }
}

//------------------------------------------------------------------------------
vtkTypeBool vtkWebGPUWindowNode::IsInitialized()
{
  return (this->GetInstance() && this->GetInstance()->IsValid());
}

//------------------------------------------------------------------------------------------------
void vtkWebGPUWindowNode::Build(bool prepass)
{
  if (prepass)
  {
    // Initialize webgpu
    if (!this->IsInitialized())
    {
      if (!this->Instance)
      {
        this->Instance = vtkWebGPUInstance::New();
      }
      this->Instance->Create();
      if (!this->Instance->IsValid())
      {
        vtkErrorMacro(<< "Could not create a valid webgpu instance");
        return;
      }
      std::cout << this->Instance->ReportCapabilities() << std::endl;
    }

    vtkWindow* w = vtkWindow::SafeDownCast(this->Renderable);
    // Create a surface and hardware window, if not available
    if (!this->Internal->Surface)
    {
      auto rwsize = w->GetSize();
      this->NextSize[0] = rwsize[0] ? rwsize[0] : 300;
      this->NextSize[1] = rwsize[1] ? rwsize[1] : 300;
      w->SetSize(this->NextSize[0], this->NextSize[1]);

      // instantiate a hardware window, if needed.
      if (!this->HardwareWindow)
      {
        this->HardwareWindow = vtkHardwareWindow::New();
        this->HardwareWindow->SetSize(this->NextSize[0], this->NextSize[1]);
        this->HardwareWindow->Create();
      }
      // this->HardwareWindow->SetSize(this->NextSize[0], this->NextSize[1]);

      // Set properties on interactor if set
      if (this->Interactor)
      {
        this->Interactor->SetHardwareWindow(this->HardwareWindow);
      }
      WGPUSurfaceDescriptor surfaceDesc;
#if defined(__EMSCRIPTEN__)
      // render into canvas elememnt
      WGPUSurfaceDescriptorFromCanvasHTMLSelector htmlSurfDesc = {};
      htmlSurfDesc.chain = (const WGPUChainedStruct){
        .sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector,
      };
      htmlSurfDesc.selector = "#canvas";
      surfaceDesc.nextInChain = (const WGPUChainedStruct*)&(htmlSurfDesc);
#elif defined(_WIN32)
      vtkWin32HardwareWindow* win32Window =
        vtkWin32HardwareWindow::SafeDownCast(this->HardwareWindow);
      WGPUSurfaceDescriptorFromWindowsHWND winSurfDesc = {};
      winSurfDesc.chain = (const WGPUChainedStruct){
        .sType = WGPUSType_SurfaceDescriptorFromWindowsHWND,
      };
      winSurfDesc.hwnd = win32Window->GetWindowId();
      winSurfDesc.hinstance = win32Window->GetApplicationInstance();
      surfaceDesc.nextInChain = (const WGPUChainedStruct*)&(winSurfDesc);
#elif defined(__APPLE__)
      vtkCocoaHardwareWindow* cocoaWindow =
        vtkCocoaHardwareWindow::SafeDownCast(this->HardwareWindow);
      WGPUSurfaceDescriptorFromMetalLayer cocoaSurfDesc = {};
      cocoaSurfDesc.chain = (const WGPUChainedStruct){
        .sType = WGPUSType_SurfaceDescriptorFromMetalLayer,
      };
      cocoaSurfDesc.layer = cocoaWindow->GetViewLayer();
      surfaceDesc.nextInChain = (const WGPUChainedStruct*)&(cocoaSurfDesc);
#elif defined(VTK_USE_X)
      vtkXlibHardwareWindow* xWindow = vtkXlibHardwareWindow::SafeDownCast(this->HardwareWindow);
      WGPUSurfaceDescriptorFromXlibWindow xSurfDesc = {};
      xSurfDesc.chain = (const WGPUChainedStruct){
        .sType = WGPUSType_SurfaceDescriptorFromXlibWindow,
      };
      xSurfDesc.display = xWindow->GetDisplayId();
      xSurfDesc.window = xWindow->GetWindowId();
      surfaceDesc.nextInChain = (const WGPUChainedStruct*)&(xSurfDesc);
// #elif defined(VTK_USE_WAYLAND)
#elif defined(VTK_USE_SDL2)
      vtkSDL2HardwareWindow* sdl2Window = vtkSDL2HardwareWindow::SafeDownCast(this->HardwareWindow);
      SDL_SysWMinfo wmInfo;
      SDL_VERSION(&wmInfo.version)
      if (!SDL_GetWindowWMInfo(sdl2Window->GetWindowId(), &wmInfo))
      {
        vtkErrorMacro(<< "Invalid SDL2 hardware window");
      }
#else
#error "No compositing system available."
#endif
    }

    this->Superclass::Build(prepass);
  }
}

//------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
