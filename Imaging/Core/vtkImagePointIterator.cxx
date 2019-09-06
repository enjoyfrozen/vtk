/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkImagePointIterator.h"
#include "vtkImageData.h"

//----------------------------------------------------------------------------
vtkImagePointIterator::vtkImagePointIterator()
{
  this->Origin[0] = 0.0;
  this->Origin[1] = 0.0;
  this->Origin[2] = 0.0;
  this->Spacing[0] = 1.0;
  this->Spacing[1] = 1.0;
  this->Spacing[2] = 1.0;
  this->Position[0] = 0.0;
  this->Position[1] = 0.0;
  this->Position[2] = 0.0;
}

//----------------------------------------------------------------------------
vtkImagePointIterator::vtkImagePointIterator(
  vtkImageData *image, const int extent[6], vtkImageStencilData *stencil,
  vtkAlgorithm *algorithm, int threadId)
  : vtkImagePointDataIterator(image, extent, stencil, algorithm, threadId)
{
  image->GetOrigin(this->Origin);
  image->GetSpacing(this->Spacing);
  this->UpdatePosition();
}

//----------------------------------------------------------------------------
void vtkImagePointIterator::Initialize(
  vtkImageData *image, const int extent[6], vtkImageStencilData *stencil,
  vtkAlgorithm *algorithm, int threadId)
{
  this->vtkImagePointDataIterator::Initialize(
    image, extent, stencil, algorithm, threadId);
  image->GetOrigin(this->Origin);
  image->GetSpacing(this->Spacing);
  this->UpdatePosition();
}
