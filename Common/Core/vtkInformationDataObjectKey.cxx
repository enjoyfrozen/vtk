/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkInformationDataObjectKey.h"

#if defined(vtkCommonDataModel_ENABLED)
# include "../DataModel/vtkDataObject.h"
#endif


//----------------------------------------------------------------------------
vtkInformationDataObjectKey::vtkInformationDataObjectKey(const char* name, const char* location):
  vtkInformationKey(name, location)
{
  vtkCommonInformationKeyManager::Register(this);
}

//----------------------------------------------------------------------------
vtkInformationDataObjectKey::~vtkInformationDataObjectKey() = default;

//----------------------------------------------------------------------------
void vtkInformationDataObjectKey::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkInformationDataObjectKey::Set(vtkInformation* info,
                                      vtkDataObject* value)
{
#if defined(vtkCommonDataModel_ENABLED)
  this->SetAsObjectBase(info, value);
#endif
}

//----------------------------------------------------------------------------
vtkDataObject* vtkInformationDataObjectKey::Get(vtkInformation* info)
{
#if defined(vtkCommonDataModel_ENABLED)
  return static_cast<vtkDataObject *>(this->GetAsObjectBase(info));
#else
  return 0;
#endif
}

//----------------------------------------------------------------------------
void vtkInformationDataObjectKey::ShallowCopy(vtkInformation* from,
                                              vtkInformation* to)
{
  this->Set(to, this->Get(from));
}

//----------------------------------------------------------------------------
void vtkInformationDataObjectKey::Report(vtkInformation* info,
                                         vtkGarbageCollector* collector)
{
  this->ReportAsObjectBase(info, collector);
}
