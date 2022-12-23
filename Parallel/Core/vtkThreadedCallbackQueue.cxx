/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkThreadedCallbackQueue.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkThreadedCallbackQueue.h"
#include "vtkObjectFactory.h"

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkThreadedCallbackQueue);

//-----------------------------------------------------------------------------
vtkThreadedCallbackQueue::vtkThreadedCallbackQueue()
  : Data(AllocatorTraits::allocate(Allocator, AllocatedSize))
  , Size(0)
  , Destroying(false)
  , Done(true)
  , NumberOfThreads(1)
  , Threads(NumberOfThreads)
{
}

//-----------------------------------------------------------------------------
vtkThreadedCallbackQueue::~vtkThreadedCallbackQueue()
{
  if (!this->Done)
  {
    this->Destroying = true;
    this->ConditionVariable.notify_all();
    this->Sync();
  }

  for (vtkIdType id = 0, index = this->StartIndex; id < this->Size; ++id, ++index)
  {
    // sanitizing in case something went wrong? like number of threads set to zero?
    index = index == this->AllocatedSize ? 0 : index;
    AllocatorTraits::destroy(this->Allocator, this->Data + index);
  }

  AllocatorTraits::deallocate(this->Allocator, this->Data, this->AllocatedSize);
}

//-----------------------------------------------------------------------------
void vtkThreadedCallbackQueue::SetNumberOfThreads(int numberOfThreads)
{
  if (static_cast<std::size_t>(numberOfThreads) == this->Threads.size())
  {
    return;
  }

  this->Stop();
  this->NumberOfThreads = numberOfThreads;
  this->Threads.resize(numberOfThreads);
}

//-----------------------------------------------------------------------------
void vtkThreadedCallbackQueue::Stop()
{
  if (this->Done)
  {
    return;
  }

  this->Done = true;
  this->ConditionVariable.notify_all();
  this->Sync();
}

//-----------------------------------------------------------------------------
void vtkThreadedCallbackQueue::Start()
{
  if (!this->Done)
  {
    return;
  }

  this->Done = false;

  for (auto& thread : this->Threads)
  {
    thread = std::unique_ptr<std::thread>(new std::thread([this] {
      while (!this->Done && (!this->Destroying || this->Size))
      {
        this->Pop();
      }
    }));
  }
}

//-----------------------------------------------------------------------------
void vtkThreadedCallbackQueue::Sync()
{
  for (auto& thread : this->Threads)
  {
    thread->join();
  }
}

//-----------------------------------------------------------------------------
void vtkThreadedCallbackQueue::Pop()
{
  std::unique_lock<std::mutex> lock(this->Mutex);
  this->ConditionVariable.wait(lock, [this] { return this->Size || this->Done; });

  if (this->Done)
  {
    return;
  }

  ValueType f = std::move(this->Data[this->StartIndex]);
  this->StartIndex = ++this->StartIndex == this->AllocatedSize ? 0 : this->StartIndex;

  --this->Size;

  if (!Size)
  {
    this->ConditionVariable.notify_all();
  }

  lock.unlock();

  (*f)();
}

//-----------------------------------------------------------------------------
void vtkThreadedCallbackQueue::PrintSelf(ostream& os, vtkIndent indent)
{
  std::lock_guard<std::mutex> lock(this->Mutex);
  os << indent << "Threads: " << this->Threads.size() << std::endl;
  os << indent << "Functions to execute: " << this->Size << std::endl;
  os << indent << "AllocatedSize: " << this->AllocatedSize << std::endl;
}

VTK_ABI_NAMESPACE_END
