/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUType.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// vtk includes
#include "vtkWebGPUType.h"
#include "vtkObjectFactory.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUType);

//-------------------------------------------------------------------------------------------------
vtkWebGPUType::vtkWebGPUType() = default;

//-------------------------------------------------------------------------------------------------
vtkWebGPUType::~vtkWebGPUType() = default;

//-------------------------------------------------------------------------------------------------
void vtkWebGPUType::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUType::vtkWebGPUTypeDetails vtkWebGPUType::GetDetailsFromTextureFormat(
  WGPUTextureFormat t, bool& ok)
{
  vtkWebGPUType::vtkWebGPUTypeDetails details = {};
  ok = true;
  switch (t)
  {
    // 8-bit formats
    case WGPUTextureFormat_R8Unorm:
      details.NumberOfComponents = 1;
      details.VTKType = VTK_UNSIGNED_CHAR;
      details.Stride = 1;
      details.ElementSize = 1;
      details.SampleType = WGPUTextureSampleType_Float;
      break;
    case WGPUTextureFormat_R8Snorm:
      details.NumberOfComponents = 1;
      details.VTKType = VTK_SIGNED_CHAR;
      details.Stride = 1;
      details.ElementSize = 1;
      details.SampleType = WGPUTextureSampleType_Float;
      break;
    case WGPUTextureFormat_R8Uint:
      details.NumberOfComponents = 1;
      details.VTKType = VTK_UNSIGNED_CHAR;
      details.Stride = 1;
      details.ElementSize = 1;
      details.SampleType = WGPUTextureSampleType_Uint;
      break;
    case WGPUTextureFormat_R8Sint:
      details.NumberOfComponents = 1;
      details.VTKType = VTK_SIGNED_CHAR;
      details.Stride = 1;
      details.ElementSize = 1;
      details.SampleType = WGPUTextureSampleType_Sint;
      break;

    // 16-bit formats
    case WGPUTextureFormat_R16Uint:
      details.NumberOfComponents = 1;
      details.VTKType = VTK_UNSIGNED_SHORT;
      details.Stride = 2;
      details.ElementSize = 2;
      details.SampleType = WGPUTextureSampleType_Uint;
      break;
    case WGPUTextureFormat_R16Sint:
      details.NumberOfComponents = 1;
      details.VTKType = VTK_SHORT;
      details.Stride = 2;
      details.ElementSize = 2;
      details.SampleType = WGPUTextureSampleType_Sint;
      break;
    case WGPUTextureFormat_R16Float:
      details.NumberOfComponents = 1;
      details.VTKType = VTK_FLOAT;
      details.Stride = 2;
      details.ElementSize = 2;
      details.SampleType = WGPUTextureSampleType_Float;
      break;
    case WGPUTextureFormat_RG8Unorm:
      details.NumberOfComponents = 2;
      details.VTKType = VTK_UNSIGNED_CHAR;
      details.Stride = 2;
      details.ElementSize = 1;
      details.SampleType = WGPUTextureSampleType_Float;
      break;
    case WGPUTextureFormat_RG8Snorm:
      details.NumberOfComponents = 2;
      details.VTKType = VTK_SIGNED_CHAR;
      details.Stride = 2;
      details.ElementSize = 1;
      details.SampleType = WGPUTextureSampleType_Float;
      break;
    case WGPUTextureFormat_RG8Uint:
      details.NumberOfComponents = 2;
      details.VTKType = VTK_UNSIGNED_CHAR;
      details.Stride = 2;
      details.ElementSize = 1;
      details.SampleType = WGPUTextureSampleType_Uint;
      break;
    case WGPUTextureFormat_RG8Sint:
      details.NumberOfComponents = 2;
      details.VTKType = VTK_SIGNED_CHAR;
      details.Stride = 2;
      details.ElementSize = 1;
      details.SampleType = WGPUTextureSampleType_Sint;
      break;

    // 32-bit formats
    case WGPUTextureFormat_R32Uint:
      details.NumberOfComponents = 1;
      details.VTKType = VTK_TYPE_UINT32;
      details.Stride = 4;
      details.ElementSize = 4;
      details.SampleType = WGPUTextureSampleType_Uint;
      break;
    case WGPUTextureFormat_R32Sint:
      details.NumberOfComponents = 1;
      details.VTKType = VTK_TYPE_INT32;
      details.Stride = 4;
      details.ElementSize = 4;
      details.SampleType = WGPUTextureSampleType_Sint;
      break;
    case WGPUTextureFormat_R32Float:
      details.NumberOfComponents = 1;
      details.VTKType = VTK_FLOAT;
      details.Stride = 4;
      details.ElementSize = 4;
      details.SampleType = WGPUTextureSampleType_UnfilterableFloat;
      break;
    case WGPUTextureFormat_RG16Uint:
      details.NumberOfComponents = 2;
      details.VTKType = VTK_UNSIGNED_SHORT;
      details.Stride = 4;
      details.ElementSize = 2;
      details.SampleType = WGPUTextureSampleType_Uint;
      break;
    case WGPUTextureFormat_RG16Sint:
      details.NumberOfComponents = 2;
      details.VTKType = VTK_SHORT;
      details.Stride = 4;
      details.ElementSize = 2;
      details.SampleType = WGPUTextureSampleType_Sint;
      break;
    case WGPUTextureFormat_RG16Float:
      details.NumberOfComponents = 2;
      details.VTKType = VTK_FLOAT;
      details.Stride = 4;
      details.ElementSize = 2;
      details.SampleType = WGPUTextureSampleType_Float;
      break;
    case WGPUTextureFormat_RGBA8Unorm:
    case WGPUTextureFormat_RGBA8UnormSrgb:
    case WGPUTextureFormat_BGRA8Unorm:
    case WGPUTextureFormat_BGRA8UnormSrgb:
      details.NumberOfComponents = 4;
      details.VTKType = VTK_UNSIGNED_CHAR;
      details.Stride = 4;
      details.ElementSize = 1;
      details.SampleType = WGPUTextureSampleType_Float;
      break;
    case WGPUTextureFormat_RGBA8Snorm:
      details.NumberOfComponents = 4;
      details.VTKType = VTK_SIGNED_CHAR;
      details.Stride = 4;
      details.ElementSize = 1;
      details.SampleType = WGPUTextureSampleType_Float;
      break;
    case WGPUTextureFormat_RGBA8Uint:
      details.NumberOfComponents = 4;
      details.VTKType = VTK_UNSIGNED_CHAR;
      details.Stride = 4;
      details.ElementSize = 1;
      details.SampleType = WGPUTextureSampleType_Uint;
      break;
    case WGPUTextureFormat_RGBA8Sint:
      details.NumberOfComponents = 4;
      details.VTKType = VTK_SIGNED_CHAR;
      details.Stride = 4;
      details.ElementSize = 1;
      details.SampleType = WGPUTextureSampleType_Sint;
      break;
    // Packed 32-bit formats
    case WGPUTextureFormat_RGB9E5Ufloat:
    case WGPUTextureFormat_RGB10A2Unorm:
      details.NumberOfComponents = 4;
      details.VTKType = VTK_TYPE_UINT32;
      details.Stride = 4;
      details.ElementSize = 1;
      details.SampleType = WGPUTextureSampleType_Float;
      break;
    case WGPUTextureFormat_RG11B10Ufloat:
      details.NumberOfComponents = 4;
      details.VTKType = VTK_FLOAT;
      details.Stride = 4;
      details.ElementSize = 1;
      details.SampleType = WGPUTextureSampleType_Float;
      break;

    // 64-bit formats
    case WGPUTextureFormat_RG32Uint:
      details.NumberOfComponents = 2;
      details.VTKType = VTK_TYPE_UINT32;
      details.Stride = 8;
      details.ElementSize = 4;
      details.SampleType = WGPUTextureSampleType_Uint;
      break;
    case WGPUTextureFormat_RG32Sint:
      details.NumberOfComponents = 2;
      details.VTKType = VTK_TYPE_INT32;
      details.Stride = 8;
      details.ElementSize = 4;
      details.SampleType = WGPUTextureSampleType_Sint;
      break;
    case WGPUTextureFormat_RG32Float:
      details.NumberOfComponents = 2;
      details.VTKType = VTK_FLOAT;
      details.Stride = 8;
      details.ElementSize = 4;
      details.SampleType = WGPUTextureSampleType_UnfilterableFloat;
      break;
    case WGPUTextureFormat_RGBA16Uint:
      details.NumberOfComponents = 4;
      details.VTKType = VTK_UNSIGNED_SHORT;
      details.Stride = 8;
      details.ElementSize = 2;
      details.SampleType = WGPUTextureSampleType_Uint;
      break;
    case WGPUTextureFormat_RGBA16Sint:
      details.NumberOfComponents = 4;
      details.VTKType = VTK_SHORT;
      details.Stride = 8;
      details.ElementSize = 2;
      details.SampleType = WGPUTextureSampleType_Sint;
      break;
    case WGPUTextureFormat_RGBA16Float:
      details.NumberOfComponents = 4;
      details.VTKType = VTK_FLOAT;
      details.Stride = 8;
      details.ElementSize = 2;
      details.SampleType = WGPUTextureSampleType_Float;
      break;

    // 128-bit formats
    case WGPUTextureFormat_RGBA32Uint:
      details.NumberOfComponents = 4;
      details.VTKType = VTK_TYPE_UINT32;
      details.Stride = 16;
      details.ElementSize = 4;
      details.SampleType = WGPUTextureSampleType_Uint;
      break;
    case WGPUTextureFormat_RGBA32Sint:
      details.NumberOfComponents = 4;
      details.VTKType = VTK_TYPE_INT32;
      details.Stride = 16;
      details.ElementSize = 4;
      details.SampleType = WGPUTextureSampleType_Sint;
      break;
    case WGPUTextureFormat_RGBA32Float:
      details.NumberOfComponents = 4;
      details.VTKType = VTK_FLOAT;
      details.Stride = 16;
      details.ElementSize = 4;
      details.SampleType = WGPUTextureSampleType_UnfilterableFloat;
      break;

    // Depth and stencil formats
    case WGPUTextureFormat_Stencil8:
      details.NumberOfComponents = 1;
      details.VTKType = VTK_UNSIGNED_CHAR;
      details.Stride = 1;
      details.ElementSize = 1;
      details.SampleType = WGPUTextureSampleType_Uint;
      break;
    case WGPUTextureFormat_Depth16Unorm:
      details.NumberOfComponents = 1;
      details.VTKType = VTK_UNSIGNED_SHORT;
      details.Stride = 2;
      details.ElementSize = 2;
      details.SampleType = WGPUTextureSampleType_Depth;
      break;
    case WGPUTextureFormat_Depth24Plus:
      details.NumberOfComponents = 1;
      details.VTKType = VTK_TYPE_UINT32;
      details.Stride = 4;
      details.ElementSize = 3;
      details.SampleType = WGPUTextureSampleType_Depth;
      break;
    case WGPUTextureFormat_Depth24PlusStencil8:
      details.NumberOfComponents = 2;
      details.VTKType = VTK_TYPE_UINT32;
      details.Stride = 4;
      details.ElementSize = 4;
      details.SampleType = WGPUTextureSampleType_Depth;
      break;
    case WGPUTextureFormat_Depth32Float:
      details.NumberOfComponents = 1;
      details.VTKType = VTK_FLOAT;
      details.Stride = 4;
      details.ElementSize = 4;
      details.SampleType = WGPUTextureSampleType_Depth;
      break;
    case WGPUTextureFormat_Depth32FloatStencil8:
      details.NumberOfComponents = 2;
      details.VTKType = VTK_FLOAT;
      details.Stride = 4;
      details.ElementSize = 4;
      details.SampleType = WGPUTextureSampleType_Depth;
      break;

    // could not deduce format
    default:
      details.NumberOfComponents = 1;
      details.VTKType = VTK_FLOAT;
      details.Stride = 1;
      details.ElementSize = 1;
      details.SampleType = WGPUTextureSampleType_Undefined;
      ok = false;
      break;
  }
  return details;
}

//-------------------------------------------------------------------------------------------------
WGPUTextureSampleType vtkWebGPUType::GetTextureSampleTypeFromFormat(WGPUTextureFormat t, bool& ok)
{
  vtkWebGPUType type;
  vtkWebGPUType::vtkWebGPUTypeDetails details = type.GetDetailsFromTextureFormat(t, ok);
  if (!ok)
  {
    return details.SampleType;
  }
  return WGPUTextureSampleType_Undefined;
}

//-------------------------------------------------------------------------------------------------
WGPUTextureSampleType vtkWebGPUType::GetTextureSampleTypeFromFormat(uint32_t t, bool& ok)
{
  return vtkWebGPUType::GetTextureSampleTypeFromFormat(static_cast<WGPUTextureFormat>(t), ok);
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
