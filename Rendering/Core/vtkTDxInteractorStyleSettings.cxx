/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkTDxInteractorStyleSettings.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkTDxInteractorStyleSettings);

// ----------------------------------------------------------------------------
vtkTDxInteractorStyleSettings::vtkTDxInteractorStyleSettings()
{
  this->AngleSensitivity=1.0;
  this->UseRotationX=true;
  this->UseRotationY=true;
  this->UseRotationZ=true;
  this->TranslationXSensitivity=1.0;
  this->TranslationYSensitivity=1.0;
  this->TranslationZSensitivity=1.0;
}

// ----------------------------------------------------------------------------
vtkTDxInteractorStyleSettings::~vtkTDxInteractorStyleSettings() = default;


//----------------------------------------------------------------------------
void vtkTDxInteractorStyleSettings::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "AngleSensitivity: " << this->AngleSensitivity << endl;
  os << indent << "UseRotationX: " << this->UseRotationX << endl;
  os << indent << "UseRotationY: " << this->UseRotationY << endl;
  os << indent << "UseRotationZ: " << this->UseRotationZ << endl;

  os << indent << "TranslationXSensitivity: " <<
    this->TranslationXSensitivity << endl;
  os << indent << "TranslationYSensitivity: " <<
    this->TranslationYSensitivity << endl;
  os << indent << "TranslationZSensitivity: " <<
    this->TranslationZSensitivity << endl;
}
