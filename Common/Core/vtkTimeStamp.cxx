/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkTimeStamp.h"

#include "vtkObjectFactory.h"
#include "vtkWindows.h"

// We use the Schwarz Counter idiom to make sure that GlobalTimeStamp
// is initialized before any other class uses it.

#include "vtkAtomicTypes.h"

//-------------------------------------------------------------------------
vtkTimeStamp* vtkTimeStamp::New()
{
  // If the factory was unable to create the object, then create it here.
  return new vtkTimeStamp;
}

//-------------------------------------------------------------------------
void vtkTimeStamp::Modified()
{
#if defined(VTK_USE_64BIT_TIMESTAMPS) || VTK_SIZEOF_VOID_P == 8
  static vtkAtomicUInt64 GlobalTimeStamp(0);
#else
  static vtkAtomicUInt32 GlobalTimeStamp(0);
#endif
  this->ModifiedTime = (vtkMTimeType)++GlobalTimeStamp;
}
