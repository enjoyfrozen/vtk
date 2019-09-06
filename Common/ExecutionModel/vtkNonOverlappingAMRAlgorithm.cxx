/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkNonOverlappingAMRAlgorithm.h"
#include "vtkObjectFactory.h"
#include "vtkNonOverlappingAMR.h"
#include "vtkInformation.h"
#include "vtkCompositeDataPipeline.h"

vtkStandardNewMacro(vtkNonOverlappingAMRAlgorithm);

//------------------------------------------------------------------------------
vtkNonOverlappingAMRAlgorithm::vtkNonOverlappingAMRAlgorithm()
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);
}

//------------------------------------------------------------------------------
vtkNonOverlappingAMRAlgorithm::~vtkNonOverlappingAMRAlgorithm() = default;

//------------------------------------------------------------------------------
void vtkNonOverlappingAMRAlgorithm::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------------
vtkNonOverlappingAMR* vtkNonOverlappingAMRAlgorithm::GetOutput()
{
  return( this->GetOutput(0) );
}

//------------------------------------------------------------------------------
vtkNonOverlappingAMR* vtkNonOverlappingAMRAlgorithm::GetOutput(int port)
{
  vtkDataObject *output =
      vtkCompositeDataPipeline::SafeDownCast(
          this->GetExecutive())->GetCompositeOutputData(port);
  return( vtkNonOverlappingAMR::SafeDownCast(output) );
}

//------------------------------------------------------------------------------
int vtkNonOverlappingAMRAlgorithm::FillOutputPortInformation(
    int vtkNotUsed(port), vtkInformation* info )
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(),"vtkNonOverlappingAMR");
  return 1;
}

//------------------------------------------------------------------------------
int vtkNonOverlappingAMRAlgorithm::FillInputPortInformation(
    int vtkNotUsed(port), vtkInformation *info )
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(),"vtkNonOverlappingAMR");
  return 1;
}
