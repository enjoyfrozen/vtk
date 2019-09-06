/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkOSPRayVolumeNode.h"

#include "vtkActor.h"
#include "vtkVolume.h"
#include "vtkAbstractVolumeMapper.h"
#include "vtkCompositeDataIterator.h"
#include "vtkCompositeDataSet.h"
#include "vtkDataArray.h"
#include "vtkInformation.h"
#include "vtkInformationIntegerKey.h"
#include "vtkInformationObjectBaseKey.h"
#include "vtkInformationStringKey.h"
#include "vtkMapper.h"
#include "vtkObjectFactory.h"
#include "vtkPiecewiseFunction.h"
#include "vtkPolyData.h"
#include "vtkViewNodeCollection.h"
#include "vtkVolumeProperty.h"

#include "RTWrapper/RTWrapper.h"

//============================================================================
vtkStandardNewMacro(vtkOSPRayVolumeNode);

//----------------------------------------------------------------------------
vtkOSPRayVolumeNode::vtkOSPRayVolumeNode()
{
}

//----------------------------------------------------------------------------
vtkOSPRayVolumeNode::~vtkOSPRayVolumeNode()
{
}

//----------------------------------------------------------------------------
void vtkOSPRayVolumeNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
vtkMTimeType vtkOSPRayVolumeNode::GetMTime()
{
  vtkMTimeType mtime = this->Superclass::GetMTime();
  vtkVolume *vol = (vtkVolume*)this->GetRenderable();
  if (vol->GetMTime() > mtime)
  {
    mtime = vol->GetMTime();
  }
  if (vol->GetProperty())
  {
    mtime = std::max(mtime, vol->GetProperty()->GetMTime());
  }
  vtkAbstractVolumeMapper *mapper = vol->GetMapper();

  if (mapper)
  {
    vtkDataObject *dobj = mapper->GetDataSetInput();
    if (dobj)
    {
      mtime = std::max(mtime, dobj->GetMTime());
    }
    if (mapper->GetMTime() > mtime)
    {
      mtime = mapper->GetMTime();
    }
    if (mapper->GetInformation()->GetMTime() > mtime)
    {
      mtime = mapper->GetInformation()->GetMTime();
    }
  }
  return mtime;
}
