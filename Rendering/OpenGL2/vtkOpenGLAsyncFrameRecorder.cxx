// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkOpenGLAsyncFrameRecorder.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLFramebufferObject.h"
#include "vtkOpenGLRenderUtilities.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkOpenGLShaderCache.h"
#include "vtkOpenGLState.h"
#include "vtkOpenGLVertexArrayObject.h"
#include "vtkPixelBufferObject.h"
#include "vtkPointData.h"
#include "vtkShaderProgram.h"
#include "vtkTextureObject.h"
#include "vtkUnsignedCharArray.h"

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkOpenGLAsyncFrameRecorder);

namespace
{
using GLUtil = vtkOpenGLRenderUtilities;
}

//------------------------------------------------------------------------------
vtkOpenGLAsyncFrameRecorder::vtkOpenGLAsyncFrameRecorder() = default;

//------------------------------------------------------------------------------
vtkOpenGLAsyncFrameRecorder::~vtkOpenGLAsyncFrameRecorder() = default;

//------------------------------------------------------------------------------
void vtkOpenGLAsyncFrameRecorder::ReleaseGraphicsResources(vtkWindow* window)
{
  if (!this->Initialized)
  {
    return;
  }
  this->TearDown(vtkOpenGLRenderWindow::SafeDownCast(window));
  if (this->Program)
  {
    this->Program->ReleaseGraphicsResources(window);
    this->VAO->ReleaseGraphicsResources();
  }
}

//------------------------------------------------------------------------------
void vtkOpenGLAsyncFrameRecorder::Capture(
  vtkRenderWindow* window, int srcWidth, int srcHeight, int dstWidth, int dstHeight)
{
  auto oglRenWin = vtkOpenGLRenderWindow::SafeDownCast(window);
  if (!oglRenWin)
  {
    return;
  }
  if (srcWidth != this->LastSrcWidth || srcHeight != this->LastSrcHeight ||
    dstWidth != this->LastDstWidth || dstHeight != this->LastDstHeight)
  {
    this->TearDown(oglRenWin);
    this->Setup(oglRenWin, srcWidth, srcHeight, dstWidth, dstHeight);
  }
  if (!this->Program)
  {
    this->PrepShaderProgram(oglRenWin);
  }
  if (!this->FrameQueue.empty() && (this->FrameQueue.front() == this->FrameIndex)) // buffer overrun
  {
    return;
  }
  this->LastSrcWidth = srcWidth;
  this->LastSrcHeight = srcHeight;
  this->LastDstWidth = dstWidth;
  this->LastDstHeight = dstHeight;

  auto& srcFBO = this->ActualSizeFramebuffers[this->FrameIndex];
  auto& srcTex = this->ActualSizeTextures[this->FrameIndex];
  auto& dstFBO = this->ScaledFramebuffers[this->FrameIndex];
  auto& pbo = this->PBOs[this->FrameIndex];

  /// blit display framebuffer to our hi-res frame buffer with the actual image size.
  auto ostate = oglRenWin->GetState();
  ostate->PushDrawFramebufferBinding();
  srcFBO->Bind(GL_DRAW_FRAMEBUFFER);
  srcFBO->ActivateDrawBuffer(0);
  oglRenWin->BlitDisplayFramebuffer();
  ostate->PopDrawFramebufferBinding();

  oglRenWin->GetShaderCache()->ReadyShaderProgram(this->Program);
  if (this->Program && this->Program->GetCompiled())
  {
    /// scale actual size framebuffer into scaled framebuffer with texture interpolation on the GPU
    // 1. activate and bind the actual size texture that we just filled up.
    srcTex->Activate();
    glGenerateMipmap(GL_TEXTURE_2D);
    this->Program->SetUniformi("inputTexture", srcTex->GetTextureUnit());
    // 2. bind vbos
    this->VAO->Bind();
    // 3. draw the scaled image using the shader program.
    ostate->PushDrawFramebufferBinding();
    dstFBO->Bind(GL_DRAW_FRAMEBUFFER);
    dstFBO->ActivateDrawBuffer(0);
    ostate->vtkglViewport(0, 0, dstWidth, dstHeight);
    ostate->vtkglScissor(0, 0, dstWidth, dstHeight);
    ostate->vtkglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLUtil::DrawFullScreenQuad();
    ostate->PopDrawFramebufferBinding();
    // 4. schedule a read back from the scaled framebuffer
    ostate->PushReadFramebufferBinding();
    dstFBO->Bind(GL_READ_FRAMEBUFFER);
    dstFBO->ActivateReadBuffer(0);
    pbo->BindToPackedBuffer();
    // 5. data=nullptr enqueues a read-back into PBO.
    glReadPixels(0, 0, dstWidth, dstHeight, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    ostate->PopReadFramebufferBinding();
    // 6. fence which signals completion of the full render pipeline we just built above.
    this->Fences[this->FrameIndex] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    this->FrameQueue.emplace_back(this->FrameIndex);
    this->FrameIndex = ((this->FrameIndex + 1) % 4);
    // 7. cleanup vao, texture.
    this->VAO->Release();
    srcTex->Deactivate();
  }
}

//------------------------------------------------------------------------------
void vtkOpenGLAsyncFrameRecorder::RetrieveAvailableFrames()
{
  while (!this->FrameQueue.empty())
  {
    const auto& idx = this->FrameQueue.front();
    if (glClientWaitSync(this->Fences[idx], 0, 0) == GL_TIMEOUT_EXPIRED)
    {
      break;
    }
    glDeleteSync(this->Fences[idx]);
    this->PBOs[idx]->BindToPackedBuffer();
    vtkIdType size = this->LastDstWidth * this->LastDstHeight * 4;
    auto data = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, size, GL_MAP_READ_BIT);
    // wrap the pixels into a vtkUnsignedCharArray
    auto wrappedPixels = vtk::TakeSmartPointer(vtkUnsignedCharArray::New());
    wrappedPixels->SetNumberOfComponents(4);
    wrappedPixels->SetVoidArray(data, size, 1);
    // create a vtkImageData with those pixels.
    vtkNew<vtkImageData> image;
    image->SetDimensions(this->LastDstWidth, this->LastDstHeight, 1);
    image->GetPointData()->SetScalars(wrappedPixels);
    this->InvokeEvent(vtkAsyncFrameRecorder::FrameCapturedEvent, image);
    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    this->FrameQueue.erase(this->FrameQueue.begin());
  }
}

//------------------------------------------------------------------------------
std::size_t vtkOpenGLAsyncFrameRecorder::GetBacklogSize() const
{
  return this->FrameQueue.size();
}

//------------------------------------------------------------------------------
void vtkOpenGLAsyncFrameRecorder::PrepShaderProgram(vtkOpenGLRenderWindow* oglRenWin)
{
  // Prep full screen quad shader program which can render an input texture to the viewport
  std::string fragShader = GLUtil::GetFullScreenQuadFragmentShaderTemplate();
  vtkShaderProgram::Substitute(fragShader, "//VTK::FSQ::Decl", "uniform sampler2D inputTexture;");
  vtkShaderProgram::Substitute(
    fragShader, "//VTK::FSQ::Impl", "gl_FragData[0] = texture2D(inputTexture, texCoord);");
  std::string vertShader = GLUtil::GetFullScreenQuadVertexShader();
  auto shaderCache = oglRenWin->GetShaderCache();
  this->Program = shaderCache->ReadyShaderProgram(vertShader.c_str(), fragShader.c_str(), "");
  GLUtil::PrepFullScreenVAO(oglRenWin, this->VAO, this->Program);
}

//------------------------------------------------------------------------------
void vtkOpenGLAsyncFrameRecorder::Setup(
  vtkOpenGLRenderWindow* oglRenWin, int srcWidth, int srcHeight, int dstWidth, int dstHeight)
{
  this->FrameIndex = 0;
  this->FrameQueue.clear();

  for (int i = 0; i < 4; ++i)
  {
    // init actual-size texture
    this->ActualSizeTextures[i]->SetContext(oglRenWin);
    this->ActualSizeTextures[i]->SetMinificationFilter(vtkTextureObject::LinearMipmapLinear);
    this->ActualSizeTextures[i]->SetMagnificationFilter(vtkTextureObject::Linear);
    this->ActualSizeTextures[i]->Create2D(srcWidth, srcHeight, 4, VTK_UNSIGNED_CHAR, false);
    // init actual-size frambuffer
    this->ActualSizeFramebuffers[i]->SetContext(oglRenWin);
    oglRenWin->GetState()->PushFramebufferBindings();
    this->ActualSizeFramebuffers[i]->Bind();
    this->ActualSizeFramebuffers[i]->AddColorAttachment(0, this->ActualSizeTextures[i]);
    oglRenWin->GetState()->PopFramebufferBindings();
    // init scaled texture
    this->ScaledTextures[i]->SetContext(oglRenWin);
    this->ScaledTextures[i]->SetMinificationFilter(vtkTextureObject::Nearest);
    this->ScaledTextures[i]->SetMagnificationFilter(vtkTextureObject::Nearest);
    this->ScaledTextures[i]->Create2D(dstWidth, dstHeight, 4, VTK_UNSIGNED_CHAR, false);
    // init scaled frambuffer
    this->ScaledFramebuffers[i]->SetContext(oglRenWin);
    oglRenWin->GetState()->PushFramebufferBindings();
    this->ScaledFramebuffers[i]->Bind();
    this->ScaledFramebuffers[i]->AddColorAttachment(0, this->ScaledTextures[i]);
    oglRenWin->GetState()->PopFramebufferBindings();
    // init PBOs for read-back from ScaledTextures
    this->PBOs[i]->SetContext(oglRenWin);
    this->PBOs[i]->BindToPackedBuffer();
    glBufferData(GL_PIXEL_PACK_BUFFER, dstWidth * dstHeight * 4, nullptr, GL_STREAM_READ);
  }
  this->Initialized = true;
}

//------------------------------------------------------------------------------
void vtkOpenGLAsyncFrameRecorder::TearDown(vtkOpenGLRenderWindow* oglRenWin)
{
  if (this->Initialized)
  {
    for (int i = 0; i < 4; ++i)
    {
      this->ActualSizeFramebuffers[i]->ReleaseGraphicsResources(oglRenWin);
      this->ActualSizeTextures[i]->ReleaseGraphicsResources(oglRenWin);
      this->ScaledFramebuffers[i]->ReleaseGraphicsResources(oglRenWin);
      this->ScaledTextures[i]->ReleaseGraphicsResources(oglRenWin);
      this->PBOs[i]->SetContext(nullptr);
    }
    this->Initialized = false;
  }
}

//------------------------------------------------------------------------------
void vtkOpenGLAsyncFrameRecorder::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << "Initialized: " << this->Initialized << std::endl;
  os << "FrameIndex: " << this->FrameIndex << std::endl;
  os << "Backlog: " << this->FrameQueue.size() << std::endl;
}
VTK_ABI_NAMESPACE_END
