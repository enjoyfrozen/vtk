/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkVoronoiKernel.h"
#include "vtkAbstractPointLocator.h"

//----------------------------------------------------------------------------
vtkVoronoiKernel::vtkVoronoiKernel()
{
}


//----------------------------------------------------------------------------
vtkVoronoiKernel::~vtkVoronoiKernel()
{
}


//----------------------------------------------------------------------------
void vtkVoronoiKernel::
vtkIdType ComputeWeights(double x[3], vtkIdList *pIds,vtkDoubleArray *weights)
{
}


//----------------------------------------------------------------------------
void vtkVoronoiKernel::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}
