/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkSMPTools.h"

// Simple implementation that runs everything sequentially.

//--------------------------------------------------------------------------------
void vtkSMPTools::Initialize(int)
{
}

int vtkSMPTools::GetEstimatedNumberOfThreads()
{
  return 1;
}
