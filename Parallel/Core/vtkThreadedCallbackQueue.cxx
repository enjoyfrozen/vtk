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

//=============================================================================
// This class does the same things as vktThreadedCallbackQueue.
// The difference between this class and its parent is that Controller is set to nullptr
// in this instance. It is set in the constructor of vtkThreadedCallbackQueu` using a little
// template meta-programming depending on the type of Controller at construction.
//
// Note that we are controller the calls to Start, Stop and SetNumberOfThreads by the same class
// it is controlling. It works well because we are spawning only one thread in this instance,
// and the Controller's calls to Start, Stop and SetNumberOfThreads are executed serially because
// the Controller of the Controller is nullptr.
class vtkThreadedCallbackQueue::vtkInternalController : public vtkThreadedCallbackQueue
{
public:
  static vtkInternalController* New();
  vtkTypeMacro(vtkInternalController, vtkThreadedCallbackQueue);

  vtkInternalController() = default;
  ~vtkInternalController() = default;

private:
  vtkInternalController(const vtkInternalController&) = delete;
  void operator=(const vtkInternalController&) = delete;
};

vtkStandardNewMacro(vtkThreadedCallbackQueue::vtkInternalController);

//-----------------------------------------------------------------------------
// Instantiates the Controller depending on its static type.
// The Controller of vtkThreadedCallbackQueue is of static type
// vtkThreadedCallbackQueue::vtkInternalController, whereas the Controller of
// vtkThreadedCallbackQueue::vtkInternalController is of static type
// vtkThreadedCallbackQueue::vtkInternalController::vtkInternalController.
//
// Note: we need to write the default implementation for
// vtkThreadedCallbackQueue::vtkInternalController::vtkInternalController because
// the compiler mistakes this instance with the constructor of
// vtkThreadedCallbackQueue::vtkInternalController.
template <class ControllerT>
vtkSmartPointer<vtkThreadedCallbackQueue::vtkInternalController>
vtkThreadedCallbackQueue ::CreateController()
{
  return nullptr;
}

//-----------------------------------------------------------------------------
// Specializing for creating the base Controller.
template <>
vtkSmartPointer<vtkThreadedCallbackQueue::vtkInternalController>
vtkThreadedCallbackQueue ::CreateController<vtkThreadedCallbackQueue::vtkInternalController>()
{
  auto controller = vtkSmartPointer<vtkThreadedCallbackQueue::vtkInternalController>::New();
  controller->SetNumberOfThreads(1);
  controller->Start();
  return controller;
}

//-----------------------------------------------------------------------------
vtkThreadedCallbackQueue::vtkThreadedCallbackQueue()
  : Empty(true)
  , Destroying(false)
  , Running(false)
  , NumberOfThreads(1)
  , Threads(NumberOfThreads)
  , Controller(CreateController<decltype(*Controller)>())
{
}

//-----------------------------------------------------------------------------
vtkThreadedCallbackQueue::~vtkThreadedCallbackQueue()
{
  // By deleting the controller, we ensure that all the Start(), Stop()
  // and SetNumberOfThreads() calls are terminated and that we have a sane state
  // of our queue.
  this->Controller = nullptr;

  if (this->Running)
  {
    {
      std::lock_guard<std::mutex> lock(this->Mutex);
      this->Destroying = true;
      this->ConditionVariable.notify_all();
    }
    this->Sync();
  }
}

//-----------------------------------------------------------------------------
void vtkThreadedCallbackQueue::SetNumberOfThreads(int numberOfThreads)
{
  // Runs asynchronously if there is a Controller.
  auto setNumberOfThreads = [this, &numberOfThreads] {
    if (static_cast<std::size_t>(numberOfThreads) == this->Threads.size())
    {
      return;
    }

    this->Stop();
    this->NumberOfThreads = numberOfThreads;
    this->Threads.resize(numberOfThreads);
  };

  this->Controller ? this->Controller->Push(setNumberOfThreads) : setNumberOfThreads();
}

//-----------------------------------------------------------------------------
void vtkThreadedCallbackQueue::Stop()
{
  // Runs asynchronously if there is a Controller.
  auto stop = [this] {
    if (!this->Running)
    {
      return;
    }

    {
      std::lock_guard<std::mutex> lock(this->Mutex);
      this->Running = false;
      this->ConditionVariable.notify_all();
    }

    this->Sync();
  };

  this->Controller ? this->Controller->Push(stop) : stop();
}

//-----------------------------------------------------------------------------
void vtkThreadedCallbackQueue::Start()
{
  // Runs asynchronously if there is a Controller.
  auto start = [this] {
    if (this->Running)
    {
      return;
    }

    this->Running = true;

    for (std::thread& thread : this->Threads)
    {
      thread = std::thread([this] {
        while (this->Running && (!this->Destroying || !this->Empty))
        {
          this->Pop();
        }
      });
    }
  };

  this->Controller ? this->Controller->Push(start) : start();
}

//-----------------------------------------------------------------------------
void vtkThreadedCallbackQueue::Sync()
{
  for (std::thread& thread : this->Threads)
  {
    thread.join();
  }
}

//-----------------------------------------------------------------------------
void vtkThreadedCallbackQueue::Pop()
{
  std::unique_lock<std::mutex> lock(this->Mutex);

  if (!this->Destroying && this->Workers.empty())
  {
    this->ConditionVariable.wait(
      lock, [this] { return !this->Workers.empty() || !this->Running || this->Destroying; });
  }

  if (this->Workers.empty())
  {
    return;
  }

  WorkerPointer worker = std::move(this->Workers.front());
  this->Workers.pop();
  this->Empty = this->Workers.empty();

  lock.unlock();

  (*worker)();
}

//-----------------------------------------------------------------------------
void vtkThreadedCallbackQueue::PrintSelf(ostream& os, vtkIndent indent)
{
  std::lock_guard<std::mutex> lock(this->Mutex);
  os << indent << "Threads: " << this->Threads.size() << std::endl;
  os << indent << "Functions to execute: " << this->Workers.size() << std::endl;
}

VTK_ABI_NAMESPACE_END
