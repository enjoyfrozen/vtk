/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkVolumeMapperNode.h"

#include "vtkActor.h"
#include "vtkObjectFactory.h"
#include "vtkCellArray.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolygon.h"
#include "vtkProperty.h"

//============================================================================
vtkStandardNewMacro(vtkVolumeMapperNode);

//----------------------------------------------------------------------------
vtkVolumeMapperNode::vtkVolumeMapperNode()
{
}

//----------------------------------------------------------------------------
vtkVolumeMapperNode::~vtkVolumeMapperNode()
{
}

//----------------------------------------------------------------------------
void vtkVolumeMapperNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
