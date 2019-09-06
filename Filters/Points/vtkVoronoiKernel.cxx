/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkVoronoiKernel.h"
#include "vtkAbstractPointLocator.h"
#include "vtkObjectFactory.h"
#include "vtkIdList.h"
#include "vtkDoubleArray.h"

vtkStandardNewMacro(vtkVoronoiKernel);

//----------------------------------------------------------------------------
vtkVoronoiKernel::vtkVoronoiKernel() = default;


//----------------------------------------------------------------------------
vtkVoronoiKernel::~vtkVoronoiKernel() = default;

//----------------------------------------------------------------------------
vtkIdType vtkVoronoiKernel::
ComputeBasis(double x[3], vtkIdList *pIds, vtkIdType)
{
  pIds->SetNumberOfIds(1);
  vtkIdType pId = this->Locator->FindClosestPoint(x);
  pIds->SetId(0,pId);

  return 1;
}

//----------------------------------------------------------------------------
vtkIdType vtkVoronoiKernel::
ComputeWeights(double*, vtkIdList*, vtkDoubleArray *weights)
{
  weights->SetNumberOfTuples(1);
  weights->SetValue(0,1.0);

  return 1;
}

//----------------------------------------------------------------------------
void vtkVoronoiKernel::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}
