/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkInformationInformationVectorKey.h"

#include "vtkInformationVector.h"
#include "vtkInformation.h"


//----------------------------------------------------------------------------
vtkInformationInformationVectorKey::vtkInformationInformationVectorKey(const char* name, const char* location):
  vtkInformationKey(name, location)
{
  vtkCommonInformationKeyManager::Register(this);
}

//----------------------------------------------------------------------------
vtkInformationInformationVectorKey::~vtkInformationInformationVectorKey() = default;

//----------------------------------------------------------------------------
void vtkInformationInformationVectorKey::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkInformationInformationVectorKey::Set(vtkInformation* info,
                                             vtkInformationVector* value)
{
  this->SetAsObjectBase(info, value);
}

//----------------------------------------------------------------------------
vtkInformationVector*
vtkInformationInformationVectorKey::Get(vtkInformation* info)
{
  return static_cast<vtkInformationVector *>(this->GetAsObjectBase(info));
}

//----------------------------------------------------------------------------
void vtkInformationInformationVectorKey::ShallowCopy(vtkInformation* from,
                                              vtkInformation* to)
{
  this->Set(to, this->Get(from));
}

//----------------------------------------------------------------------------
void vtkInformationInformationVectorKey::DeepCopy(vtkInformation* from,
                                              vtkInformation* to)
{
  vtkInformationVector *fromVector = this->Get(from);
  vtkInformationVector *toVector = vtkInformationVector::New();
  vtkInformation *toInfo;
  int i;

  for (i = 0; i < fromVector->GetNumberOfInformationObjects(); i++)
  {
    toInfo = vtkInformation::New();
    toInfo->Copy(fromVector->GetInformationObject(i), 1);
    toVector->Append(toInfo);
    toInfo->FastDelete();
  }
  this->Set(to, toVector);
  toVector->FastDelete();
}

//----------------------------------------------------------------------------
void vtkInformationInformationVectorKey::Report(vtkInformation* info,
                                                vtkGarbageCollector* collector)
{
  this->ReportAsObjectBase(info, collector);
}
