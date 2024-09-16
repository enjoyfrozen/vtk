/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGoldenBallSource.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkGoldenBallSource.h"

#include "vtkCellArray.h"
#include "vtkDelaunay3D.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkUnstructuredGrid.h"
#include "vtkVector.h"
#include "vtkVectorOperators.h"

#include <cmath>

vtkStandardNewMacro(vtkGoldenBallSource);

vtkGoldenBallSource::vtkGoldenBallSource()
  : Radius{0.5}
  , Center{0.0, 0.0, 0.0}
  , Resolution(20)
  , IncludeCenterPoint{0}
  , GenerateNormals{1}
  , OutputPointsPrecision(vtkAlgorithm::SINGLE_PRECISION)
{
  this->SetNumberOfInputPorts(0);
}

int vtkGoldenBallSource::RequestData(
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector),
  vtkInformationVector* outputVector)
{
  // get the info object
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // get the output
  vtkUnstructuredGrid* output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkNew<vtkPoints> coords;
  vtkNew<vtkDoubleArray> normals;
  coords->SetDataType(
    this->OutputPointsPrecision == vtkAlgorithm::DOUBLE_PRECISION ?
    VTK_DOUBLE : VTK_FLOAT);
  coords->SetNumberOfPoints(this->Resolution + (this->IncludeCenterPoint ? 1 : 0));
  if (this->GenerateNormals)
  {
    normals->SetName("normals");
    normals->SetNumberOfComponents(3);
    normals->SetNumberOfTuples(this->Resolution + (this->IncludeCenterPoint ? 1 : 0));
  }

  vtkNew<vtkPolyData> vtmp;
  vtkNew<vtkCellArray> verts;
  if (this->IncludeCenterPoint)
  {
    verts->AllocateExact(this->Resolution + 1, 2 * (this->Resolution + 1));
  }
  else
  {
    verts->AllocateExact(this->Resolution, 2 * this->Resolution);
  }
  vtmp->SetPoints(coords);
  vtmp->SetVerts(verts);

  const double phi = M_PI * (std::sqrt(5.) - 1.);
  const double nm1 = this->Resolution - 1.0;
  const vtkIdType nn = this->Resolution;
  for (vtkIdType ii = 0; ii < nn; ++ii)
  {
    double th = ii * phi;
    double y = (1.0 - 2. * (ii / nm1));
    double rr = this->Radius * std::sqrt(1. - y * y);
    double x = rr * std::cos(th);
    double z = rr * std::sin(th);
    coords->SetPoint(ii, x, this->Radius * y, z);
    if (this->GenerateNormals)
    {
      vtkVector3d norm(x, this->Radius * y, z);
      norm = norm - vtkVector3d(this->Center);
      normals->SetTuple(ii, norm.Normalized().GetData());
    }
    verts->InsertNextCell(1, &ii);
  }
  if (this->IncludeCenterPoint)
  {
    coords->SetPoint(nn, this->Center);
    verts->InsertNextCell(1, &nn);
    if (this->GenerateNormals)
    {
      normals->SetTuple3(nn, 0, 0, 0);
    }
  }

  vtkNew<vtkDelaunay3D> delaunay;
  delaunay->SetInputDataObject(vtmp);
  delaunay->Update();

  output->ShallowCopy(delaunay->GetOutputDataObject(0));
  if (this->GenerateNormals)
  {
    output->GetPointData()->SetNormals(normals);
  }
  return 1;
}

void vtkGoldenBallSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Radius: " << this->Radius << "\n";
  os << indent << "Center: ("
      << this->Center[0] << ", " << this->Center[1] << ", " << this->Center[2] << ")\n";
  os << indent << "Resolution: " << this->Resolution << "\n";
  os << indent << "IncludeCenterPoint: " << (this->IncludeCenterPoint ? "ON" : "OFF") << "\n";
  os << indent << "Output Points Precision: " << this->OutputPointsPrecision
     << "\n";
}
