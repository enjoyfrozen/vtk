/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkInformationInformationKey.h"

#include "vtkInformation.h"


//----------------------------------------------------------------------------
vtkInformationInformationKey::vtkInformationInformationKey(const char* name, const char* location):
  vtkInformationKey(name, location)
{
  vtkCommonInformationKeyManager::Register(this);
}

//----------------------------------------------------------------------------
vtkInformationInformationKey::~vtkInformationInformationKey() = default;

//----------------------------------------------------------------------------
void vtkInformationInformationKey::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkInformationInformationKey::Set(vtkInformation* info,
                                      vtkInformation* value)
{
  this->SetAsObjectBase(info, value);
}

//----------------------------------------------------------------------------
vtkInformation* vtkInformationInformationKey::Get(vtkInformation* info)
{
  return static_cast<vtkInformation *>(this->GetAsObjectBase(info));
}

//----------------------------------------------------------------------------
void vtkInformationInformationKey::ShallowCopy(vtkInformation* from,
                                        vtkInformation* to)
{
  this->Set(to, this->Get(from));
}

//----------------------------------------------------------------------------
void vtkInformationInformationKey::DeepCopy(vtkInformation* from,
                                        vtkInformation* to)
{
  vtkInformation *toInfo = vtkInformation::New();
  toInfo->Copy(this->Get(from), 1);
  this->Set(to, toInfo);
  toInfo->Delete();
}
