/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkFunctionSet.h"


vtkFunctionSet::vtkFunctionSet()
{
  this->NumFuncs = 0;
  this->NumIndepVars = 0;
}

void vtkFunctionSet::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Number of functions: " << this->NumFuncs
     << "\n";
  os << indent << "Number of independent variables: " << this->NumIndepVars
     << "\n";
}
