/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkDummyGPUInfoList.h"

#include "vtkGPUInfoListArray.h"

#include "vtkObjectFactory.h"
#include <cassert>

vtkStandardNewMacro(vtkDummyGPUInfoList);

// ----------------------------------------------------------------------------
// Description:
// Build the list of vtkInfoGPU if not done yet.
// \post probed: IsProbed()
void vtkDummyGPUInfoList::Probe()
{
  if(!this->Probed)
  {
    this->Probed=true;
    this->Array=new vtkGPUInfoListArray;
    this->Array->v.resize(0); // no GPU.
  }
  assert("post: probed" && this->IsProbed());
}

// ----------------------------------------------------------------------------
vtkDummyGPUInfoList::vtkDummyGPUInfoList() = default;

// ----------------------------------------------------------------------------
vtkDummyGPUInfoList::~vtkDummyGPUInfoList() = default;

// ----------------------------------------------------------------------------
void vtkDummyGPUInfoList::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
