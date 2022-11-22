/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkIndexedArray.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef vtkIndexedArray_h
#define vtkIndexedArray_h

#ifdef VTK_INDEXED_ARRAY_INSTANTIATING
#define VTK_IMPLICIT_VALUERANGE_INSTANTIATING
#include "vtkDataArrayPrivate.txx"
#endif

#include "vtkCommonImplicitArraysModule.h" // for export macro
#include "vtkImplicitArray.h"
#include "vtkIndexedImplicitBackend.h" // for the array backend

#ifdef VTK_INDEXED_ARRAY_INSTANTIATING
#undef VTK_IMPLICIT_VALUERANGE_INSTANTIATING
#endif

#include <vector>

/**
 * \var vtkIndexedArray
 * \brief A utility alias for creating a wrapper array around an existing array and reindexing its
 * components
 *
 * In order to be usefully included in the dispatchers, these arrays need to be instantiated at the
 * vtk library compile time.
 *
 * @sa
 * vtkImplicitArray vtkIndexedImplicitBackend
 */

VTK_ABI_NAMESPACE_BEGIN
template <typename T>
using vtkIndexedArray = vtkImplicitArray<vtkIndexedImplicitBackend<T>>;
VTK_ABI_NAMESPACE_END

#endif // vtkIndexedArray_h

#ifdef VTK_INDEXED_ARRAY_INSTANTIATING

#define VTK_INSTANTIATE_INDEXED_ARRAY(ValueType)                                                   \
  VTK_ABI_NAMESPACE_BEGIN                                                                          \
  template class VTKCOMMONIMPLICITARRAYS_EXPORT                                                    \
    vtkImplicitArray<vtkIndexedImplicitBackend<ValueType>>;                                        \
  VTK_ABI_NAMESPACE_END                                                                            \
  namespace vtkDataArrayPrivate                                                                    \
  {                                                                                                \
  VTK_ABI_NAMESPACE_BEGIN                                                                          \
  VTK_INSTANTIATE_VALUERANGE_ARRAYTYPE(                                                            \
    vtkImplicitArray<vtkIndexedImplicitBackend<ValueType>>, double)                                \
  VTK_ABI_NAMESPACE_END                                                                            \
  }

#endif
