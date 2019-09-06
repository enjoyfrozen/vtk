/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkXMLHierarchicalBoxDataReader.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkXMLHierarchicalBoxDataReader);

//----------------------------------------------------------------------------
vtkXMLHierarchicalBoxDataReader::vtkXMLHierarchicalBoxDataReader() = default;

//----------------------------------------------------------------------------
vtkXMLHierarchicalBoxDataReader::~vtkXMLHierarchicalBoxDataReader() = default;

//----------------------------------------------------------------------------
void vtkXMLHierarchicalBoxDataReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
