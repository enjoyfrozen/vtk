/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkGaussianRandomSequence.h"


// ----------------------------------------------------------------------------
vtkGaussianRandomSequence::vtkGaussianRandomSequence() = default;

// ----------------------------------------------------------------------------
vtkGaussianRandomSequence::~vtkGaussianRandomSequence() = default;

// ----------------------------------------------------------------------------
double vtkGaussianRandomSequence::GetScaledValue(double mean,
                                                 double standardDeviation)
{
  return mean+standardDeviation*this->GetValue();
}

// ----------------------------------------------------------------------------
void vtkGaussianRandomSequence::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
