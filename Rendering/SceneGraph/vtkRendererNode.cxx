/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkRendererNode.h"

#include "vtkActor.h"
#include "vtkActorNode.h"
#include "vtkCamera.h"
#include "vtkCameraNode.h"
#include "vtkCollectionIterator.h"
#include "vtkLight.h"
#include "vtkLightCollection.h"
#include "vtkLightNode.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkRendererNode.h"
#include "vtkRenderWindow.h"
#include "vtkViewNodeCollection.h"

//============================================================================
vtkStandardNewMacro(vtkRendererNode);

//----------------------------------------------------------------------------
vtkRendererNode::vtkRendererNode()
{
  this->Size[0] = 0;
  this->Size[1] = 0;
}

//----------------------------------------------------------------------------
vtkRendererNode::~vtkRendererNode()
{
}

//----------------------------------------------------------------------------
void vtkRendererNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkRendererNode::Synchronize(bool prepass)
{
  if (prepass)
  {
    vtkRenderer *mine = vtkRenderer::SafeDownCast
      (this->GetRenderable());
    if (!mine)
    {
      return;
    }
    int *tmp = mine->GetSize();
    this->Size[0] = tmp[0];
    this->Size[1] = tmp[1];
  }
}

//----------------------------------------------------------------------------
void vtkRendererNode::Build(bool prepass)
{
  if (prepass)
  {
    vtkRenderer *mine = vtkRenderer::SafeDownCast
      (this->GetRenderable());
    if (!mine)
    {
      return;
    }

    this->PrepareNodes();
    this->AddMissingNodes(mine->GetLights());
    this->AddMissingNodes(mine->GetActors());
    this->AddMissingNodes(mine->GetVolumes());
    this->AddMissingNode(mine->GetActiveCamera());
    this->RemoveUnusedNodes();
  }
}
