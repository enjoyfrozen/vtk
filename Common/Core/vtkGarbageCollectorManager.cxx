/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkGarbageCollectorManager.h"

#include "vtkGarbageCollector.h"

// Must NOT be initialized.  Default initialization to zero is
// necessary.
static unsigned int vtkGarbageCollectorManagerCount;

vtkGarbageCollectorManager::vtkGarbageCollectorManager()
{
  if(++vtkGarbageCollectorManagerCount == 1)
  {
    vtkGarbageCollector::ClassInitialize();
  }
}

vtkGarbageCollectorManager::~vtkGarbageCollectorManager()
{
  if(--vtkGarbageCollectorManagerCount == 0)
  {
    vtkGarbageCollector::ClassFinalize();
  }
}
