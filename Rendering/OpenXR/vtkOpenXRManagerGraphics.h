/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOpenXRManagerGraphics.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkOpenXRManagerGraphics

 * @brief   OpenXR manager graphics implementation
 *
 * Abstract class that defines the rendering backend used by vtkOpenXRManager.
 * It handles creation and management of the OpenXR rendering ressources,
 * based on the rendering backend implemented by subclasses.
 *
 * @sa
 * vtkOpenXRManagerOpenGLGraphics vtkOpenXRManagerD3DGraphics
 */

#ifndef vtkOpenXRManagerGraphics_h
#define vtkOpenXRManagerGraphics_h

#include "vtkObject.h"
#include "vtkRenderingOpenXRModule.h" // For export macro

#include "vtkOpenXR.h"
#include <vector>

class vtkOpenGLRenderWindow;

class VTKRENDERINGOPENXR_EXPORT vtkOpenXRManagerGraphics : public vtkObject
{
public:
  vtkTypeMacro(vtkOpenXRManagerGraphics, vtkObject);

  //@{
  /**
   * Resize the internal vectors storing the color and depth swapchains.
   */
  virtual void SetNumberOfSwapchains(const uint32_t viewCount) = 0;
  //@}

  //@{
  /**
   * Fill \p texture with the swapchain image for the specified eye. The image index should be
   * obtained beforehand using xrAcquireSwapchainImage.
   */
  virtual void GetColorSwapchainImage(
    const uint32_t eyeIndex, const uint32_t imgIndex, void* texture) = 0;
  virtual void GetDepthSwapchainImage(
    const uint32_t eyeIndex, const uint32_t imgIndex, void* texture) = 0;
  //@}

  //@{
  /**
   * Acquire swapchain images using xrEnumerateSwapchainImages and store them at the specified eye
   * index.
   */
  virtual void EnumerateColorSwapchainImages(XrSwapchain swapchain, const uint32_t eyeIndex) = 0;
  virtual void EnumerateDepthSwapchainImages(XrSwapchain swapchain, const uint32_t eyeIndex) = 0;
  //@}

  //@{
  /**
   * Acquire the number of images in the specified swapchain.
   */
  uint32_t GetChainLength(XrSwapchain swapchain);
  //@}

  //@{
  /**
   * Return the list of pixel formats supported by VTK. The first pixel format
   * in the list that is also supported by the runtime will be picked for rendering.
   */
  virtual const std::vector<int64_t>& GetSupportedColorFormats() = 0;
  virtual const std::vector<int64_t>& GetSupportedDepthFormats() = 0;
  //@}

  //@{
  /**
   * Create the graphics binding and store it in GraphicsBindings ptr.
   * It points to a XrGraphicsBindingXXX structure, depending on the
   * desired rendering backend.
   * \pre \p helperWindow must be initialized
   */
  virtual bool CreateGraphicsBinding(vtkOpenGLRenderWindow* helperWindow) = 0;
  //@}

  //@{
  /**
   * Return pointer to the backend-specific XrGraphicsBindingXXX structure
   * that is required to create the OpenXR session.
   */
  const void* GetGraphicsBinding() { return this->GraphicsBinding.get(); };
  //@}

  //@{
  /**
   * OpenXR requires checking graphics requirements before creating a session.
   * This uses a function pointer loaded with the selected graphics API extension.
   * /pre The XR instance and system id must be initialized
   */
  virtual bool CheckGraphicsRequirements(
    XrInstance instance, XrSystemId id, xr::ExtensionDispatchTable extensions) = 0;
  //@}

  //@{
  /**
   * Return the extension name to enable a specific rendering backend
   */
  virtual const char* GetBackendExtensionName() = 0;
  //@}

protected:
  vtkOpenXRManagerGraphics() = default;
  ~vtkOpenXRManagerGraphics() = default;

  std::shared_ptr<void> GraphicsBinding;

private:
  vtkOpenXRManagerGraphics(const vtkOpenXRManagerGraphics&) = delete;
  void operator=(const vtkOpenXRManagerGraphics&) = delete;
};

#endif
