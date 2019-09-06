/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkCameraNode.h"
#include "vtkCamera.h"
#include "vtkObjectFactory.h"

//============================================================================
vtkStandardNewMacro(vtkCameraNode);

//----------------------------------------------------------------------------
vtkCameraNode::vtkCameraNode()
{
}

//----------------------------------------------------------------------------
vtkCameraNode::~vtkCameraNode()
{
}

//----------------------------------------------------------------------------
void vtkCameraNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
