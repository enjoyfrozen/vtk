/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkLinearContourLineInterpolator.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkLinearContourLineInterpolator);

//----------------------------------------------------------------------
vtkLinearContourLineInterpolator::vtkLinearContourLineInterpolator() = default;

//----------------------------------------------------------------------
vtkLinearContourLineInterpolator::~vtkLinearContourLineInterpolator() = default;

//----------------------------------------------------------------------
int vtkLinearContourLineInterpolator::InterpolateLine( vtkRenderer *vtkNotUsed(ren),
                                                       vtkContourRepresentation *vtkNotUsed(rep),
                                                       int vtkNotUsed(idx1), int vtkNotUsed(idx2) )
{
  return 1;
}

//----------------------------------------------------------------------
void vtkLinearContourLineInterpolator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

