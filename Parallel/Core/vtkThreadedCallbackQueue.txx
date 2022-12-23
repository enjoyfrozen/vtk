/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkThreadedCallbackQueue.txx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <tuple>
#include <type_traits>

#include "vtkLogger.h"

VTK_ABI_NAMESPACE_BEGIN

namespace vtkdetail
{
//=============================================================================
struct vtkBaseWorker
{
  virtual ~vtkBaseWorker() = default;
  virtual void operator()() = 0;
};

//=============================================================================
template <std::size_t... Is>
struct vtkIntegerSequence
{
};

//=============================================================================
template <std::size_t N, std::size_t... Is>
struct vtkMakeIntegerSequence : vtkMakeIntegerSequence<N - 1, N - 1, Is...>
{
};

//=============================================================================
template <std::size_t... Is>
struct vtkMakeIntegerSequence<0, Is...> : vtkIntegerSequence<Is...>
{
};

//=============================================================================
template <class FT, class... ArgsT>
class vtkWorkerWrapper : public vtkBaseWorker
{
public:
  template <class FTT, class... ArgsTT>
  vtkWorkerWrapper(FTT&& f, ArgsTT&&... args)
    : Functor(std::forward<FT>(f))
    , Args(std::make_tuple(std::forward<ArgsT>(args)...))
  {
  }

  ~vtkWorkerWrapper() override = default;

  void operator()() override { this->Execute(vtkMakeIntegerSequence<sizeof...(ArgsT)>()); }

private:
  template <std::size_t... Is>
  void Execute(vtkIntegerSequence<Is...>)
  {
    this->Functor(std::get<Is>(this->Args)...);
  }

  FT Functor;
  std::tuple<ArgsT...> Args;
};
} // namespace vtkdetail

//-----------------------------------------------------------------------------
template <class FT, class... ArgsT>
void vtkThreadedCallbackQueue::Push(FT&& f, ArgsT&&... args)
{
  std::lock_guard<std::mutex> lock(this->Mutex);

  if (this->Size == this->AllocatedSize)
  {
    ValueType* newArray = AllocatorTraits::allocate(this->Allocator, this->AllocatedSize << 1);

    for (vtkIdType id = 0, index = this->StartIndex; id < this->Size; ++id, ++index)
    {
      index = index == this->AllocatedSize ? 0 : index;
      AllocatorTraits::construct(this->Allocator, newArray + id, std::move(this->Data[index]));
    }

    this->Data = newArray;
    this->AllocatedSize <<= 1;
    this->StartIndex = 0;
  }

  AllocatorTraits::construct(this->Allocator,
    this->Data + (this->StartIndex + this->Size) % this->AllocatedSize,
    new vtkdetail::vtkWorkerWrapper<FT,
      typename std::remove_reference<typename std::remove_cv<ArgsT>::type>::type...>(
      std::forward<FT>(f), std::forward<ArgsT>(args)...));
  ++this->Size;

  this->ConditionVariable.notify_one();
}

VTK_ABI_NAMESPACE_END
