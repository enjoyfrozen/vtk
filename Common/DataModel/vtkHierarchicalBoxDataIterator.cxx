/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkHierarchicalBoxDataIterator.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkHierarchicalBoxDataIterator);

vtkHierarchicalBoxDataIterator::vtkHierarchicalBoxDataIterator() = default;

//------------------------------------------------------------------------------
vtkHierarchicalBoxDataIterator::~vtkHierarchicalBoxDataIterator() = default;

//------------------------------------------------------------------------------
void vtkHierarchicalBoxDataIterator::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

