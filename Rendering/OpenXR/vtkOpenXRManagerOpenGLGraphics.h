/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOpenXRManagerOpenGLGraphics.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkOpenXRManagerOpenGLGraphics

 * @brief   OpenXR manager OpenGL graphics implementation
 *
 * Allow vtkOpenXRManager to use an OpenGL rendering backend.
 */

#ifndef vtkOpenXRManagerOpenGLGraphics_h
#define vtkOpenXRManagerOpenGLGraphics_h

#include "vtkOpenXRManagerGraphics.h"
#include "vtkRenderingOpenXRModule.h" // For export macro

#include <vector>

class VTKRENDERINGOPENXR_EXPORT vtkOpenXRManagerOpenGLGraphics : public vtkOpenXRManagerGraphics
{
public:
  static vtkOpenXRManagerOpenGLGraphics* New();
  vtkTypeMacro(vtkOpenXRManagerOpenGLGraphics, vtkOpenXRManagerGraphics);

  virtual void SetNumberOfSwapchains(const uint32_t viewCount)
  {
    this->ColorSwapchains.resize(viewCount);
    this->DepthSwapchains.resize(viewCount);
  };

  void GetColorSwapchainImage(const uint32_t scIndex, const uint32_t imgIndex, void* texture)
  {
    *(GLuint*)texture = this->ColorSwapchains[scIndex].Images[imgIndex].image;
  };

  void GetDepthSwapchainImage(const uint32_t scIndex, const uint32_t imgIndex, void* texture)
  {
    *(GLuint*)texture = this->DepthSwapchains[scIndex].Images[imgIndex].image;
  };

  void EnumerateColorSwapchainImages(XrSwapchain swapchain, const uint32_t scIndex)
  {
    this->EnumerateSwapchainImages(swapchain, this->ColorSwapchains[scIndex]);
  };

  void EnumerateDepthSwapchainImages(XrSwapchain swapchain, const uint32_t scIndex)
  {
    this->EnumerateSwapchainImages(swapchain, this->DepthSwapchains[scIndex]);
  };

  const std::vector<int64_t>& GetSupportedColorFormats()
  {
    const static std::vector<int64_t> supportedColorFormats = { GL_RGBA32F, GL_RGBA16F, GL_RGBA16,
      GL_SRGB8_ALPHA8_EXT };
    return supportedColorFormats;
  }

  const std::vector<int64_t>& GetSupportedDepthFormats()
  {
    const static std::vector<int64_t> supportedDepthFormats = { GL_DEPTH_COMPONENT16,
      GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT32F };
    return supportedDepthFormats;
  }

  bool CreateGraphicsBinding(vtkOpenGLRenderWindow* helperWindow);

  bool CheckGraphicsRequirements(
    XrInstance instance, XrSystemId id, xr::ExtensionDispatchTable extensions);

  const char* GetBackendExtensionName() { return XR_KHR_OPENGL_ENABLE_EXTENSION_NAME; };

protected:
  vtkOpenXRManagerOpenGLGraphics() = default;
  ~vtkOpenXRManagerOpenGLGraphics() = default;

  /**
   * OpenGL structure to store swapchain images.
   */
  struct SwapchainImagesOpenGL
  {
    std::vector<XrSwapchainImageOpenGLKHR> Images;
  };

  /**
   * Acquire OpenGL swapchain images an store them in \p swapchainImages.
   */
  void EnumerateSwapchainImages(XrSwapchain swapchain, SwapchainImagesOpenGL& swapchainImages);

  // OpenGL swapchains
  std::vector<SwapchainImagesOpenGL> ColorSwapchains;
  std::vector<SwapchainImagesOpenGL> DepthSwapchains;

private:
  vtkOpenXRManagerOpenGLGraphics(const vtkOpenXRManagerOpenGLGraphics&) = delete;
  void operator=(const vtkOpenXRManagerOpenGLGraphics&) = delete;
};

#endif
