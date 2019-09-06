/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkReebGraphSimplificationMetric.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkReebGraphSimplificationMetric);

//----------------------------------------------------------------------------
vtkReebGraphSimplificationMetric::vtkReebGraphSimplificationMetric()
{
  this->LowerBound = 0;
  this->UpperBound = 1;
}

//----------------------------------------------------------------------------
vtkReebGraphSimplificationMetric::~vtkReebGraphSimplificationMetric() = default;

//----------------------------------------------------------------------------
void vtkReebGraphSimplificationMetric::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Upper Bound: " << this->UpperBound << "\n";
  os << indent << "Lower Bound: " << this->LowerBound << "\n";
}

//----------------------------------------------------------------------------
double vtkReebGraphSimplificationMetric::ComputeMetric(vtkDataSet* vtkNotUsed(mesh), vtkDataArray* vtkNotUsed(scalarField), vtkIdType vtkNotUsed(startCriticalPoint), vtkAbstractArray* vtkNotUsed(vertexList), vtkIdType vtkNotUsed(endCriticalPoint))
{
  printf("too bad, wrong code\n");
  return 0;
}
