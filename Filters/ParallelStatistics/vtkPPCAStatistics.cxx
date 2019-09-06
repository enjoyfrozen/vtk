/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
#include "vtkToolkits.h"

#include "vtkPPCAStatistics.h"

#include "vtkAbstractArray.h"
#include "vtkCommunicator.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkMultiProcessController.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPOrderStatistics.h"
#include "vtkTable.h"
#include "vtkPMultiCorrelativeStatistics.h"

vtkStandardNewMacro(vtkPPCAStatistics);
vtkCxxSetObjectMacro(vtkPPCAStatistics, Controller, vtkMultiProcessController);
//-----------------------------------------------------------------------------
vtkPPCAStatistics::vtkPPCAStatistics()
{
  this->Controller = 0;
  this->SetController( vtkMultiProcessController::GetGlobalController() );
}

//-----------------------------------------------------------------------------
vtkPPCAStatistics::~vtkPPCAStatistics()
{
  this->SetController( 0 );
}

//-----------------------------------------------------------------------------
void vtkPPCAStatistics::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Controller: " << this->Controller << endl;
}

// ----------------------------------------------------------------------
void vtkPPCAStatistics::Learn( vtkTable* inData,
                               vtkTable* inParameters,
                               vtkMultiBlockDataSet* outMeta )
{
  if ( ! outMeta )
  {
    return;
  }

  // First calculate correlative statistics on local data set
  this->Superclass::Learn( inData, inParameters, outMeta );

  // Get a hold of the (sparse) covariance matrix
  vtkTable* sparseCov = vtkTable::SafeDownCast( outMeta->GetBlock( 0 ) );
  if ( ! sparseCov )
  {
    return;
  }

  if ( !this->MedianAbsoluteDeviation )
  {
    vtkPMultiCorrelativeStatistics::GatherStatistics( this->Controller, sparseCov );
  }
}

// ----------------------------------------------------------------------
void vtkPPCAStatistics::Test( vtkTable* inData,
                              vtkMultiBlockDataSet* inMeta,
                              vtkTable* outMeta )
{
  if ( this->Controller->GetNumberOfProcesses() > 1 )
  {
    vtkWarningMacro( "Parallel PCA: Hypothesis testing not implemented for more than 1 process." );
    return;
  }

  this->Superclass::Test( inData, inMeta, outMeta );
}

// ----------------------------------------------------------------------
vtkOrderStatistics* vtkPPCAStatistics::CreateOrderStatisticsInstance()
{
  return vtkPOrderStatistics::New();
}
