/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkObjectFactoryCollection.h"

#include "vtkDebugLeaks.h"
#include "vtkObjectFactory.h"

vtkObjectFactoryCollection* vtkObjectFactoryCollection::New()
{
  // Don't use the object factory macros. Creating an object factory here
  // will cause an infinite loop.
  vtkObjectFactoryCollection *ret = new vtkObjectFactoryCollection;
  ret->InitializeObjectBase();
  return ret;
}
