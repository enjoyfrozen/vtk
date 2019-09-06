/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkAbstractGridConnectivity.h"

vtkAbstractGridConnectivity::vtkAbstractGridConnectivity()
{
  this->NumberOfGrids                = 0;
  this->NumberOfGhostLayers          = 0;
  this->AllocatedGhostDataStructures = false;
}

//------------------------------------------------------------------------------
vtkAbstractGridConnectivity::~vtkAbstractGridConnectivity()
{
  this->DeAllocateUserRegisterDataStructures();
  this->DeAllocateInternalDataStructures();
}

//------------------------------------------------------------------------------
void vtkAbstractGridConnectivity::PrintSelf(std::ostream &os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
  os << "NumberOfGrids: " << this->NumberOfGrids << std::endl;
  os << "NumberOfGhostLayers: " << this->NumberOfGhostLayers << std::endl;
}
