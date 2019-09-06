/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkCameraWidget.h"
#include "vtkCameraRepresentation.h"
#include "vtkCameraInterpolator.h"
#include "vtkCallbackCommand.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkCameraWidget);

//-------------------------------------------------------------------------
vtkCameraWidget::vtkCameraWidget() = default;

//-------------------------------------------------------------------------
vtkCameraWidget::~vtkCameraWidget() = default;

//----------------------------------------------------------------------
void vtkCameraWidget::CreateDefaultRepresentation()
{
  if ( ! this->WidgetRep )
  {
    this->WidgetRep = vtkCameraRepresentation::New();
  }
}

//-------------------------------------------------------------------------
void vtkCameraWidget::SelectRegion(double eventPos[2])
{
  if ( ! this->WidgetRep )
  {
    return;
  }

  double x = eventPos[0];
  if ( x < 0.3333 )
  {
    reinterpret_cast<vtkCameraRepresentation*>(this->WidgetRep)->AddCameraToPath();
  }
  else if ( x < 0.666667 )
  {
    reinterpret_cast<vtkCameraRepresentation*>(this->WidgetRep)->
      AnimatePath(this->Interactor);
  }
  else if ( x < 1.0 )
  {
    reinterpret_cast<vtkCameraRepresentation*>(this->WidgetRep)->InitializePath();
  }

  this->Superclass::SelectRegion(eventPos);
}

//-------------------------------------------------------------------------
void vtkCameraWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}
