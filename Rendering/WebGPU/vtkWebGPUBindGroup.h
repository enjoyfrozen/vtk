/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUBindGroup.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUBindGroup
 * @brief Bindables in the webgpu backend
 */

#ifndef vtkWebGPUBindGroup_h
#define vtkWebGPUBindGroup_h

// vtk includes
#include "vtkRenderingWebGPUModule.h" // for export macro
#include "vtkWebGPUObject.h"

// STL includes
#include <vector> // for vector

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations
struct WGPUBindGroupDescriptor;
class vtkWebGPUDevice;
class vtkWebGPUBindableObject;

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUBindGroup : public vtkWebGPUObject
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUBindGroup* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUBindGroup, vtkWebGPUObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  /**
   * Get a handle to the webgpu bind group
   */
  void* GetHandle() override;

  /**
   * Get access to the descriptor
   */
  WGPUBindGroupDescriptor& GetDescriptor();

  /**
   * Create the bind group
   */
  virtual void Create(vtkWebGPUDevice* d);

  /**
   * Destroy/Release the bind group
   */
  virtual void Destroy();

  ///@{
  /*
   * Set/Get bindables of the bind group
   */
  virtual void SetBindables(std::vector<vtkWebGPUBindableObject*> b);
  virtual std::vector<vtkWebGPUBindableObject*> GetBindables();
  ///@}

  /**
   * Get a handle to the bind group layout
   */
  virtual void* GetBindGroupLayout();

  /**
   * Get the bind group creation time
   */
  virtual vtkMTimeType GetBindGroupTime() const;

protected:
  vtkWebGPUBindGroup();
  ~vtkWebGPUBindGroup();

  // Helper members
  std::vector<vtkWebGPUBindableObject*> Bindables;

  ///@(
  /**
   * Create/destroy bind group layout
   */
  virtual void CreateBindGroupLayout(vtkWebGPUDevice*);
  virtual void DestroyBindGroupLayout();
  ///@}

  /**
   * Cache bind group creation time
   */
  vtkTimeStamp BindGroupTime;

  /**
   * Need to recreate bind group
   */
  virtual vtkTypeBool GetNeedToRecreateBindGroup();

private:
  class vtkInternal;
  vtkInternal* Internal;

  vtkWebGPUBindGroup(const vtkWebGPUBindGroup&) = delete;
  void operator=(const vtkWebGPUBindGroup) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUBindGroup_h
