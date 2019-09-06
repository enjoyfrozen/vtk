/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkXMLPHierarchicalBoxDataWriter.h"

#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkXMLPHierarchicalBoxDataWriter);

//----------------------------------------------------------------------------
vtkXMLPHierarchicalBoxDataWriter::vtkXMLPHierarchicalBoxDataWriter() = default;

//----------------------------------------------------------------------------
vtkXMLPHierarchicalBoxDataWriter::~vtkXMLPHierarchicalBoxDataWriter() = default;

//----------------------------------------------------------------------------
void vtkXMLPHierarchicalBoxDataWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
