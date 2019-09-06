/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkGeneralizedKernel.h"
#include "vtkAbstractPointLocator.h"
#include "vtkObjectFactory.h"
#include "vtkIdList.h"
#include "vtkDoubleArray.h"
#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkMath.h"

//----------------------------------------------------------------------------
vtkGeneralizedKernel::vtkGeneralizedKernel()
{
  this->KernelFootprint = vtkGeneralizedKernel::RADIUS;
  this->Radius = 1.0;
  this->NumberOfPoints = 8;
  this->NormalizeWeights = true;
}


//----------------------------------------------------------------------------
vtkGeneralizedKernel::~vtkGeneralizedKernel() = default;

//----------------------------------------------------------------------------
vtkIdType vtkGeneralizedKernel::
ComputeBasis(double x[3], vtkIdList *pIds, vtkIdType)
{
  if ( this->KernelFootprint == vtkGeneralizedKernel::RADIUS )
  {
    this->Locator->FindPointsWithinRadius(this->Radius, x, pIds);
  }
  else
  {
    this->Locator->FindClosestNPoints(this->NumberOfPoints, x, pIds);
  }

  return pIds->GetNumberOfIds();
}

//----------------------------------------------------------------------------
void vtkGeneralizedKernel::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Kernel Footprint: " << this->GetKernelFootprint() << "\n";
  os << indent << "Radius: " << this->GetRadius() << "\n";
  os << indent << "Number of Points: " << this->GetNumberOfPoints() << "\n";
  os << indent << "Normalize Weights: "
     << (this->GetNormalizeWeights() ? "On\n" : "Off\n");

}
