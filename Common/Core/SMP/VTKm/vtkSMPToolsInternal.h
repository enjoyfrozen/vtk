/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSMPToolsInternal.h.in

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkCommonCoreModule.h" // For export macro
#include <vector>

#include <vtkm/cont/Algorithm.h>
#include <vtkm/cont/ArrayHandle.h>

namespace vtk
{
namespace detail
{
namespace smp
{

//--------------------------------------------------------------------------------
template <typename T>
struct FuncCall : public vtkm::exec::FunctorBase
{
  T& o;
  vtkIdType first;

  void operator=(const FuncCall&) = delete;

public:
  FuncCall(T& _o, vtkIdType _first)
    : o(_o)
    , first(_first)
  {
  }

  void operator()(vtkm::Id index) const { o.Execute(index + first, index + 1 + first); }
};

//--------------------------------------------------------------------------------
template <typename FunctorInternal>
void vtkSMPTools_Impl_For(
  vtkIdType first, vtkIdType last, vtkIdType vtkNotUsed(grain), FunctorInternal& fi)
{
  vtkm::cont::Algorithm::Schedule(FuncCall<FunctorInternal>(fi, first), last - first);
}

//--------------------------------------------------------------------------------
template <typename InputIt, typename OutputIt, typename Functor>
void vtkSMPTools_Impl_Transform(
  InputIt inBegin, InputIt inEnd, OutputIt outBegin, Functor transform)
{
  vtkm::Id size = std::distance(inBegin, inEnd);

  auto binaryTransform = [&](typename InputIt::value_type value1,
                           typename OutputIt::value_type value2) { return transform(value1); };

  auto inArray = vtkm::cont::make_ArrayHandle(&*inBegin, size, vtkm::CopyFlag::Off);
  auto outArray = vtkm::cont::make_ArrayHandle(&*outBegin, size, vtkm::CopyFlag::Off);
  vtkm::cont::Algorithm::Transform(inArray, outArray, outArray, binaryTransform);
}

//--------------------------------------------------------------------------------
template <typename InputIt1, typename InputIt2, typename OutputIt, typename Functor>
void vtkSMPTools_Impl_Transform(
  InputIt1 inBegin1, InputIt1 inEnd, InputIt2 inBegin2, OutputIt outBegin, Functor transform)
{
  vtkm::Id size = std::distance(inBegin1, inEnd);

  auto inArray1 = vtkm::cont::make_ArrayHandle(&*inBegin1, size, vtkm::CopyFlag::Off);
  auto inArray2 = vtkm::cont::make_ArrayHandle(&*inBegin2, size, vtkm::CopyFlag::Off);
  auto outArray = vtkm::cont::make_ArrayHandle(&*outBegin, size, vtkm::CopyFlag::Off);
  vtkm::cont::Algorithm::Transform(inArray1, inArray2, outArray, transform);
}

//--------------------------------------------------------------------------------
template <typename Iterator, typename T>
void vtkSMPTools_Impl_Fill(Iterator begin, Iterator end, const T& value)
{
  // Doesn't work with DataArrayTupleRange and set
  vtkm::Id size = std::distance(begin, end);
  auto array = vtkm::cont::make_ArrayHandle(&*begin, size, vtkm::CopyFlag::Off);
  vtkm::cont::Algorithm::Fill(array, value);
}

//--------------------------------------------------------------------------------
template <typename RandomAccessIterator>
void vtkSMPTools_Impl_Sort(RandomAccessIterator begin, RandomAccessIterator end)
{
  vtkm::Id size = std::distance(begin, end);
  auto array = vtkm::cont::make_ArrayHandle(&*begin, size, vtkm::CopyFlag::Off);
  vtkm::cont::Algorithm::Sort(array);
}

//--------------------------------------------------------------------------------
template <typename RandomAccessIterator, typename Compare>
void vtkSMPTools_Impl_Sort(RandomAccessIterator begin, RandomAccessIterator end, Compare comp)
{
  vtkm::Id size = std::distance(begin, end);
  auto array = vtkm::cont::make_ArrayHandle(&*begin, size, vtkm::CopyFlag::Off);
  vtkm::cont::Algorithm::Sort(array, comp);
}

} // namespace smp
} // namespace detail
} // namespace vtk
