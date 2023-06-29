/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUType.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUType
 * @brief Utility class to handle type conversions between vtk and webgpu
 *
 */

#ifndef vtkWebGPUType_h
#define vtkWebGPUType_h

// vtk includes
#include "vtkObject.h"
#include "vtkRenderingWebGPUModule.h" // for export macro
#include "vtk_wgpu.h"                 // for webgpu

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUType : public vtkObject
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUType* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUType, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  ///@{
  /**
   * Get the texture sample type from the texture format
   */
  static WGPUTextureSampleType GetTextureSampleTypeFromFormat(WGPUTextureFormat, bool& ok);
  static WGPUTextureSampleType GetTextureSampleTypeFromFormat(uint32_t, bool& ok);
  ///@}

  enum PrimitiveTopologies
  {
    PointList = 0x00000000,
    LineList = 0x00000001,
    LineStrip = 0x00000002,
    TriangleList = 0x00000003,
    TriangleStrip = 0x00000004,
    NumberOfPrimitiveTopologies = 0x7FFFFFFF
  };

protected:
  vtkWebGPUType();
  ~vtkWebGPUType();

  // Helper members
  typedef struct vtkWebGPUTypeDetails
  {
    WGPUTextureSampleType SampleType;
    int VTKType;
    int Stride;
    int NumberOfComponents;
    int ElementSize;
  } vtkWebGPUTypeDetails;

  /**
   * Get the type details given a specific texture format
   */
  static vtkWebGPUTypeDetails GetDetailsFromTextureFormat(WGPUTextureFormat, bool& ok);

private:
  vtkWebGPUType(const vtkWebGPUType&) = delete;
  void operator=(const vtkWebGPUType) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUType_h
