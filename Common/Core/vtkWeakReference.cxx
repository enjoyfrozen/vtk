/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkWeakReference.h"
#include "vtkWeakPointer.h"
#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkWeakReference)


//----------------------------------------------------------------------------
vtkWeakReference::vtkWeakReference() = default;

//----------------------------------------------------------------------------
vtkWeakReference::~vtkWeakReference() = default;

//----------------------------------------------------------------------------
void vtkWeakReference::Set(vtkObject *object)
{
  this->Object = object;
}

//----------------------------------------------------------------------------
vtkObject* vtkWeakReference::Get()
{
  return this->Object;
}
