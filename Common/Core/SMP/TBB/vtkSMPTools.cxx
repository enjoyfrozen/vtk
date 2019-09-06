/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkSMPTools.h"

#include "vtkCriticalSection.h"

#ifdef _MSC_VER
#  pragma push_macro("__TBB_NO_IMPLICIT_LINKAGE")
#  define __TBB_NO_IMPLICIT_LINKAGE 1
#endif

#include <tbb/task_scheduler_init.h>

#ifdef _MSC_VER
#  pragma pop_macro("__TBB_NO_IMPLICIT_LINKAGE")
#endif

struct vtkSMPToolsInit
{
  tbb::task_scheduler_init Init;

  vtkSMPToolsInit(int numThreads) : Init(numThreads)
  {
  }
};

static bool vtkSMPToolsInitialized = 0;
static int vtkTBBNumSpecifiedThreads = 0;
static vtkSimpleCriticalSection vtkSMPToolsCS;

//--------------------------------------------------------------------------------
void vtkSMPTools::Initialize(int numThreads)
{
  vtkSMPToolsCS.Lock();
  if (!vtkSMPToolsInitialized)
  {
    // If numThreads <= 0, don't create a task_scheduler_init
    // and let TBB do the default thing.
    if (numThreads > 0)
    {
      static vtkSMPToolsInit aInit(numThreads);
      vtkTBBNumSpecifiedThreads = numThreads;
    }
    vtkSMPToolsInitialized = true;
  }
  vtkSMPToolsCS.Unlock();
}

//--------------------------------------------------------------------------------
int vtkSMPTools::GetEstimatedNumberOfThreads()
{
  return vtkTBBNumSpecifiedThreads ? vtkTBBNumSpecifiedThreads
    : tbb::task_scheduler_init::default_num_threads();
}
