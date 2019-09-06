/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkAbstractContextBufferId.h"
#include "vtkObjectFactory.h"

vtkAbstractObjectFactoryNewMacro(vtkAbstractContextBufferId)

// ----------------------------------------------------------------------------
vtkAbstractContextBufferId::vtkAbstractContextBufferId()
{
  this->Width=0;
  this->Height=0;
}

// ----------------------------------------------------------------------------
vtkAbstractContextBufferId::~vtkAbstractContextBufferId() = default;

// ----------------------------------------------------------------------------
void vtkAbstractContextBufferId::ReleaseGraphicsResources()
{
}

//-----------------------------------------------------------------------------
void vtkAbstractContextBufferId::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
