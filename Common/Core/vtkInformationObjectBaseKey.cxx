/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkInformationObjectBaseKey.h"

#include "vtkInformation.h" // For vtkErrorWithObjectMacro


//----------------------------------------------------------------------------
vtkInformationObjectBaseKey
::vtkInformationObjectBaseKey(const char* name, const char* location,
                              const char* requiredClass):
  vtkInformationKey(name, location)
{
  vtkCommonInformationKeyManager::Register(this);

  this->RequiredClass = nullptr;
  this->SetRequiredClass(requiredClass);
}

//----------------------------------------------------------------------------
vtkInformationObjectBaseKey::~vtkInformationObjectBaseKey()
{
  delete [] this->RequiredClass;
}

//----------------------------------------------------------------------------
void vtkInformationObjectBaseKey::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkInformationObjectBaseKey::Set(vtkInformation* info,
                                      vtkObjectBase* value)
{
  if(value && this->RequiredClass && !value->IsA(this->RequiredClass))
  {
    vtkErrorWithObjectMacro(
      info,
      "Cannot store object of type " << value->GetClassName()
      << " with key " << this->Location << "::" << this->Name
      << " which requires objects of type "
      << this->RequiredClass << ".  Removing the key instead.");
    this->SetAsObjectBase(info, nullptr);
    return;
  }
  this->SetAsObjectBase(info, value);
}

//----------------------------------------------------------------------------
vtkObjectBase* vtkInformationObjectBaseKey::Get(vtkInformation* info)
{
  return this->GetAsObjectBase(info);
}

//----------------------------------------------------------------------------
void vtkInformationObjectBaseKey::ShallowCopy(vtkInformation* from,
                                       vtkInformation* to)
{
  this->Set(to, this->Get(from));
}

//----------------------------------------------------------------------------
void vtkInformationObjectBaseKey::Report(vtkInformation* info,
                                         vtkGarbageCollector* collector)
{
  this->ReportAsObjectBase(info, collector);
}
