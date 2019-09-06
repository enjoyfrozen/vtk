/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkTransmitRectilinearGridPiece.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkTransmitRectilinearGridPiece);

//----------------------------------------------------------------------------
vtkTransmitRectilinearGridPiece::vtkTransmitRectilinearGridPiece() = default;

//----------------------------------------------------------------------------
vtkTransmitRectilinearGridPiece::~vtkTransmitRectilinearGridPiece() = default;

//----------------------------------------------------------------------------
void vtkTransmitRectilinearGridPiece::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
