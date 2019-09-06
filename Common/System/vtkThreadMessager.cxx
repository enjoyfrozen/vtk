/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkThreadMessager.h"

#include "vtkObjectFactory.h"

#ifdef VTK_USE_WIN32_THREADS
# include "vtkWindows.h"
#endif

vtkStandardNewMacro(vtkThreadMessager);

vtkThreadMessager::vtkThreadMessager()
{
#ifdef VTK_USE_WIN32_THREADS
  this->WSignal = CreateEvent(0, FALSE, FALSE, 0);
#elif defined(VTK_USE_PTHREADS)
  pthread_cond_init(&this->PSignal, nullptr);
  pthread_mutex_init(&this->Mutex, nullptr);
  pthread_mutex_lock(&this->Mutex);
#endif
}

vtkThreadMessager::~vtkThreadMessager()
{
#ifdef VTK_USE_WIN32_THREADS
  CloseHandle(this->WSignal);
#elif defined(VTK_USE_PTHREADS)
  pthread_mutex_unlock(&this->Mutex);
  pthread_mutex_destroy(&this->Mutex);
  pthread_cond_destroy(&this->PSignal);
#endif
}

void vtkThreadMessager::WaitForMessage()
{
#ifdef VTK_USE_WIN32_THREADS
  WaitForSingleObject( this->WSignal, INFINITE );
#elif defined(VTK_USE_PTHREADS)
  pthread_cond_wait(&this->PSignal, &this->Mutex);
#endif
}

//----------------------------------------------------------------------------
void vtkThreadMessager::SendWakeMessage()
{
#ifdef VTK_USE_WIN32_THREADS
  SetEvent( this->WSignal );
#elif defined(VTK_USE_PTHREADS)
  pthread_cond_broadcast(&this->PSignal);
#endif
}

void vtkThreadMessager::EnableWaitForReceiver()
{
#if defined(VTK_USE_PTHREADS)
  pthread_mutex_lock(&this->Mutex);
#endif
}

void vtkThreadMessager::WaitForReceiver()
{
#if defined(VTK_USE_PTHREADS)
  pthread_mutex_lock(&this->Mutex);
#endif
}

void vtkThreadMessager::DisableWaitForReceiver()
{
#if defined(VTK_USE_PTHREADS)
  pthread_mutex_unlock(&this->Mutex);
#endif
}

void vtkThreadMessager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
