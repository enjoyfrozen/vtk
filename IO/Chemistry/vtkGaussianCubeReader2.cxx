// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkGaussianCubeReader2.h"

#include "vtkDataObject.h"
#include "vtkDoubleArray.h"
#include "vtkExecutive.h"
#include "vtkFieldData.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMolecule.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPartitionedDataSet.h"
#include "vtkPeriodicTable.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkSimpleBondPerceiver.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkStructuredGrid.h"
#include "vtkTransform.h"

#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

// Taken from https://physics.nist.gov/cgi-bin/cuu/Value?bohrrada0
static constexpr double BohrToAngstrom = 5.29177210903e-1;

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkGaussianCubeReader2);

//------------------------------------------------------------------------------
vtkGaussianCubeReader2::vtkGaussianCubeReader2()
  : FileName(nullptr)
{
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(2);

  this->GetExecutive()->SetOutputData(1, vtkSmartPointer<vtkPartitionedDataSet>::New());
}

//------------------------------------------------------------------------------
vtkGaussianCubeReader2::~vtkGaussianCubeReader2()
{
  this->SetFileName(nullptr);
}

//------------------------------------------------------------------------------
vtkMolecule* vtkGaussianCubeReader2::GetOutput()
{
  return vtkMolecule::SafeDownCast(this->GetOutputDataObject(0));
}

//------------------------------------------------------------------------------
void vtkGaussianCubeReader2::SetOutput(vtkMolecule* output)
{
  this->GetExecutive()->SetOutputData(0, output);
}

//------------------------------------------------------------------------------
vtkImageData* vtkGaussianCubeReader2::GetGridOutput()
{
  if (this->GetNumberOfOutputPorts() < 2)
  {
    return nullptr;
  }

  // This is an attempt to retain compatibility with old code that expects this function to return
  // a vtkImageData object. In cases where there is only a single data set inside the cube file and
  // that set happens to be of type vtkImageData (which is not guaranteed, but probably the most
  // likely use case), return that. Otherwise, return nullptr.
  vtkPartitionedDataSet* partitioned =
    vtkPartitionedDataSet::SafeDownCast(this->GetOutputDataObject(1));
  if (!partitioned || partitioned->GetNumberOfPartitions() != 1)
  {
    return nullptr;
  }

  return vtkImageData::SafeDownCast(partitioned->GetPartition(0));
}

//------------------------------------------------------------------------------
vtkPartitionedDataSet* vtkGaussianCubeReader2::GetDataOutput()
{
  if (this->GetNumberOfOutputPorts() < 2)
  {
    return nullptr;
  }

  return vtkPartitionedDataSet::SafeDownCast(this->GetOutputDataObject(1));
}

struct HeaderData
{
  std::string name;
  std::size_t nAtoms;
  std::array<double, 3> dataOrigin;
  std::size_t nDatasets;
  std::size_t xDimension;
  std::array<double, 3> xDirection;
  std::size_t yDimension;
  std::array<double, 3> yDirection;
  std::size_t zDimension;
  std::array<double, 3> zDirection;
  bool containsDatasetIDs;
};

bool ParseHeader(std::ifstream& fileStream, HeaderData& data, std::string& errorMessage)
{
  assert(fileStream.is_open());
  assert(static_cast<bool>(fileStream));

  // A cube file starts with two lines of comments/titles. More often than not, the first line
  // acts as a kind of (short) title for the stored data whereas the second acts as a comment or
  // long title.
  std::getline(fileStream, data.name);

  if (!fileStream)
  {
    errorMessage = "Unexpected EOF while trying to read first title/comment line";
    return false;
  }

  if (data.name.empty())
  {
    // If the first line was empty, we instead take the second line as the data set name
    std::getline(fileStream, data.name);
  }
  else
  {
    // Ignore second title/comment
    fileStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }

  if (!fileStream)
  {
    errorMessage = "Unexpected EOF while trying to read second title/comment line";
    return false;
  }

  // This is the default
  data.nDatasets = 1;

  std::string line;
  std::getline(fileStream, line);

  if (!fileStream)
  {
    errorMessage = "Unexpected EOF while trying to read first data line (nAtoms, origin (x,y,z), "
                   "[valuesPerVoxel])";
    return false;
  }

#define VTK_EXTRACT_AND_CHECK(stream, name)                                                        \
  stream >> name;                                                                                  \
  if (!stream)                                                                                     \
  {                                                                                                \
    errorMessage = "Error while trying to extract " #name;                                         \
    return false;                                                                                  \
  }
#define VTK_VECTOR_BOHR_TO_ANGSTROM(name)                                                          \
  name[0] *= BohrToAngstrom;                                                                       \
  name[1] *= BohrToAngstrom;                                                                       \
  name[2] *= BohrToAngstrom;

  int nAtoms = 0;

  std::stringstream lineStream(line);
  VTK_EXTRACT_AND_CHECK(lineStream, nAtoms);
  VTK_EXTRACT_AND_CHECK(lineStream, data.dataOrigin[0]);
  VTK_EXTRACT_AND_CHECK(lineStream, data.dataOrigin[1]);
  VTK_EXTRACT_AND_CHECK(lineStream, data.dataOrigin[2]);

  VTK_VECTOR_BOHR_TO_ANGSTROM(data.dataOrigin);

  if (lineStream)
  {
    // Attempt to read optional values per voxel specification
    lineStream >> data.nDatasets;

    if (!lineStream && !lineStream.eof())
    {
      errorMessage = "Error while trying to extract number of data points per voxel";
      return false;
    }
  }

  data.containsDatasetIDs = nAtoms < 0;
  if (data.containsDatasetIDs)
  {
    nAtoms *= -1;
  }
  data.nAtoms = nAtoms;

  if (!data.containsDatasetIDs && data.nDatasets != 1)
  {
    errorMessage = "The number of values per voxel must be zero if no data set IDs are specified";
    return false;
  }

  VTK_EXTRACT_AND_CHECK(fileStream, data.xDimension);
  VTK_EXTRACT_AND_CHECK(fileStream, data.xDirection[0]);
  VTK_EXTRACT_AND_CHECK(fileStream, data.xDirection[1]);
  VTK_EXTRACT_AND_CHECK(fileStream, data.xDirection[2]);
  VTK_VECTOR_BOHR_TO_ANGSTROM(data.xDirection);

  VTK_EXTRACT_AND_CHECK(fileStream, data.yDimension);
  VTK_EXTRACT_AND_CHECK(fileStream, data.yDirection[0]);
  VTK_EXTRACT_AND_CHECK(fileStream, data.yDirection[1]);
  VTK_EXTRACT_AND_CHECK(fileStream, data.yDirection[2]);
  VTK_VECTOR_BOHR_TO_ANGSTROM(data.yDirection);

  VTK_EXTRACT_AND_CHECK(fileStream, data.zDimension);
  VTK_EXTRACT_AND_CHECK(fileStream, data.zDirection[0]);
  VTK_EXTRACT_AND_CHECK(fileStream, data.zDirection[1]);
  VTK_EXTRACT_AND_CHECK(fileStream, data.zDirection[2]);
  VTK_VECTOR_BOHR_TO_ANGSTROM(data.zDirection);

#undef VTK_EXTRACT_AND_CHECK
#undef VTK_VECTOR_BOHR_TO_ANGSTROM

  return true;
}

double Magnitude(const std::array<double, 3>& vec)
{
  return std::sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
}

bool IsParallel(const std::array<double, 3>& first, const std::array<double, 3>& second)
{
  double scalarProduct = first[0] * second[0] + first[1] * second[1] + first[2] * second[2];
  return std::abs(1 - scalarProduct / (Magnitude(first) * Magnitude(second))) <
    std::numeric_limits<float>::epsilon();
}

bool IsRegular(const std::array<double, 3>& xDir, const std::array<double, 3>& yDir,
  const std::array<double, 3>& zDir)
{
  return IsParallel(xDir, { 1, 0, 0 }) && IsParallel(yDir, { 0, 1, 0 }) &&
    IsParallel(zDir, { 0, 0, 1 });
}

int vtkGaussianCubeReader2::RequestInformation(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* vtkNotUsed(outputVector))
{
  vtkInformation* dataInfo = this->GetExecutive()->GetOutputInformation(1);

  if (!this->FileName)
  {
    return 0;
  }

  std::ifstream fileStream(this->FileName);

  if (!fileStream.is_open())
  {
    vtkErrorMacro("GaussianCubeReader2 error opening file: " << this->FileName);
    return 0;
  }

  HeaderData header;
  std::string error;
  if (!ParseHeader(fileStream, header, error))
  {
    vtkErrorMacro("GaussianCubeReader2 - Error while parsing header: " << error);
    return 0;
  }
  fileStream.close();

  vtkDebugMacro(<< "Grid Size " << header.xDimension << " " << header.yDimension << " "
                << header.zDimension);
  dataInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), 0, header.xDimension - 1, 0,
    header.yDimension - 1, 0, header.zDimension - 1);

  dataInfo->Set(vtkDataObject::ORIGIN(), 0, 0, 0);

  const double xSpacing = Magnitude(header.xDirection);
  const double ySpacing = Magnitude(header.yDirection);
  const double zSpacing = Magnitude(header.zDirection);
  dataInfo->Set(vtkDataObject::SPACING(), xSpacing, ySpacing, zSpacing);

  if (IsRegular(header.xDirection, header.yDirection, header.zDirection))
  {
    dataInfo->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData");
  }
  else
  {
    dataInfo->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkStructuredGrid");
  }

  // TODO: Specify that the scalar datasets will be scalars
  // TODO: Specify the amount of data points per grid point somewhere

  return 1;
}

int vtkGaussianCubeReader2::RequestData(
  vtkInformation*, vtkInformationVector**, vtkInformationVector* outputVector)
{
  vtkMolecule* output = vtkMolecule::SafeDownCast(vtkDataObject::GetData(outputVector));

  if (!output)
  {
    vtkErrorMacro(<< "vtkGaussianCubeReader2 does not have a vtkMolecule "
                     "as output.");
    return 1;
  }
  // Output 0 (the default is the vtkMolecule)
  // Output 1 will be the gridded Image data

  if (!this->FileName)
  {
    return 0;
  }

  std::ifstream fileStream(this->FileName);

  if (!fileStream.is_open())
  {
    vtkErrorMacro("GaussianCubeReader2 error opening file: " << this->FileName);
    return 0;
  }

  HeaderData header;
  std::string error;
  if (!ParseHeader(fileStream, header, error))
  {
    vtkErrorMacro("GaussianCubeReader2 - Error while parsing header: " << error);
    return 0;
  }

  vtkDebugMacro(<< "Grid Size " << header.xDimension << " " << header.yDimension << " "
                << header.zDimension);

  // Parse and construct vtkMolecule
  int atomType;
  std::array<double, 3> coords;
  // Nuclear charge can be different from atomic number if an effective core potential
  // (ECP) has been used for this nucleus
  float nuclearCharge;

  for (std::size_t i = 0; i < header.nAtoms; ++i)
  {
    if (!(fileStream >> atomType >> nuclearCharge >> coords[0] >> coords[1] >> coords[2]))
    {
      vtkErrorMacro("GaussianCubeReader2 error reading file: "
        << this->FileName << " Premature EOF while reading molecule.");
      fileStream.close();
      return 0;
    }
    coords[0] *= BohrToAngstrom;
    coords[1] *= BohrToAngstrom;
    coords[2] *= BohrToAngstrom;
    output->AppendAtom(atomType, coords[0], coords[1], coords[2]);
  }

  // Parse grid data

  const bool isRegular = IsRegular(header.xDirection, header.yDirection, header.zDirection);

  std::vector<int> dataSetIDs;
  dataSetIDs.resize(header.nDatasets);

  if (header.containsDatasetIDs)
  {
    std::size_t numberOfDatasets;
    if (!(fileStream >> numberOfDatasets))
    {
      vtkErrorMacro("GaussianCubeReader error reading file: "
        << this->FileName << " Premature EOF while reading number of dataset IDs.");
      fileStream.close();
      return 0;
    }
    assert(numberOfDatasets == header.nDatasets);

    for (int k = 0; k < numberOfDatasets; k++)
    {
      if (!(fileStream >> dataSetIDs[k]))
      {
        vtkErrorMacro("GaussianCubeReader error reading file: "
          << this->FileName << " Premature EOF while reading data set ID #" << k << "/"
          << numberOfDatasets);
        fileStream.close();
        return 0;
      }
    }
  }
  else
  {
    // Assign incrementing data set IDs (1-based)
    std::iota(dataSetIDs.begin(), dataSetIDs.end(), 1);
  }

  vtkInformation* outInfo = this->GetExecutive()->GetOutputInformation(1);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), 0, header.xDimension - 1, 0,
    header.yDimension - 1, 0, header.zDimension - 1);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),
    outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()), 6);

  const std::size_t nPoints = header.xDimension * header.yDimension * header.zDimension;

  // Create data set topology and geometry
  vtkSmartPointer<vtkDataSet> dataset;
  if (isRegular)
  {
    vtkNew<vtkImageData> image;

    image->SetExtent(outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()));
    image->SetOrigin(header.dataOrigin[0], header.dataOrigin[1], header.dataOrigin[2]);
    image->SetSpacing(
      Magnitude(header.xDirection), Magnitude(header.yDirection), Magnitude(header.zDirection));

    dataset = std::move(image);
  }
  else
  {
    vtkNew<vtkStructuredGrid> grid;

    grid->SetExtent(outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()));

    // Explicitly create a set of points
    vtkNew<vtkPoints> points;
    points->SetNumberOfPoints(nPoints);
    for (std::size_t x = 0; x < header.xDimension; ++x)
    {
      for (std::size_t y = 0; y < header.yDimension; ++y)
      {
        for (std::size_t z = 0; z < header.xDimension; ++z)
        {
          const std::size_t pointID =
            x + header.xDimension * y + header.xDimension * header.yDimension * z;
          const double xCoord =
            x * header.xDirection[0] + y * header.yDirection[0] + z * header.zDirection[0];
          const double yCoord =
            x * header.xDirection[1] + y * header.yDirection[1] + z * header.zDirection[1];
          const double zCoord =
            x * header.xDirection[2] + y * header.yDirection[2] + z * header.zDirection[2];
          points->InsertPoint(pointID, xCoord, yCoord, zCoord);
        }
      }
    }

    grid->SetPoints(std::move(points));

    dataset = std::move(grid);
  }

  // Allocate the data arrays for the individual data sets
  for (std::size_t i = 0; i < header.nDatasets; ++i)
  {
    vtkNew<vtkDoubleArray> dataHolder;
    dataHolder->SetNumberOfComponents(1);
    dataHolder->SetNumberOfValues(nPoints);
    if (header.nDatasets > 1)
    {
      dataHolder->SetName((header.name + " " + std::to_string(dataSetIDs[i])).c_str());
    }
    else
    {
      dataHolder->SetName(header.name.c_str());
    }
    dataset->GetPointData()->AddArray(std::move(dataHolder));
  }

  // Now start parsing the data points and insert them as point data
  vtkPointData* pointData = dataset->GetPointData();
  for (std::size_t x = 0; x < header.xDimension; ++x)
  {
    for (std::size_t y = 0; y < header.yDimension; ++y)
    {
      for (std::size_t z = 0; z < header.zDimension; ++z)
      {
        const std::size_t valueID =
          x + y * header.xDimension + z * header.xDimension * header.yDimension;

        for (std::size_t dset = 0; dset < header.nDatasets; ++dset)
        {
          double value;
          if (!(fileStream >> value))
          {
            vtkErrorMacro("GaussianCubeReader2 error reading file: "
              << this->FileName << " Error while reading data scalar at 0-based index (" << x
              << ", " << y << ", " << z << ", " << dset << ")");
            return 0;
          }

          vtkAbstractArray* array = pointData->GetAbstractArray(dset);
          vtkDoubleArray* floatArray = vtkDoubleArray::SafeDownCast(array);
          assert(floatArray);
          floatArray->InsertValue(valueID, value);
        }
      }
    }
  }

  fileStream.close();

  outInfo->Set(vtkDataObject::DATA_OBJECT(), dataset);

  return 1;
}

int vtkGaussianCubeReader2::FillOutputPortInformation(int port, vtkInformation* info)
{
  if (port == 0)
  {
    return this->Superclass::FillOutputPortInformation(port, info);
  }
  // TODO: Shouldn't we be more concrete?
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkDataObject");
  // info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData");
  return 1;
}

void vtkGaussianCubeReader2::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
VTK_ABI_NAMESPACE_END
