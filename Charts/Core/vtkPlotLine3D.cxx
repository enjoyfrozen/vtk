/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkPlotLine3D.h"

#include "vtkPen.h"
#include "vtkContext2D.h"
#include "vtkContext3D.h"

#include "vtkObjectFactory.h"

//-----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPlotLine3D);

//-----------------------------------------------------------------------------
vtkPlotLine3D::vtkPlotLine3D() = default;

//-----------------------------------------------------------------------------
vtkPlotLine3D::~vtkPlotLine3D() = default;

//-----------------------------------------------------------------------------
bool vtkPlotLine3D::Paint(vtkContext2D *painter)
{
  // This is where everything should be drawn, or dispatched to other methods.
  vtkDebugMacro(<< "Paint event called in vtkPlotLine3D.");

  if (!this->Visible || this->Points.empty())
  {
    return false;
  }

  // Get the 3D context.
  vtkContext3D *context = painter->GetContext3D();
  if(context == nullptr)
  {
    return false;
  }

  // Draw the line between the points
  context->ApplyPen(this->Pen);
  context->DrawPoly(this->Points[0].GetData(), static_cast< int >(this->Points.size()));

  return this->vtkPlotPoints3D::Paint(painter);
}

//-----------------------------------------------------------------------------
void vtkPlotLine3D::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
