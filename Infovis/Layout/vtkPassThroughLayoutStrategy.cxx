/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkPassThroughLayoutStrategy.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkPassThroughLayoutStrategy);

// ----------------------------------------------------------------------

vtkPassThroughLayoutStrategy::vtkPassThroughLayoutStrategy() = default;

// ----------------------------------------------------------------------

vtkPassThroughLayoutStrategy::~vtkPassThroughLayoutStrategy() = default;

// ----------------------------------------------------------------------
// Set the graph that will be laid out
void vtkPassThroughLayoutStrategy::Initialize()
{
}

// ----------------------------------------------------------------------

// Simple graph layout method
void vtkPassThroughLayoutStrategy::Layout()
{
}

void vtkPassThroughLayoutStrategy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
