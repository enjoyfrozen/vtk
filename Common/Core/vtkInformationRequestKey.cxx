/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkInformationRequestKey.h"

#include "vtkInformation.h"


//----------------------------------------------------------------------------
vtkInformationRequestKey::vtkInformationRequestKey(const char* name, const char* location):
  vtkInformationKey(name, location)
{
  vtkCommonInformationKeyManager::Register(this);
}

//----------------------------------------------------------------------------
vtkInformationRequestKey::~vtkInformationRequestKey() = default;

//----------------------------------------------------------------------------
void vtkInformationRequestKey::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkInformationRequestKey::Set(vtkInformation* info)
{
  if (info->GetRequest() != this)
  {
    if (info->GetRequest())
    {
      vtkGenericWarningMacro("Setting request key when one is already set. Current request is " << info->GetRequest()->GetName() << " while setting " << this->GetName() << "\n");
    }
    info->SetRequest(this);
    info->Modified(this);
  }
}

//----------------------------------------------------------------------------
int vtkInformationRequestKey::Has(vtkInformation* info)
{
  return (info->GetRequest() == this)?1:0;
}

//----------------------------------------------------------------------------
void vtkInformationRequestKey::Remove(vtkInformation* info)
{
  info->SetRequest(nullptr);
}

//----------------------------------------------------------------------------
void vtkInformationRequestKey::ShallowCopy(vtkInformation* from, vtkInformation* to)
{
  to->SetRequest(from->GetRequest());
}

//----------------------------------------------------------------------------
void vtkInformationRequestKey::Print(ostream& os, vtkInformation* info)
{
  // Print the value.
  if(this->Has(info))
  {
    os << "1\n";
  }
}
