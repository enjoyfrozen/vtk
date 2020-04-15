/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMeshReader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkMeshReader.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkFieldData.h"
#include "vtkFloatArray.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkUnstructuredGrid.h"

#include <array>
#include <fstream>

vtkStandardNewMacro(vtkMeshReader);

vtkMeshReader::vtkMeshReader()
{
  this->SetNumberOfInputPorts(0);
}

vtkMeshReader::~vtkMeshReader()
{
}

void vtkMeshReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "FileName: " << (this->FileName ? this->FileName : "(null)") << "\n";
}

int vtkMeshReader::CanReadFile(const char* fname)
{
  if (!fname || !fname[0])
  {
    return 0;
  }
  if (strstr(fname, ".mesh") != nullptr)
  {
    return 1;
  }
  return 0;
}

// int vtkMeshReader::RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*)
// {
// }

int vtkMeshReader::RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector* outVec)
{
  if (!this->FileName)
  {
    vtkErrorMacro("FileName has to be specified!");
    return 0;
  }
  vtkInformation* outInfo = outVec->GetInformationObject(0);
  auto output = vtkMultiBlockDataSet::GetData(outInfo);
  vtkNew<vtkPoints> points;
  vtkNew<vtkUnstructuredGrid> mesh;
  vtkNew<vtkPolyData> surf;

  std::ifstream file(this->FileName);
  if (!file.good())
  {
    vtkErrorMacro("Could not open " << this->FileName << ".");
    return 0;
  }
  vtkIdType npts;
  file >> npts;
  if (!file.good() || npts <= 0)
  {
    vtkErrorMacro("Could not read num points from " << this->FileName << ".");
    return 0;
  }

  points->SetNumberOfPoints(npts);
  std::array<double, 3> coord;
  for (vtkIdType ii = 0; ii < npts; ++ii)
  {
    file >> coord[0] >> coord[1] >> coord[2];
    points->SetPoint(ii, coord.data());
  }

  vtkIdType ntet;
  file >> ntet;
  if (!file.good() || ntet <= 0)
  {
    vtkErrorMacro("Could not read num cells from " << this->FileName << ".");
    return 0;
  }
  mesh->AllocateExact(ntet, 4 * ntet);
  mesh->SetPoints(points);
  for (vtkIdType ii = 0; ii < ntet; ++ii)
  {
    int groupId;
    std::array<vtkIdType, 4> conn;
    file >> groupId >> conn[0] >> conn[1] >> conn[2] >> conn[3];
    for (int jj = 0; jj < 4; ++jj)
    {
      conn[jj] -= 1; // VTK uses 0-based indexing.
    }
    mesh->InsertNextCell(VTK_TETRA, 4, conn.data());
  }
  vtkIdType ntri;
  file >> ntri;
  if (!file.good() || ntri <= 0)
  {
    vtkErrorMacro("Could not read num cells from " << this->FileName << ".");
    return 0;
  }
  surf->AllocateExact(ntri, 3 * ntri);
  surf->SetPoints(points);
  for (vtkIdType ii = 0; ii < ntri; ++ii)
  {
    int groupId;
    std::array<vtkIdType, 3> conn;
    file >> groupId >> conn[0] >> conn[1] >> conn[2];
    for (int jj = 0; jj < 3; ++jj)
    {
      conn[jj] -= 1; // VTK uses 0-based indexing.
    }
    surf->InsertNextCell(VTK_TRIANGLE, 3, conn.data());
  }
  output->SetNumberOfBlocks(2);
  output->SetBlock(0, mesh);
  output->SetBlock(1, surf);
  return 1;
}
