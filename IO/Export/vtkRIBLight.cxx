/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkRIBLight.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkRIBLight);

vtkRIBLight::vtkRIBLight ()
{
  this->Shadows = 0;
  // create a vtkLight that can be rendered
  this->Light = vtkLight::New ();
}

vtkRIBLight::~vtkRIBLight()
{
  if (this->Light)
  {
    this->Light->Delete();
  }
}

void vtkRIBLight::Render(vtkRenderer *ren, int index)
{
  int ref;

  // Copy this light's ivars into the light to be rendered
  ref = this->Light->GetReferenceCount();
  this->Light->DeepCopy(this);
  //this->Light->SetDeleteMethod(nullptr);
  this->Light->SetReferenceCount(ref);

  // Render the light
  this->Light->Render (ren, index);
}

void vtkRIBLight::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);


  os << indent << "Shadows: " << (this->Shadows ? "On\n" : "Off\n");
}

