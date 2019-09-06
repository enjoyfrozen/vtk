#include "vtkLocalExample.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkLocalExample);

//----------------------------------------------------------------------------
vtkLocalExample::vtkLocalExample() = default;

//----------------------------------------------------------------------------
vtkLocalExample::~vtkLocalExample() = default;

//----------------------------------------------------------------------------
void vtkLocalExample::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
