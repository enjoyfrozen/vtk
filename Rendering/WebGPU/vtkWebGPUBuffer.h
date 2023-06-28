/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUBuffer.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUBuffer
 * @brief An object that represents a webgpu buffer
 */

#ifndef vtkWebGPUBuffer_h
#define vtkWebGPUBuffer_h

// vtk includes
#include "vtkRenderingWebGPUModule.h" // for export macro
#include "vtkWebGPUBindableObject.h"

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations
struct WGPUBufferDescriptor;
class vtkDataArray;

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUBuffer : public vtkWebGPUBindableObject
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUBuffer* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUBuffer, vtkWebGPUBindableObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  /**
   * Get access to the handle
   */
  void* GetHandle() override;

  /**
   * Get access to the descriptor
   */
  WGPUBufferDescriptor& GetDescriptor();

  enum BufferUsage
  {
    None = 0x00000000,
    MapRead = 0x00000001,
    MapWrite = 0x00000002,
    CopySrc = 0x00000004,
    CopyDst = 0x00000008,
    Index = 0x00000010,
    Vertex = 0x00000020,
    Uniform = 0x00000040,
    Storage = 0x00000080,
    Indirect = 0x00000100,
    QueryResolve = 0x00000200,
  };

  enum BufferMapState
  {
    Unmapped = 0x00000000,
    Pending = 0x00000001,
    Mapped = 0x00000002,
  };

  ///@{
  /**
   * Set/Get buffer usage flags
   */
  vtkSetMacro(UsageFlags, vtkTypeUInt32);
  vtkGetMacro(UsageFlags, vtkTypeUInt32);
  ///@}

  ///@{
  /**
   * Set/Get the buffer size in bytes
   */
  vtkSetMacro(SizeInBytes, vtkTypeUInt64);
  vtkGetMacro(SizeInBytes, vtkTypeUInt64);
  ///@}

  ///@{
  /**
   * Set/Get whether the buffer will be mapped right away at creation time
   */
  vtkSetMacro(MappedAtCreation, vtkTypeBool);
  vtkGetMacro(MappedAtCreation, vtkTypeBool);
  vtkBooleanMacro(MappedAtCreation, vtkTypeBool);
  ///@}

  /**
   * Create the buffer
   */
  virtual void Create();

  /**
   * Destroy the buffer
   */
  virtual void Destroy();

  ///@{
  /**
   * Write a data array to the buffer
   */
  virtual void Write(vtkDataArray* a);
  virtual void Write(void* data, vtkTypeUInt64 size);
  ///@}

  /**
   * Create and map the buffer
   */
  virtual void CreateAndWrite(void* data, vtkTypeUInt64 size, vtkTypeUInt64 usage);

  /**
   * Get the mapped state of the buffer
   */
  virtual BufferMapState GetMappedState();

  /**
   * Get the bind group entry
   */
  void* GetBindGroupEntry() override;

  /**
   * Get the bind group layout entry
   */
  void* GetBindGroupLayoutEntry() override;

protected:
  vtkWebGPUBuffer();
  ~vtkWebGPUBuffer();

  // Helper members
  vtkTypeUInt32 UsageFlags = BufferUsage::None;
  vtkTypeUInt64 SizeInBytes = 0;
  vtkTypeBool MappedAtCreation = false;

private:
  // Internals
  class vtkInternal;
  vtkInternal* Internal = nullptr;

  vtkWebGPUBuffer(const vtkWebGPUBuffer&) = delete;
  void operator=(const vtkWebGPUBuffer) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUBuffer_h
