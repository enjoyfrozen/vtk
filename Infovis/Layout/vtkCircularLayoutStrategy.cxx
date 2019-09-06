/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkCircularLayoutStrategy.h"

#include "vtkGraph.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"

vtkStandardNewMacro(vtkCircularLayoutStrategy);

vtkCircularLayoutStrategy::vtkCircularLayoutStrategy() = default;

vtkCircularLayoutStrategy::~vtkCircularLayoutStrategy() = default;

void vtkCircularLayoutStrategy::Layout()
{
  vtkPoints* points = vtkPoints::New();
  vtkIdType numVerts = this->Graph->GetNumberOfVertices();
  points->SetNumberOfPoints(numVerts);
  for (vtkIdType i = 0; i < numVerts; i++)
  {
    double x = cos(2.0*vtkMath::Pi()*i/numVerts);
    double y = sin(2.0*vtkMath::Pi()*i/numVerts);
    points->SetPoint(i, x, y, 0);
  }
  this->Graph->SetPoints(points);
  points->Delete();
}

void vtkCircularLayoutStrategy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

