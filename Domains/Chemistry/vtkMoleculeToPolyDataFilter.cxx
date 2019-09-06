/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
#include "vtkMoleculeToPolyDataFilter.h"

#include "vtkInformation.h"
#include "vtkMolecule.h"


//----------------------------------------------------------------------------
vtkMoleculeToPolyDataFilter::vtkMoleculeToPolyDataFilter()
{
  this->SetNumberOfInputPorts(1);
}

//----------------------------------------------------------------------------
vtkMoleculeToPolyDataFilter::~vtkMoleculeToPolyDataFilter() = default;

//----------------------------------------------------------------------------
vtkMolecule * vtkMoleculeToPolyDataFilter::GetInput()
{
  return vtkMolecule::SafeDownCast(this->Superclass::GetInput(0));
}

//----------------------------------------------------------------------------
int vtkMoleculeToPolyDataFilter::FillInputPortInformation(int port,
                                                          vtkInformation* info)
{
  if(!this->Superclass::FillInputPortInformation(port, info))
  {
    return 0;
  }
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkMolecule");
  return 1;
}

//----------------------------------------------------------------------------
void vtkMoleculeToPolyDataFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
