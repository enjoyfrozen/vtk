/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkRendererDelegate.h"


vtkRendererDelegate::vtkRendererDelegate()
{
  this->Used=false;
}

vtkRendererDelegate::~vtkRendererDelegate() = default;

void vtkRendererDelegate::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Used: ";
  if(this->Used)
  {
      os<<"On";
  }
  else
  {
      os<<"Off";
  }
  os<<endl;
}
