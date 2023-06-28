/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUSampler.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUSampler
 * @brief WebGPU bindable object for texture samplers
 */

#ifndef vtkWebGPUSampler_h
#define vtkWebGPUSampler_h

// vtk includes
#include "vtkRenderingWebGPUModule.h" // for export macro
#include <vtkWebGPUBindableObject.h>

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations
struct WGPUSamplerDescriptor;

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUSampler : public vtkWebGPUBindableObject
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUSampler* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUSampler, vtkWebGPUBindableObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  /**
   * Create the view
   */
  virtual void Create();

  /**
   * Release the view
   */
  virtual void Destroy();

  /**
   * Get access to the handle
   */
  void* GetHandle() override;

  /**
   * Get access to the descriptor
   */
  WGPUSamplerDescriptor& GetDescriptor();

  /**
   * Get the bind group entry
   */
  void* GetBindGroupEntry() override;

  /**
   * Get the bind group layout entry
   */
  void* GetBindGroupLayoutEntry() override;

  enum AddressModes
  {
    Repeat = 0x00000000,
    MirrorRepeat = 0x00000001,
    ClampToEdge = 0x00000002,
    NumberOfAddressModes = 0x7FFFFFFF
  };

  ///@{
  /**
   * Set/Get address modes
   */
  vtkSetClampMacro(AddressModeU, int, AddressModes::Repeat, AddressModes::NumberOfAddressModes);
  vtkGetMacro(AddressModeU, int);
  vtkSetClampMacro(AddressModeV, int, AddressModes::Repeat, AddressModes::NumberOfAddressModes);
  vtkGetMacro(AddressModeV, int);
  vtkSetClampMacro(AddressModeW, int, AddressModes::Repeat, AddressModes::NumberOfAddressModes);
  vtkGetMacro(AddressModeW, int);
  ///@}

  enum FilterModes
  {
    Nearest = 0x00000000,
    Linear = 0x00000001,
    NumberOfFilterModes = 0x7FFFFFFF
  };

  ///@{
  /**
   * Set/Get the min, mag and mipmap filtering
   */
  vtkSetClampMacro(MinFilter, int, FilterModes::Nearest, FilterModes::NumberOfFilterModes);
  vtkGetMacro(MinFilter, int);
  vtkSetClampMacro(MagFilter, int, FilterModes::Nearest, FilterModes::NumberOfFilterModes);
  vtkGetMacro(MagFilter, int);
  vtkSetClampMacro(MipMapFilter, int, FilterModes::Nearest, FilterModes::NumberOfFilterModes);
  vtkGetMacro(MipMapFilter, int);
  ///@}

protected:
  vtkWebGPUSampler();
  ~vtkWebGPUSampler();

  // Helper members
  int AddressModeU = AddressModes::ClampToEdge;
  int AddressModeV = AddressModes::ClampToEdge;
  int AddressModeW = AddressModes::ClampToEdge;
  int MinFilter = FilterModes::Nearest;
  int MagFilter = FilterModes::Nearest;
  int MipMapFilter = FilterModes::Nearest;

private:
  // Internals
  class vtkInternal;
  vtkInternal* Internal = nullptr;

  vtkWebGPUSampler(const vtkWebGPUSampler&) = delete;
  void operator=(const vtkWebGPUSampler) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUSampler_h
