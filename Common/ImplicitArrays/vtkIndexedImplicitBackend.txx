/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkIndexedImplicitBackend.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkIndexedImplicitBackend.h"

#include "vtkDataArrayRange.h"
#include "vtkIdList.h"

VTK_ABI_NAMESPACE_BEGIN
//-----------------------------------------------------------------------
template <typename ValueType>
struct vtkIndexedImplicitBackend<ValueType>::Internals
{
  Internals(vtkIdList* indexes, vtkDataArray* array)
    : Handles(indexes)
    , Array(array)
  {
    if (this->Handles == nullptr || this->Array == nullptr)
    {
      vtkErrorWithObjectMacro(nullptr, "Either index array or array itself is nullptr");
      return;
    }
    this->Range = vtk::DataArrayValueRange(this->Array);
  }

  vtkSmartPointer<vtkIdList> Handles;
  vtkSmartPointer<vtkDataArray> Array;
  vtk::detail::SelectValueRange<vtkDataArray*, vtk::detail::DynamicTupleSize>::type Range;
};

//-----------------------------------------------------------------------
template <typename ValueType>
vtkIndexedImplicitBackend<ValueType>::vtkIndexedImplicitBackend(
  vtkIdList* indexes, vtkDataArray* array)
  : Internal(std::unique_ptr<Internals>(new Internals(indexes, array)))
{
}

//-----------------------------------------------------------------------
template <typename ValueType>
vtkIndexedImplicitBackend<ValueType>::~vtkIndexedImplicitBackend()
{
}

//-----------------------------------------------------------------------
template <typename ValueType>
ValueType vtkIndexedImplicitBackend<ValueType>::operator()(int idx) const
{
  return static_cast<ValueType>(this->Internal->Range[this->Internal->Handles->GetId(idx)]);
}
VTK_ABI_NAMESPACE_END
