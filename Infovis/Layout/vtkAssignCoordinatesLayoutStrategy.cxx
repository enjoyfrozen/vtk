/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkAssignCoordinatesLayoutStrategy.h"

#include "vtkAssignCoordinates.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkTree.h"

vtkStandardNewMacro(vtkAssignCoordinatesLayoutStrategy);

vtkAssignCoordinatesLayoutStrategy::vtkAssignCoordinatesLayoutStrategy()
{
  this->AssignCoordinates = vtkSmartPointer<vtkAssignCoordinates>::New();
}

vtkAssignCoordinatesLayoutStrategy::~vtkAssignCoordinatesLayoutStrategy() = default;

void vtkAssignCoordinatesLayoutStrategy::SetXCoordArrayName(const char* name)
{
  this->AssignCoordinates->SetXCoordArrayName(name);
}

const char* vtkAssignCoordinatesLayoutStrategy::GetXCoordArrayName()
{
  return this->AssignCoordinates->GetXCoordArrayName();
}

void vtkAssignCoordinatesLayoutStrategy::SetYCoordArrayName(const char* name)
{
  this->AssignCoordinates->SetYCoordArrayName(name);
}

const char* vtkAssignCoordinatesLayoutStrategy::GetYCoordArrayName()
{
  return this->AssignCoordinates->GetYCoordArrayName();
}

void vtkAssignCoordinatesLayoutStrategy::SetZCoordArrayName(const char* name)
{
  this->AssignCoordinates->SetZCoordArrayName(name);
}

const char* vtkAssignCoordinatesLayoutStrategy::GetZCoordArrayName()
{
  return this->AssignCoordinates->GetZCoordArrayName();
}

void vtkAssignCoordinatesLayoutStrategy::Layout()
{
  this->AssignCoordinates->SetInputData(this->Graph);
  this->AssignCoordinates->Update();
  this->Graph->ShallowCopy(this->AssignCoordinates->GetOutput());
}

void vtkAssignCoordinatesLayoutStrategy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
