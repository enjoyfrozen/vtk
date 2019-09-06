/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkDebugLeaksManager.h"
#include "vtkDebugLeaks.h"

// Global optimization performed by MSVC breaks the initialization
// order across translation units enforced by this manager.  Disable
// them for this object file.
#if defined(_MSC_VER)
# pragma optimize("g", off)
#endif

// Must NOT be initialized.  Default initialization to zero is
// necessary.
static unsigned int vtkDebugLeaksManagerCount;

vtkDebugLeaksManager::vtkDebugLeaksManager()
{
  if(++vtkDebugLeaksManagerCount == 1)
  {
    vtkDebugLeaks::ClassInitialize();
  }
}

vtkDebugLeaksManager::~vtkDebugLeaksManager()
{
  if(--vtkDebugLeaksManagerCount == 0)
  {
    vtkDebugLeaks::ClassFinalize();
  }
}

#if defined(_MSC_VER)
# pragma optimize("g", on)
#endif
