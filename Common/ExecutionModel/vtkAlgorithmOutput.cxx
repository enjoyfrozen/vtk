/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkAlgorithmOutput.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkAlgorithmOutput);

//----------------------------------------------------------------------------
vtkAlgorithmOutput::vtkAlgorithmOutput()
{
  this->Producer = nullptr;
  this->Index = 0;
}

//----------------------------------------------------------------------------
vtkAlgorithmOutput::~vtkAlgorithmOutput() = default;

//----------------------------------------------------------------------------
void vtkAlgorithmOutput::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  if(this->Producer)
  {
    os << indent << "Producer: " << this->Producer << "\n";
  }
  else
  {
    os << indent << "Producer: (none)\n";
  }
  os << indent << "Index: " << this->Index << "\n";
}

//----------------------------------------------------------------------------
void vtkAlgorithmOutput::SetIndex(int index)
{
  this->Index = index;
}

//----------------------------------------------------------------------------
int vtkAlgorithmOutput::GetIndex()
{
  return this->Index;
}

//----------------------------------------------------------------------------
vtkAlgorithm* vtkAlgorithmOutput::GetProducer()
{
  return this->Producer;
}

//----------------------------------------------------------------------------
void vtkAlgorithmOutput::SetProducer(vtkAlgorithm* producer)
{
  this->Producer = producer;
}
