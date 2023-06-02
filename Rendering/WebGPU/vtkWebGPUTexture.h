/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUTexture.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUTexture
 * @brief  Convenience class for textures in webgpu
 *
 */

#ifndef vtkWebGPUTexture_h
#define vtkWebGPUTexture_h

// VTK includes
#include "vtkRenderingWebGPUModule.h" // for export macro
#include "vtkWebGPUObject.h"

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations
class vtkWebGPUDevice;

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUTexture : public vtkWebGPUObject
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUTexture* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUTexture, vtkWebGPUObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  /**
   * Create the texture
   */
  virtual void Create(vtkWebGPUDevice* d);

  /**
   * Destroy/Release the texture
   */
  virtual void Destroy();

  ///@{
  /**
   * Set/Get the texture dimensions
   */
  vtkSetVector3Macro(Dimensions, uint32_t);
  vtkGetVector3Macro(Dimensions, uint32_t);
  ///@}

  ///@{
  /**
   * Set/Get the MIP level
   */
  vtkSetMacro(MIPLevel, uint32_t);
  vtkGetMacro(MIPLevel, uint32_t);
  ///@}

  ///@{
  /**
   * Set/Get the format
   */
  vtkSetMacro(Format, uint32_t);
  vtkGetMacro(Format, uint32_t);
  ///@}

  ///@{
  /**
   * Set/Get texture usage flags
   */
  vtkSetMacro(UsageFlags, uint32_t);
  vtkGetMacro(UsageFlags, uint32_t);
  ///@}

  /**
   * Get access to the handle
   */
  void* GetHandle() override;

protected:
  vtkWebGPUTexture();
  ~vtkWebGPUTexture();

  // Helper members
  uint32_t Dimensions[3] = { 1, 1, 1 };
  uint32_t MIPLevel = 0;
  uint32_t Format = 0;
  uint32_t UsageFlags = 0;

private:
  // Internals
  class vtkInternal;
  vtkInternal* Internal = nullptr;

  vtkWebGPUTexture(const vtkWebGPUTexture&) = delete;
  void operator=(const vtkWebGPUTexture) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUTexture_h
