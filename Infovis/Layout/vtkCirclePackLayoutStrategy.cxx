/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkCirclePackLayoutStrategy.h"

#include "vtkTree.h"


vtkCirclePackLayoutStrategy::vtkCirclePackLayoutStrategy() = default;

vtkCirclePackLayoutStrategy::~vtkCirclePackLayoutStrategy() = default;

void vtkCirclePackLayoutStrategy::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);
}
