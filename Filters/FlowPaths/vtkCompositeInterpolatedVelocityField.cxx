/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkCompositeInterpolatedVelocityField.h"

#include "vtkMath.h"
#include "vtkDataSet.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"
#include "vtkGenericCell.h"
#include "vtkObjectFactory.h"


vtkCompositeInterpolatedVelocityField::vtkCompositeInterpolatedVelocityField()
{
  this->LastDataSetIndex = 0;
  this->DataSets = new vtkCompositeInterpolatedVelocityFieldDataSetsType;
}


vtkCompositeInterpolatedVelocityField::~vtkCompositeInterpolatedVelocityField()
{
  delete this->DataSets;
  this->DataSets = nullptr;
}

void vtkCompositeInterpolatedVelocityField::PrintSelf( ostream & os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );

  os << indent << "DataSets: "           << this->DataSets         << endl;
  os << indent << "Last Dataset Index: " << this->LastDataSetIndex << endl;
}
