/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkInteractorStyleTrackball.h"
#include "vtkMath.h"
#include "vtkPropPicker.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkObjectFactory.h"
#include "vtkCommand.h"

vtkStandardNewMacro(vtkInteractorStyleTrackball);

//----------------------------------------------------------------------------
vtkInteractorStyleTrackball::vtkInteractorStyleTrackball()
{
  vtkWarningMacro("vtkInteractorStyleTrackball will be deprecated in" << endl
                  << "the next release after VTK 4.0. Please use" << endl
                  << "vtkInteractorStyleSwitch instead." );
}

//----------------------------------------------------------------------------
vtkInteractorStyleTrackball::~vtkInteractorStyleTrackball() = default;

//----------------------------------------------------------------------------
void vtkInteractorStyleTrackball::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
