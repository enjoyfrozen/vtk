/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkSimpleCriticalSection
 * @brief   Critical section locking class
 *
 * vtkCriticalSection allows the locking of variables which are accessed
 * through different threads.  This header file also defines
 * vtkSimpleCriticalSection which is not a subclass of vtkObject.
 * The API is identical to that of vtkMutexLock, and the behavior is
 * identical as well, except on Windows 9x/NT platforms. The only difference
 * on these platforms is that vtkMutexLock is more flexible, in that
 * it works across processes as well as across threads, but also costs
 * more, in that it evokes a 600-cycle x86 ring transition. The
 * vtkCriticalSection provides a higher-performance equivalent (on
 * Windows) but won't work across processes. Since it is unclear how,
 * in vtk, an object at the vtk level can be shared across processes
 * in the first place, one should use vtkCriticalSection unless one has
 * a very good reason to use vtkMutexLock. If higher-performance equivalents
 * for non-Windows platforms (Irix, SunOS, etc) are discovered, they
 * should replace the implementations in this class
*/

#ifndef vtkSimpleCriticalSection_h
#define vtkSimpleCriticalSection_h

#include "vtkCommonCoreModule.h" // For export macro
#include "vtkSystemIncludes.h"

#if defined(VTK_USE_PTHREADS)
#include <pthread.h> // Needed for pthreads implementation of mutex
typedef pthread_mutex_t vtkCritSecType;
#endif

#ifdef VTK_USE_WIN32_THREADS
# include "vtkWindows.h" // Needed for win32 implementation of mutex
typedef CRITICAL_SECTION vtkCritSecType;
#endif

#ifndef VTK_USE_PTHREADS
#ifndef VTK_USE_WIN32_THREADS
typedef int vtkCritSecType;
#endif
#endif

// Critical Section object that is not a vtkObject.
class VTKCOMMONCORE_EXPORT vtkSimpleCriticalSection
{
public:
  // Default cstor
  vtkSimpleCriticalSection()
  {
    this->Init();
  }
  // Construct object locked if isLocked is different from 0
  vtkSimpleCriticalSection(int isLocked)
  {
    this->Init();
    if(isLocked)
    {
      this->Lock();
    }
  }
  // Destructor
  virtual ~vtkSimpleCriticalSection();

  void Init();

  /**
   * Lock the vtkCriticalSection
   */
  void Lock();

  /**
   * Unlock the vtkCriticalSection
   */
  void Unlock();

protected:
  vtkCritSecType   CritSec;

private:
  vtkSimpleCriticalSection(const vtkSimpleCriticalSection& other) = delete;
  vtkSimpleCriticalSection& operator=(const vtkSimpleCriticalSection& rhs) = delete;
};

#endif
// VTK-HeaderTest-Exclude: vtkSimpleCriticalSection.h
