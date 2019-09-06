/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkVolumeNode.h"

#include "vtkVolume.h"
#include "vtkCellArray.h"
#include "vtkMapper.h"
#include "vtkAbstractVolumeMapper.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolygon.h"
#include "vtkProperty.h"

//============================================================================
vtkStandardNewMacro(vtkVolumeNode);

//----------------------------------------------------------------------------
vtkVolumeNode::vtkVolumeNode()
{
}

//----------------------------------------------------------------------------
vtkVolumeNode::~vtkVolumeNode()
{
}

//----------------------------------------------------------------------------
void vtkVolumeNode::Build(bool prepass)
{
  if (prepass)
  {
    vtkVolume *mine = vtkVolume::SafeDownCast
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
void vtkVolumeNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
