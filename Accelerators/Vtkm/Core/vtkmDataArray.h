//============================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//
//  Copyright 2019 Sandia Corporation.
//  Copyright 2019 UT-Battelle, LLC.
//  Copyright 2019 Los Alamos National Security.
//
//  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
//  the U.S. Government retains certain rights in this software.
//
//  Under the terms of Contract DE-AC52-06NA25396 with Los Alamos National
//  Laboratory (LANL), the U.S. Government retains certain rights in
//  this software.
//============================================================================
#ifndef vtkmDataArray_h
#define vtkmDataArray_h

#include "vtkAcceleratorsVTKmCoreModule.h" // For export macro
#include "vtkGenericDataArray.h"
#include "vtkmConfigCore.h" // For template export

#include <vtkm/List.h>                    // For vtkm::List
#include <vtkm/VecFromPortal.h>           // For vtkm::VecFromPortal
#include <vtkm/VecTraits.h>               // For vtkm::VecTraits
#include <vtkm/cont/ArrayHandle.h>        // For vtkm::cont::ArrayHandle
#include <vtkm/cont/UnknownArrayHandle.h> // For vtkm::cont::UnknownArrayHandle

#include <memory> // For unique_ptr

enum class ALLOC
{
  HOST = 0,
  DEVICE
};

struct MetaData
{
  const void* Device;
  const void* Context;
  size_t NumPointers;
  std::vector<const void*> Pointers;
  std::vector<ALLOC> Allocs;
};

namespace internal
{
VTK_ABI_NAMESPACE_BEGIN

template <typename T>
class ArrayHandleWrapperBase;

VTK_ABI_NAMESPACE_END
} // internal

VTK_ABI_NAMESPACE_BEGIN
enum class WrapOnly
{
  On,
  Off
};

template <typename T>
class vtkmDataArray : public vtkGenericDataArray<vtkmDataArray<T>, T>
{
  static_assert(std::is_arithmetic<T>::value, "T must be an integral or floating-point type");

  using GenericDataArrayType = vtkGenericDataArray<vtkmDataArray<T>, T>;

public:
  using SelfType = vtkmDataArray<T>;
  vtkTemplateTypeMacro(SelfType, GenericDataArrayType);

  using typename Superclass::ValueType;

  using VtkmTypesList = vtkm::List<T, vtkm::Vec<T, 2>, vtkm::Vec<T, 3>, vtkm::Vec<T, 4>,
    vtkm::VecFromPortal<typename vtkm::cont::ArrayHandle<T>::WritePortalType>>;

  static vtkmDataArray* New();

  template <typename V, typename S>
  void SetVtkmArrayHandle(const vtkm::cont::ArrayHandle<V, S>& ah);

  vtkm::cont::UnknownArrayHandle GetVtkmUnknownArrayHandle() const;

  /// concept methods for \c vtkGenericDataArray
  ValueType GetValue(vtkIdType valueIdx) const;
  void SetValue(vtkIdType valueIdx, ValueType value);
  void GetTypedTuple(vtkIdType tupleIdx, ValueType* tuple) const;
  void SetTypedTuple(vtkIdType tupleIdx, const ValueType* tuple);
  ValueType GetTypedComponent(vtkIdType tupleIdx, int compIdx) const;
  void SetTypedComponent(vtkIdType tupleIdx, int compIdx, ValueType value);

  void SetWrapOnly();

  MetaData GetArrayInformation() const;
  // std::vector<const void*> GetHostArrays() const;

protected:
  vtkmDataArray();
  ~vtkmDataArray() override;

  /// concept methods for \c vtkGenericDataArray
  bool AllocateTuples(vtkIdType numTuples);
  bool ReallocateTuples(vtkIdType numTuples);

private:
  vtkmDataArray(const vtkmDataArray&) = delete;
  void operator=(const vtkmDataArray&) = delete;

  // To access AllocateTuples and ReallocateTuples
  friend Superclass;

  bool WrapOnly = false;
  std::unique_ptr<internal::ArrayHandleWrapperBase<T>> VtkmArray;
};

//=============================================================================
template <typename T, typename S>
inline vtkmDataArray<typename vtkm::VecTraits<T>::BaseComponentType>* make_vtkmDataArray(
  const vtkm::cont::ArrayHandle<T, S>& ah, const WrapOnly wrapOnly = WrapOnly::Off)
{
  auto ret = vtkmDataArray<typename vtkm::VecTraits<T>::BaseComponentType>::New();
  if (wrapOnly == WrapOnly::On)
    ret->SetWrapOnly();
  ret->SetVtkmArrayHandle(ah);
  return ret;
}

//=============================================================================
#ifndef vtkmDataArray_cxx
extern template class VTKACCELERATORSVTKMCORE_TEMPLATE_EXPORT vtkmDataArray<char>;
extern template class VTKACCELERATORSVTKMCORE_TEMPLATE_EXPORT vtkmDataArray<double>;
extern template class VTKACCELERATORSVTKMCORE_TEMPLATE_EXPORT vtkmDataArray<float>;
extern template class VTKACCELERATORSVTKMCORE_TEMPLATE_EXPORT vtkmDataArray<int>;
extern template class VTKACCELERATORSVTKMCORE_TEMPLATE_EXPORT vtkmDataArray<long>;
extern template class VTKACCELERATORSVTKMCORE_TEMPLATE_EXPORT vtkmDataArray<long long>;
extern template class VTKACCELERATORSVTKMCORE_TEMPLATE_EXPORT vtkmDataArray<short>;
extern template class VTKACCELERATORSVTKMCORE_TEMPLATE_EXPORT vtkmDataArray<signed char>;
extern template class VTKACCELERATORSVTKMCORE_TEMPLATE_EXPORT vtkmDataArray<unsigned char>;
extern template class VTKACCELERATORSVTKMCORE_TEMPLATE_EXPORT vtkmDataArray<unsigned int>;
extern template class VTKACCELERATORSVTKMCORE_TEMPLATE_EXPORT vtkmDataArray<unsigned long>;
extern template class VTKACCELERATORSVTKMCORE_TEMPLATE_EXPORT vtkmDataArray<unsigned long long>;
extern template class VTKACCELERATORSVTKMCORE_TEMPLATE_EXPORT vtkmDataArray<unsigned short>;
#endif // vtkmDataArray_cxx

VTK_ABI_NAMESPACE_END
#endif // vtkmDataArray_h

#include "vtkmlib/vtkmDataArray.hxx"

// VTK-HeaderTest-Exclude: vtkmDataArray.h
