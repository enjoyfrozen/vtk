/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkPassThroughEdgeStrategy.h"

#include "vtkCellArray.h"
#include "vtkDirectedGraph.h"
#include "vtkEdgeListIterator.h"
#include "vtkGraph.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkSmartPointer.h"

#include <utility>
#include <vector>
#include <map>

vtkStandardNewMacro(vtkPassThroughEdgeStrategy);

vtkPassThroughEdgeStrategy::vtkPassThroughEdgeStrategy() = default;

vtkPassThroughEdgeStrategy::~vtkPassThroughEdgeStrategy() = default;

void vtkPassThroughEdgeStrategy::Layout()
{
}

void vtkPassThroughEdgeStrategy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
