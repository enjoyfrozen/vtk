/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkTransmitImageDataPiece.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkTransmitImageDataPiece);

//----------------------------------------------------------------------------
vtkTransmitImageDataPiece::vtkTransmitImageDataPiece() = default;

//----------------------------------------------------------------------------
vtkTransmitImageDataPiece::~vtkTransmitImageDataPiece() = default;

//----------------------------------------------------------------------------
void vtkTransmitImageDataPiece::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
