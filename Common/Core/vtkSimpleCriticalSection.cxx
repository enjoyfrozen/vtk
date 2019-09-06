/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkSimpleCriticalSection.h"

void vtkSimpleCriticalSection::Init()
{
#ifdef VTK_USE_WIN32_THREADS
  //this->MutexLock = CreateMutex( nullptr, FALSE, nullptr );
  InitializeCriticalSection(&this->CritSec);
#endif

#ifdef VTK_USE_PTHREADS
  pthread_mutex_init(&(this->CritSec), nullptr);
#endif
}


// Destruct the vtkMutexVariable
vtkSimpleCriticalSection::~vtkSimpleCriticalSection()
{
#ifdef VTK_USE_WIN32_THREADS
  //CloseHandle(this->MutexLock);
  DeleteCriticalSection(&this->CritSec);
#endif

#ifdef VTK_USE_PTHREADS
  pthread_mutex_destroy( &this->CritSec);
#endif
}

// Lock the vtkCriticalSection
void vtkSimpleCriticalSection::Lock()
{
#ifdef VTK_USE_WIN32_THREADS
  //WaitForSingleObject( this->MutexLock, INFINITE );
  EnterCriticalSection(&this->CritSec);
#endif

#ifdef VTK_USE_PTHREADS
  pthread_mutex_lock( &this->CritSec);
#endif
}

// Unlock the vtkCriticalSection
void vtkSimpleCriticalSection::Unlock()
{
#ifdef VTK_USE_WIN32_THREADS
  //ReleaseMutex( this->MutexLock );
  LeaveCriticalSection(&this->CritSec);
#endif

#ifdef VTK_USE_PTHREADS
  pthread_mutex_unlock( &this->CritSec);
#endif
}
