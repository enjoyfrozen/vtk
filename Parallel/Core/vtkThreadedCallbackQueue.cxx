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
// in this instance. It is set in the constructor of vtkThreadedCallbackQueue using a little
// template meta-programming depending on the type of Controller at construction.
//
// Note that we are handing over the calls to Start, Stop and SetNumberOfThreads to the same class
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

//=============================================================================
// The ControllerHelper allows to decide at compile time whether
// to execute code relevant to the main callback queue, or to its controller.
// We template this helper over the static type of the Controller, which is different between
// vtkThreadedCallbackQueue and vtkThreadedCallbackQueue::vtkInternalController
//
// Note: we need to write the default implementation for
// vtkThreadedCallbackQueue::vtkInternalController::vtkInternalController because
// the compiler mistakes this instance with the constructor of
// vtkThreadedCallbackQueue::vtkInternalController.
template<class ControllerT>
struct vtkThreadedCallbackQueue::ControllerHelper
{
  using ControllerType = typename std::remove_reference<ControllerT>::type;
  using ControllerPointer = vtkSmartPointer<ControllerType>;

  template<class FT>
  static void Execute(ControllerPointer&, FT&& command) { command(); }

  static ControllerPointer CreateController() { return nullptr; }
};

//=============================================================================
template<>
struct vtkThreadedCallbackQueue::ControllerHelper<vtkThreadedCallbackQueue::vtkInternalController>
{
  using ControllerType = vtkThreadedCallbackQueue::vtkInternalController;
  using ControllerPointer = vtkSmartPointer<ControllerType>;

  template<class FT>
  static void Execute(ControllerPointer& controller, FT&& command)
  {
    controller->Push(std::forward<FT>(command));
  }

  static ControllerPointer CreateController()
  {
    ControllerPointer controller = ControllerPointer::New();
    controller->SetNumberOfThreads(1);
    controller->Start();
    return controller;
  }
};

//-----------------------------------------------------------------------------
vtkThreadedCallbackQueue::vtkThreadedCallbackQueue()
  : Empty(true)
  , Destroying(false)
  , Running(false)
  , NumberOfThreads(1)
  , Threads(NumberOfThreads)
  , Controller(ControllerHandler::CreateController())
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
  ControllerHandler::Execute(this->Controller, [this, &numberOfThreads] {
      if (static_cast<std::size_t>(numberOfThreads) == this->Threads.size())
      {
        return;
      }

      this->Stop();
      this->NumberOfThreads = numberOfThreads;
      this->Threads.resize(numberOfThreads);
    });
}

//-----------------------------------------------------------------------------
void vtkThreadedCallbackQueue::Stop()
{
  ControllerHandler::Execute(this->Controller, [this] {
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
    });
}

//-----------------------------------------------------------------------------
void vtkThreadedCallbackQueue::Start()
{
  ControllerHandler::Execute(this->Controller, [this] {
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
    });
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
