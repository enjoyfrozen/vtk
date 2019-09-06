/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkObjectFactory.h"
#include "vtkIncrementalPointLocator.h"


vtkIncrementalPointLocator::vtkIncrementalPointLocator() = default;

vtkIncrementalPointLocator::~vtkIncrementalPointLocator() = default;

void vtkIncrementalPointLocator::PrintSelf( ostream & os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

