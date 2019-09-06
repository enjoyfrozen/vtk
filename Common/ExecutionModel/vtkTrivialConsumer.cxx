/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkTrivialConsumer.h"

#include "vtkInformation.h"
#include "vtkObjectFactory.h"
#include "vtkDataObject.h"

vtkStandardNewMacro(vtkTrivialConsumer);

//----------------------------------------------------------------------------
vtkTrivialConsumer::vtkTrivialConsumer()
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(0);
}

//----------------------------------------------------------------------------
vtkTrivialConsumer::~vtkTrivialConsumer() = default;

//----------------------------------------------------------------------------
void vtkTrivialConsumer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
int vtkTrivialConsumer::FillInputPortInformation(int, vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkDataObject");
  return 1;
}

//----------------------------------------------------------------------------
int vtkTrivialConsumer::FillOutputPortInformation(int, vtkInformation*)
{
  return 1;
}
