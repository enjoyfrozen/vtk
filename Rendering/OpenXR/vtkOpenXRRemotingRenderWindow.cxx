/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOpenXRManagerD3DGraphics.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkOpenXRRemotingRenderWindow.h"

#include "vtkObjectFactory.h"
#include "vtkOpenGLFramebufferObject.h"
#include "vtkOpenXRManager.h"
#include "vtkOpenXRManagerD3DGraphics.h"
#include "vtkTextureObject.h"
#include "vtkWin32OpenGLDXRenderWindow.h"

vtkStandardNewMacro(vtkOpenXRRemotingRenderWindow);

//------------------------------------------------------------------------------
vtkOpenXRRemotingRenderWindow::vtkOpenXRRemotingRenderWindow()
{
  // Flip the texture before presenting to D3D which using a different convention
  // for texture orientation.
  this->FramebufferFlipY = true;

  // Use an OpenGL-DX render window to allow streaming VTK rendering in a D3D texture.
  this->HelperWindow = vtkWin32OpenGLDXRenderWindow::New();

  // Use a D3D rendering backend in OpenXR
  vtkNew<vtkOpenXRManagerD3DGraphics> D3Dgraphics;
  vtkOpenXRManager::GetInstance()->SetGraphicsStrategy(D3Dgraphics);

  vtkOpenXRManager::GetInstance()->RemotingOn();
}

//------------------------------------------------------------------------------
void vtkOpenXRRemotingRenderWindow::SetRemotingIPAddress(const char* host)
{
  vtkOpenXRManager::GetInstance()->SetRemotingIPAddress(host);
}

//------------------------------------------------------------------------------
void vtkOpenXRRemotingRenderWindow::Initialize()
{
  if (this->Initialized)
  {
    return;
  }

  // Prevent failure on D3D texture creation when size is null
  this->HelperWindow->SetSize(this->Size[0], this->Size[1]);

  this->Superclass::Initialize();

  vtkOpenGLRenderWindow::CreateFramebuffers(this->Size[0], this->Size[1]);

  vtkWin32OpenGLDXRenderWindow* helperWindow =
    vtkWin32OpenGLDXRenderWindow::SafeDownCast(this->HelperWindow);
  // Register this window display framebuffer with the helper window D3D texture.
  // We use the display buffer to benefit from FramebufferFlipY.
  helperWindow->RegisterSharedTexture(
    this->GetDisplayFramebuffer()->GetColorAttachmentAsTextureObject(0)->GetHandle());

  // Resize shared texture
  this->HelperWindow->SetSize(this->Size[0], this->Size[1]);
}

//------------------------------------------------------------------------------
void vtkOpenXRRemotingRenderWindow::StereoUpdate()
{
  this->Superclass::StereoUpdate();

  // Lock the shared texture for rendering
  vtkWin32OpenGLDXRenderWindow::SafeDownCast(this->HelperWindow)->Lock();
}

//------------------------------------------------------------------------------
void vtkOpenXRRemotingRenderWindow::StereoMidpoint()
{
  // Blit to DisplayFamebuffer with FramebufferFlipY enabled
  this->Frame();

  // RenderOneEye
  this->Superclass::StereoMidpoint();
}

//------------------------------------------------------------------------------
void vtkOpenXRRemotingRenderWindow::StereoRenderComplete()
{
  // Blit to DisplayFamebuffer with FramebufferFlipY enabled
  this->Frame();

  // RenderOneEye
  this->Superclass::StereoRenderComplete();

  // Unlock the shared texture
  vtkWin32OpenGLDXRenderWindow::SafeDownCast(this->HelperWindow)->Unlock();
}

//------------------------------------------------------------------------------
void vtkOpenXRRemotingRenderWindow::RenderOneEye(const uint32_t eye)
{
  vtkOpenXRManager* xrManager = vtkOpenXRManager::GetInstance();

  ID3D11Texture2D* colorTexture = nullptr;
  ID3D11Texture2D* depthTexture = nullptr;
  if (!xrManager->PrepareRendering(eye, &colorTexture, &depthTexture))
  {
    return;
  }

  this->RenderModels();

  // D3D11 Rendering
  vtkWin32OpenGLDXRenderWindow* helperWindow =
    vtkWin32OpenGLDXRenderWindow::SafeDownCast(this->HelperWindow);
  helperWindow->Unlock();
  helperWindow->BlitToTexture(colorTexture);
  helperWindow->Lock();

  // Release this swapchain image
  xrManager->ReleaseSwapchainImage(eye);
}
