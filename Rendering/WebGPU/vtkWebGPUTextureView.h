/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUTextureView.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUTextureView
 * @brief Convenience class for managing texture views in webgpu
 *
 */
#ifndef vtkWebGPUTextureView_h
#define vtkWebGPUTextureView_h

// VTK includes
#include "vtkRenderingWebGPUModule.h" // for export macro
#include "vtkWebGPUBindableObject.h"

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations
class vtkWebGPUTexture;

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUTextureView : public vtkWebGPUBindableObject
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUTextureView* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUTextureView, vtkWebGPUBindableObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  ///@{
  /**
   * Set/Get the texture for this view
   */
  virtual void SetTexture(vtkWebGPUTexture* tex);
  vtkGetObjectMacro(Texture, vtkWebGPUTexture);
  ///@}

  /**
   * Create the view
   */
  virtual void Create();

  /**
   * Release the view
   */
  virtual void Destroy();

  ///@{
  /**
   * Set/Get the format
   */
  vtkSetMacro(Format, uint32_t);
  vtkGetMacro(Format, uint32_t);
  ///@}

  enum Dimensions
  {
    TEXTUREVIEW_2D = 0,
    TEXTUREVIEW_3D = 1
  };

  ///@{
  /**
   * Set/Get the dimensionality of the texture view.
   */
  vtkSetClampMacro(Dimension, int, TEXTUREVIEW_2D, TEXTUREVIEW_3D);
  vtkGetMacro(Dimension, int);
  ///@}

  /**
   * Get access to the handle
   */
  void* GetHandle() override;

  /**
   * Get the bind group entry
   */
  void* GetBindGroupEntry() override;

  /**
   * Get the bind group layout entry
   */
  void* GetBindGroupLayoutEntry() override;

protected:
  vtkWebGPUTextureView();
  ~vtkWebGPUTextureView();

  // Helper members
  vtkWebGPUTexture* Texture = nullptr;
  uint32_t Format = 0;
  int Dimension = TEXTUREVIEW_2D;

private:
  // Internals
  class vtkInternal;
  vtkInternal* Internal = nullptr;

  vtkWebGPUTextureView(const vtkWebGPUTextureView&) = delete;
  void operator=(const vtkWebGPUTextureView) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUTextureView_h
