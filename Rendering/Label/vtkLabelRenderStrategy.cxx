/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkLabelRenderStrategy.h"

#include "vtkRenderer.h"
#include "vtkTextProperty.h"

vtkCxxSetObjectMacro(vtkLabelRenderStrategy, Renderer, vtkRenderer);
vtkCxxSetObjectMacro(vtkLabelRenderStrategy, DefaultTextProperty, vtkTextProperty);

//----------------------------------------------------------------------------
vtkLabelRenderStrategy::vtkLabelRenderStrategy()
{
  this->Renderer = nullptr;
  this->DefaultTextProperty = vtkTextProperty::New();
}

//----------------------------------------------------------------------------
vtkLabelRenderStrategy::~vtkLabelRenderStrategy()
{
  this->SetRenderer(nullptr);
  this->SetDefaultTextProperty(nullptr);
}

//----------------------------------------------------------------------------
void vtkLabelRenderStrategy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Renderer: " << this->Renderer << endl;
  os << indent << "DefaultTextProperty: " << this->DefaultTextProperty << endl;
}
