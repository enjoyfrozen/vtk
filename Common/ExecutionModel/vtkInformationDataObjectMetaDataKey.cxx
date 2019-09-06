/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkInformationDataObjectMetaDataKey.h"

#include "vtkInformation.h"
#include "vtkStreamingDemandDrivenPipeline.h"

//----------------------------------------------------------------------------
vtkInformationDataObjectMetaDataKey::vtkInformationDataObjectMetaDataKey(const char* name, const char* location) :
  vtkInformationDataObjectKey(name, location)
{
}

//----------------------------------------------------------------------------
vtkInformationDataObjectMetaDataKey::~vtkInformationDataObjectMetaDataKey() = default;

//----------------------------------------------------------------------------
void vtkInformationDataObjectMetaDataKey::CopyDefaultInformation(
  vtkInformation* request,
  vtkInformation* fromInfo,
  vtkInformation* toInfo)
{
  if (request->Has(vtkStreamingDemandDrivenPipeline::REQUEST_INFORMATION()))
  {
    this->ShallowCopy(fromInfo, toInfo);
  }
}

//----------------------------------------------------------------------------
void vtkInformationDataObjectMetaDataKey::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
