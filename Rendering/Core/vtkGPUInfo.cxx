/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkGPUInfo.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkGPUInfo);

// ----------------------------------------------------------------------------
vtkGPUInfo::vtkGPUInfo()
{
  this->DedicatedVideoMemory=0;
  this->DedicatedSystemMemory=0;
  this->SharedSystemMemory=0;
}

// ----------------------------------------------------------------------------
vtkGPUInfo::~vtkGPUInfo() = default;

// ----------------------------------------------------------------------------
void vtkGPUInfo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Dedicated Video Memory in bytes: " << this->DedicatedVideoMemory
     << endl;
  os << indent << "Dedicated System Memory in bytes: " << this->DedicatedSystemMemory
     << endl;
  os << indent << "Shared System Memory in bytes: " << this->SharedSystemMemory
     << endl;
}
