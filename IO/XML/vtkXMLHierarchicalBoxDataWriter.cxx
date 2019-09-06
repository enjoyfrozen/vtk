/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkXMLHierarchicalBoxDataWriter.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkXMLHierarchicalBoxDataWriter);
//----------------------------------------------------------------------------
vtkXMLHierarchicalBoxDataWriter::vtkXMLHierarchicalBoxDataWriter() = default;

//----------------------------------------------------------------------------
vtkXMLHierarchicalBoxDataWriter::~vtkXMLHierarchicalBoxDataWriter() = default;

//----------------------------------------------------------------------------
void vtkXMLHierarchicalBoxDataWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
