/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkTransmitStructuredGridPiece.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkTransmitStructuredGridPiece);

//----------------------------------------------------------------------------
vtkTransmitStructuredGridPiece::vtkTransmitStructuredGridPiece() = default;

//----------------------------------------------------------------------------
vtkTransmitStructuredGridPiece::~vtkTransmitStructuredGridPiece() = default;

//----------------------------------------------------------------------------
void vtkTransmitStructuredGridPiece::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
