/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWin32OpenGL2RenderWindow.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkWin32OpenGLDXRenderWindow.h"

#include "vtkObjectFactory.h"
#include "vtkOpenGLFramebufferObject.h"
#include "vtkTextureObject.h"
#include "vtk_glew.h"

#include <dxgi.h>

vtkStandardNewMacro(vtkWin32OpenGLDXRenderWindow);

//------------------------------------------------------------------------------
void vtkWin32OpenGLDXRenderWindow::Initialize(void)
{
  this->Superclass::Initialize();

  // Require NV_DX_interop OpenGL extension
  if (!WGLEW_NV_DX_interop)
  {
    vtkErrorMacro("OpenGL extension WGLEW_NV_DX_interop unsupported.");
    return;
  }

  // Create the DXGI adapter.
  ComPtr<IDXGIFactory1> dxgiFactory;
  CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&dxgiFactory));
  ComPtr<IDXGIAdapter1> DXGIAdapter;
  for (UINT adapterIndex = 0;; adapterIndex++)
  {
    // Return when there are no more adapters to enumerate
    HRESULT hr = dxgiFactory->EnumAdapters1(adapterIndex, DXGIAdapter.GetAddressOf());
    if (hr == DXGI_ERROR_NOT_FOUND)
    {
      vtkWarningMacro("No DXGI adapter found");
      break;
    }

    DXGI_ADAPTER_DESC1 adapterDesc;
    DXGIAdapter->GetDesc1(&adapterDesc);
    // Choose the adapter matching the internal adapter id or
    // return the first adapter that is available if AdapterId is not set.
    if ((!this->AdapterId.HighPart && !this->AdapterId.LowPart) ||
      memcmp(&adapterDesc.AdapterLuid, &this->AdapterId, sizeof(this->AdapterId)) == 0)
    {
      break;
    }
  }

  // Use unknown driver type with DXGI adapters
  D3D_DRIVER_TYPE driverType =
    DXGIAdapter == nullptr ? D3D_DRIVER_TYPE_HARDWARE : D3D_DRIVER_TYPE_UNKNOWN;

  // Create the D3D API device object and a corresponding context.
  D3D11CreateDevice(DXGIAdapter.Get(), driverType, 0, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
    &this->MinFeatureLevel, 1, D3D11_SDK_VERSION, this->Device.GetAddressOf(), nullptr,
    this->D3DDeviceContext.GetAddressOf());

  if (!this->Device)
  {
    vtkErrorMacro("D3D11CreateDevice failed in Initialize().");
  }

  // Acquire a handle to the D3D device for use in OpenGL
  this->DeviceHandle = wglDXOpenDeviceNV(this->Device.Get());

  // Create D3D Texture2D
  D3D11_TEXTURE2D_DESC textureDesc;
  ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
  textureDesc.Width = this->Size[0];
  textureDesc.Height = this->Size[1];
  textureDesc.MipLevels = 1;
  textureDesc.ArraySize = 1;
  textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  textureDesc.SampleDesc.Count = 1; // TODO: handle this->MultiSamples
  textureDesc.Usage = D3D11_USAGE_DEFAULT;
  textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
  textureDesc.CPUAccessFlags = 0;
  textureDesc.MiscFlags = 0;
  this->Device->CreateTexture2D(&textureDesc, nullptr, &this->D3DSharedTexture);

  if (!this->D3DSharedTexture)
  {
    vtkErrorMacro("Failed to create D3D shared texture.");
  }
}

//------------------------------------------------------------------------------
void vtkWin32OpenGLDXRenderWindow::Lock(void)
{
  if (!this->DeviceHandle || !this->GLSharedTextureHandle)
  {
    vtkWarningMacro("Failed to lock shared texture.");
    return;
  }

  wglDXLockObjectsNV(this->DeviceHandle, 1, &this->GLSharedTextureHandle);
}

//------------------------------------------------------------------------------
void vtkWin32OpenGLDXRenderWindow::Unlock(void)
{
  if (!this->DeviceHandle || !this->GLSharedTextureHandle)
  {
    vtkWarningMacro("Failed to unlock shared texture.");
    return;
  }

  wglDXUnlockObjectsNV(this->DeviceHandle, 1, &this->GLSharedTextureHandle);
}

//------------------------------------------------------------------------------
void vtkWin32OpenGLDXRenderWindow::RegisterSharedTexture(unsigned int textureHandle)
{
  if (this->TextureId == textureHandle)
  {
    return;
  }

  if (!this->DeviceHandle || !this->D3DSharedTexture)
  {
    vtkWarningMacro("Failed to register shared texture. Initializing window.");
    this->Initialize();
  }

  this->TextureId = textureHandle;

  this->GLSharedTextureHandle = wglDXRegisterObjectNV(this->DeviceHandle, // D3D device handle
    this->D3DSharedTexture.Get(),                                         // D3D texture
    this->TextureId,                                                      // OpenGL texture id
    GL_TEXTURE_2D, // TODO: handle this->MultiSamples
    WGL_ACCESS_READ_WRITE_NV);

  if (!this->GLSharedTextureHandle)
  {
    vtkErrorMacro("wglDXRegisterObjectNV failed in RegisterSharedTexture().");
  }
}

//------------------------------------------------------------------------------
void vtkWin32OpenGLDXRenderWindow::UnregisterSharedTexture()
{
  if (!this->DeviceHandle || !this->GLSharedTextureHandle)
  {
    return;
  }

  wglDXUnregisterObjectNV(this->DeviceHandle, this->GLSharedTextureHandle);
  this->TextureId = 0;
  this->GLSharedTextureHandle = 0;
}

//------------------------------------------------------------------------------
void vtkWin32OpenGLDXRenderWindow::SetSize(int width, int height)
{
  if ((this->Size[0] != width) || (this->Size[1] != height))
  {
    this->vtkOpenGLRenderWindow::SetSize(width, height);

    if (!this->DeviceHandle || !this->D3DSharedTexture)
    {
      return;
    }

    D3D11_TEXTURE2D_DESC textureDesc;
    this->D3DSharedTexture->GetDesc(&textureDesc);

    unsigned int tId = this->TextureId;
    this->UnregisterSharedTexture();

    textureDesc.Width = this->Size[0];
    textureDesc.Height = this->Size[1];
    this->Device->CreateTexture2D(
      &textureDesc, nullptr, this->D3DSharedTexture.ReleaseAndGetAddressOf());

    this->RegisterSharedTexture(tId);
  }
}

//------------------------------------------------------------------------------
void vtkWin32OpenGLDXRenderWindow::BlitToTexture(ID3D11Texture2D* target)
{
  if (!this->D3DDeviceContext || !target || !D3DSharedTexture)
  {
    return;
  }

  D3D11_TEXTURE2D_DESC td;
  this->D3DSharedTexture->GetDesc(&td);

  this->D3DDeviceContext->CopySubresourceRegion(target, // destination
    0,                                                  // destination subresource id
    0, 0, 0,                                            // destination origin x,y,z
    this->D3DSharedTexture.Get(),                       // source
    0,                                                  // source subresource id
    nullptr);                                           // source clip box (nullptr == full extent)
}
