/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkThreadedCallbackQueue.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkThreadedCallbackQueue
 * @brief simple threaded callback queue
 *
 * This callback queue executes pushed functions and functors on threads whose
 * purpose is to wait for a function to be executed. When instantiating
 * this class, no threads are spawned yet. They are spawn upon calling `Start()`.
 * By default, one thread is created by this class, so it is advised to set `NumberOfThreads`.
 * Upon destruction of an instance of this callback queue, remaining unexecuted threads are
 * executed, unless `Stop` has been called and `Start` hasn't since.
 */

#ifndef vtkThreadedCallbackQueue_h
#define vtkThreadedCallbackQueue_h

#include "vtkObject.h"
#include "vtkParallelCoreModule.h" // For export macro

#include <atomic>             // For atomic_bool
#include <condition_variable> // For condition variable
#include <memory>             // For unique_ptr
#include <mutex>              // For mutex
#include <thread>             // For thread
#include <vector>             // For vector

#if !defined(__WRAP__)

VTK_ABI_NAMESPACE_BEGIN

namespace vtkdetail
{
class vtkBaseWorker;
} // namespace vtkdetail

class VTKPARALLELCORE_EXPORT vtkThreadedCallbackQueue : public vtkObject
{
public:
  static constexpr vtkIdType DEFAULT_ALLOCATED_SIZE = 64;

  static vtkThreadedCallbackQueue* New();
  vtkTypeMacro(vtkThreadedCallbackQueue, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkThreadedCallbackQueue();

  /**
   * Any remaining function that was not executed will be executed in the destructor.
   */
  ~vtkThreadedCallbackQueue();

  /**
   * Pushes a function f to be passed args... as arguments.
   * f will be called as soon as a running thread has the occasion to do so, in a FIFO fashion.
   * This method is thread-safe.
   */
  template <class FT, class... ArgsT>
  void Push(FT&& f, ArgsT&&... args);

  /**
   * Sets the number of threads to run. Currently running threads finish their work
   * before resetting them.
   */
  void SetNumberOfThreads(int numberOfThreads);

  /**
   * Returns the number of allocated threads. Note that this method doesn't give any information on
   * whether threads are running or not.
   */
  int GetNumberOfThreads() const { return this->NumberOfThreads; }

  /**
   * Returns true if the queue has started and hasn't been stopped.
   */
  bool IsRunning() const { return !this->Done; }

  /**
   * Stops the threads as soon as they are done with their current task.
   */
  void Stop();

  /**
   * Starts the threads.
   */
  void Start();

private:
  using ValueType = std::unique_ptr<vtkdetail::vtkBaseWorker>;
  using AllocatorTraits = std::allocator_traits<std::allocator<ValueType>>;

  vtkThreadedCallbackQueue(const vtkThreadedCallbackQueue&) = delete;
  void operator=(const vtkThreadedCallbackQueue&) = delete;

  void Sync();
  void Pop();
  void SpawnThreads();

  vtkIdType AllocatedSize = DEFAULT_ALLOCATED_SIZE;
  std::allocator<ValueType> Allocator;
  vtkIdType StartIndex = 0;
  ValueType* Data = nullptr;
  std::mutex Mutex, SpawnMutex;
  std::condition_variable ConditionVariable;
  std::atomic<vtkIdType> Size;
  std::atomic_bool Destroying;
  std::atomic_bool Done;
  std::atomic<int> NumberOfThreads;
  std::vector<std::unique_ptr<std::thread>> Threads;
};

VTK_ABI_NAMESPACE_END

#include "vtkThreadedCallbackQueue.txx"

#endif
#endif
