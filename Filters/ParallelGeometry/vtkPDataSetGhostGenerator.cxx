/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkPDataSetGhostGenerator.h"

#include "vtkMultiProcessController.h"
#include "vtkMultiBlockDataSet.h"

#include <cassert>

vtkPDataSetGhostGenerator::vtkPDataSetGhostGenerator()
{
  this->Initialized = false;
  this->Controller  = vtkMultiProcessController::GetGlobalController();
}

//------------------------------------------------------------------------------
vtkPDataSetGhostGenerator::~vtkPDataSetGhostGenerator()
{
}

//------------------------------------------------------------------------------
void vtkPDataSetGhostGenerator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << "Controller: " << this->Controller << std::endl;
}

//------------------------------------------------------------------------------
void vtkPDataSetGhostGenerator::Initialize()
{
  assert("pre: Multi-process controller is nullptr" && (this->Controller != nullptr));
  this->Rank = this->Controller->GetLocalProcessId();
  this->Initialized = true;
}

//------------------------------------------------------------------------------
void vtkPDataSetGhostGenerator::Barrier()
{
  assert("pre: Multi-process controller is nullptr" && (this->Controller != nullptr));
  assert("pre: Instance has not been initialized!" && this->Initialized);
  this->Controller->Barrier();
}


