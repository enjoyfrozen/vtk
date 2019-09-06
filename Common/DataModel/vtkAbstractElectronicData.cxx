/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkAbstractElectronicData.h"

//----------------------------------------------------------------------------
vtkAbstractElectronicData::vtkAbstractElectronicData()
  : Padding(0.0)
{
}

//----------------------------------------------------------------------------
vtkAbstractElectronicData::~vtkAbstractElectronicData() = default;

//----------------------------------------------------------------------------
void vtkAbstractElectronicData::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Padding: " << this->Padding << "\n";
}

//----------------------------------------------------------------------------
void vtkAbstractElectronicData::DeepCopy(vtkDataObject *obj)
{
  vtkAbstractElectronicData *aed =
      vtkAbstractElectronicData::SafeDownCast(obj);
  if (!aed)
  {
    vtkErrorMacro("Can only deep copy from vtkAbstractElectronicData "
                  "or subclass.");
    return;
  }

  // Call superclass
  this->Superclass::DeepCopy(aed);

  // Copy ivars
  this->Padding = aed->Padding;
}
