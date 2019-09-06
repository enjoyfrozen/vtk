/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkFrameBufferObjectBase.h"

#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
vtkFrameBufferObjectBase::vtkFrameBufferObjectBase() = default;

//----------------------------------------------------------------------------
vtkFrameBufferObjectBase::~vtkFrameBufferObjectBase() = default;

// ----------------------------------------------------------------------------
void vtkFrameBufferObjectBase::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
