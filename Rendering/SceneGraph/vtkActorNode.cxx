/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkActorNode.h"

#include "vtkActor.h"
#include "vtkCellArray.h"
#include "vtkMapper.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolygon.h"
#include "vtkProperty.h"

//============================================================================
vtkStandardNewMacro(vtkActorNode);

//----------------------------------------------------------------------------
vtkActorNode::vtkActorNode()
{
}

//----------------------------------------------------------------------------
vtkActorNode::~vtkActorNode()
{
}

//----------------------------------------------------------------------------
void vtkActorNode::Build(bool prepass)
{
  if (prepass)
  {
    vtkActor *mine = vtkActor::SafeDownCast
      (this->GetRenderable());
    if (!mine)
    {
      return;
    }
    if (!mine->GetMapper())
    {
      return;
    }

    this->PrepareNodes();
    this->AddMissingNode(mine->GetMapper());
    this->RemoveUnusedNodes();
  }
}

//----------------------------------------------------------------------------
void vtkActorNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
