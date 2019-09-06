/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkXMLMultiGroupDataReader.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkXMLMultiGroupDataReader);
//----------------------------------------------------------------------------
vtkXMLMultiGroupDataReader::vtkXMLMultiGroupDataReader() = default;

//----------------------------------------------------------------------------
vtkXMLMultiGroupDataReader::~vtkXMLMultiGroupDataReader() = default;

//----------------------------------------------------------------------------
void vtkXMLMultiGroupDataReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

