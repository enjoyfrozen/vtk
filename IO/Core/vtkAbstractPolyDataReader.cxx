/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
// .SECTION See Also
// vtkOBJReader vtkPLYReader vtkSTLReader
#include "vtkAbstractPolyDataReader.h"

vtkAbstractPolyDataReader::vtkAbstractPolyDataReader()
  : vtkPolyDataAlgorithm()
{
  this->FileName = nullptr;
  this->SetNumberOfInputPorts(0);
}

vtkAbstractPolyDataReader::~vtkAbstractPolyDataReader()
{
  this->SetFileName(nullptr);
}

void vtkAbstractPolyDataReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: " <<
    (this->FileName ? this->FileName : "NONE") << endl;
}
