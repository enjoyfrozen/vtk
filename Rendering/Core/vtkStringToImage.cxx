/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkStringToImage.h"

#include "vtkObjectFactory.h"

//-----------------------------------------------------------------------------
vtkStringToImage::vtkStringToImage()
{
  this->Antialias = true;
  this->ScaleToPowerOfTwo = false;
}

//-----------------------------------------------------------------------------
vtkStringToImage::~vtkStringToImage() = default;

//-----------------------------------------------------------------------------
void vtkStringToImage::SetScaleToPowerOfTwo(bool scale)
{
  if (this->ScaleToPowerOfTwo != scale)
  {
    this->ScaleToPowerOfTwo = scale;
    this->Modified();
  }
}

//-----------------------------------------------------------------------------
void vtkStringToImage::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "ScaleToPowerOfTwo: " << this->ScaleToPowerOfTwo << endl;
}
