/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkInformationIntegerRequestKey.h"

#include "vtkInformation.h"
#include "vtkStreamingDemandDrivenPipeline.h"

//----------------------------------------------------------------------------
vtkInformationIntegerRequestKey::vtkInformationIntegerRequestKey(const char* name, const char* location) :
  vtkInformationIntegerKey(name, location)
{
  this->DataKey = nullptr;
}

//----------------------------------------------------------------------------
vtkInformationIntegerRequestKey::~vtkInformationIntegerRequestKey() = default;

//----------------------------------------------------------------------------
void vtkInformationIntegerRequestKey::CopyDefaultInformation(
  vtkInformation* request,
  vtkInformation* fromInfo,
  vtkInformation* toInfo)
{
  if (request->Has(vtkStreamingDemandDrivenPipeline::REQUEST_UPDATE_EXTENT()))
  {
    this->ShallowCopy(fromInfo, toInfo);
  }
}

//----------------------------------------------------------------------------
bool vtkInformationIntegerRequestKey::NeedToExecute(vtkInformation* pipelineInfo,
                                                    vtkInformation* dobjInfo)
{
  if (!dobjInfo->Has(this->DataKey) ||
    dobjInfo->Get(this->DataKey) != pipelineInfo->Get(this))
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
void vtkInformationIntegerRequestKey::StoreMetaData(vtkInformation*,
                                                    vtkInformation* pipelineInfo,
                                                    vtkInformation* dobjInfo)
{
  dobjInfo->Set(this->DataKey, pipelineInfo->Get(this));
}

//----------------------------------------------------------------------------
void vtkInformationIntegerRequestKey::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
