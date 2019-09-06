/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkPCellSizeFilter.h"

#include "vtkCommunicator.h"
#include "vtkMultiProcessController.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkPCellSizeFilter);

//-----------------------------------------------------------------------------
vtkPCellSizeFilter::vtkPCellSizeFilter()
{
}

//-----------------------------------------------------------------------------
vtkPCellSizeFilter::~vtkPCellSizeFilter()
{
}

//-----------------------------------------------------------------------------
void vtkPCellSizeFilter::ComputeGlobalSum(double sum[4])
{
  vtkMultiProcessController* controller = vtkMultiProcessController::GetGlobalController();
  if (controller->GetNumberOfProcesses() > 1)
  {
    double globalSum[4];
    controller->AllReduce(sum, globalSum, 4, vtkCommunicator::SUM_OP);
    for (int i=0;i<4;i++)
    {
      sum[i] = globalSum[i];
    }
  }
}
