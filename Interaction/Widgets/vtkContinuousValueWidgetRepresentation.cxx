/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/


#include "vtkContinuousValueWidgetRepresentation.h"
#include "vtkCommand.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkMath.h"
#include "vtkEvent.h"
#include "vtkInteractorObserver.h"
#include "vtkWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"



//----------------------------------------------------------------------
vtkContinuousValueWidgetRepresentation::vtkContinuousValueWidgetRepresentation()
{
  this->Value = 0;
}

//----------------------------------------------------------------------
vtkContinuousValueWidgetRepresentation::~vtkContinuousValueWidgetRepresentation() = default;

//----------------------------------------------------------------------
void vtkContinuousValueWidgetRepresentation::PlaceWidget(double *vtkNotUsed(bds[6]))
{
  // Position the handles at the end of the lines
  this->BuildRepresentation();
}

void vtkContinuousValueWidgetRepresentation::SetValue(double)
{

}

//----------------------------------------------------------------------
void vtkContinuousValueWidgetRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  //Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Value: " << this->GetValue() << "\n";
}
