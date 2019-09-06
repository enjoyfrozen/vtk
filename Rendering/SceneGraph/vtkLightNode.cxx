/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkLightNode.h"
#include "vtkObjectFactory.h"

#include "vtkLight.h"

//============================================================================
vtkStandardNewMacro(vtkLightNode);

//----------------------------------------------------------------------------
vtkLightNode::vtkLightNode()
{
}

//----------------------------------------------------------------------------
vtkLightNode::~vtkLightNode()
{
}

//----------------------------------------------------------------------------
void vtkLightNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
