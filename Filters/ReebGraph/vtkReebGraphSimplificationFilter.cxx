/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkReebGraphSimplificationFilter.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkReebGraph.h"

vtkStandardNewMacro(vtkReebGraphSimplificationFilter);

//----------------------------------------------------------------------------
vtkReebGraphSimplificationFilter::vtkReebGraphSimplificationFilter()
{
  this->SetNumberOfInputPorts(1);
  this->SimplificationThreshold = 0;
  this->SimplificationMetric = nullptr;
}

//----------------------------------------------------------------------------
vtkReebGraphSimplificationFilter::~vtkReebGraphSimplificationFilter()
{
}

//----------------------------------------------------------------------------
void vtkReebGraphSimplificationFilter::SetSimplificationMetric(
  vtkReebGraphSimplificationMetric *simplificationMetric)
{
  if (simplificationMetric != this->SimplificationMetric)
  {
    this->SimplificationMetric = simplificationMetric;
    this->Modified();
  }
}

//----------------------------------------------------------------------------
int vtkReebGraphSimplificationFilter::FillInputPortInformation(
  int portNumber, vtkInformation *info)
{
  if(!portNumber){
    info->Remove(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkReebGraph");
  }
  return 1;
}

//----------------------------------------------------------------------------
int vtkReebGraphSimplificationFilter::FillOutputPortInformation(
  int, vtkInformation *info)
{
  info->Set(vtkDirectedGraph::DATA_TYPE_NAME(), "vtkReebGraph");
  return 1;
}

//----------------------------------------------------------------------------
void vtkReebGraphSimplificationFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Simplification Threshold: " << this->SimplificationThreshold << "\n";
}

//----------------------------------------------------------------------------
vtkReebGraph* vtkReebGraphSimplificationFilter::GetOutput()
{
  return vtkReebGraph::SafeDownCast(this->GetOutputDataObject(0));
}

//----------------------------------------------------------------------------
int vtkReebGraphSimplificationFilter::RequestData( vtkInformation* vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{

  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);

  vtkReebGraph *input = vtkReebGraph::SafeDownCast(
    inInfo->Get(vtkReebGraph::DATA_OBJECT()));

  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkReebGraph *output = vtkReebGraph::SafeDownCast(
    outInfo->Get(vtkReebGraph::DATA_OBJECT()));

  output->DeepCopy(input);
  output->Simplify(this->SimplificationThreshold, this->SimplificationMetric);

  return 1;
}
