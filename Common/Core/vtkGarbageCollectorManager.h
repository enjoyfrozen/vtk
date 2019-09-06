/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkGarbageCollectorManager
 * @brief   Manages the vtkGarbageCollector singleton.
 *
 * vtkGarbageCollectorManager should be included in any translation unit
 * that will use vtkGarbageCollector or that implements the singleton
 * pattern.  It makes sure that the vtkGarbageCollector singleton is created
 * before and destroyed after it is used.
*/

#ifndef vtkGarbageCollectorManager_h
#define vtkGarbageCollectorManager_h

#include "vtkCommonCoreModule.h" // For export macro
#include "vtkSystemIncludes.h"

#include "vtkDebugLeaksManager.h" // DebugLeaks is around longer than
                                  // the garbage collector.

class VTKCOMMONCORE_EXPORT vtkGarbageCollectorManager
{
public:
  vtkGarbageCollectorManager();
  ~vtkGarbageCollectorManager();
private:
  vtkGarbageCollectorManager(const vtkGarbageCollectorManager&);
  vtkGarbageCollectorManager& operator=(
    const vtkGarbageCollectorManager&);
};

// This instance will show up in any translation unit that uses
// vtkGarbageCollector or that has a singleton.  It will make sure
// vtkGarbageCollector is initialized before it is used finalized when
// it is done being used.
static vtkGarbageCollectorManager vtkGarbageCollectorManagerInstance;

#endif
// VTK-HeaderTest-Exclude: vtkGarbageCollectorManager.h
