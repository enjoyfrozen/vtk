/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkImageItem.h"

#include "vtkContext2D.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"

//-----------------------------------------------------------------------------
vtkStandardNewMacro(vtkImageItem);

//-----------------------------------------------------------------------------
vtkCxxSetObjectMacro(vtkImageItem, Image, vtkImageData);

//-----------------------------------------------------------------------------
vtkImageItem::vtkImageItem()
{
  this->Position[0] = this->Position[1] = 0;
  this->Image = nullptr;
}

//-----------------------------------------------------------------------------
vtkImageItem::~vtkImageItem()
{
  this->SetImage(nullptr);
}

//-----------------------------------------------------------------------------
bool vtkImageItem::Paint(vtkContext2D *painter)
{
  if (this->Image)
  {
    // Draw our image in the bottom left corner of the item
    painter->DrawImage(this->Position[0], this->Position[1], this->Image);
  }
  return true;
}

//-----------------------------------------------------------------------------
void vtkImageItem::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
