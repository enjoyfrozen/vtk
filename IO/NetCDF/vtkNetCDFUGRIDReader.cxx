/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkNetCDFUGRIDReader.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkNetCDFUGRIDReader.h"

#include "vtkArrayDispatch.h"
#include "vtkCellData.h"
#include "vtkCharArray.h"
#include "vtkDataArraySelection.h"
#include "vtkDataObject.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationDoubleKey.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkLongArray.h"
#include "vtkLongLongArray.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkShortArray.h"
#include "vtkSignedCharArray.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnsignedIntArray.h"
#include "vtkUnsignedLongArray.h"
#include "vtkUnsignedLongLongArray.h"
#include "vtkUnsignedShortArray.h"
#include "vtkUnstructuredGrid.h"

#include <array>

#include <vtk_netcdf.h>

VTK_ABI_NAMESPACE_BEGIN

vtkStandardNewMacro(vtkNetCDFUGRIDReader);

//--------------------------------------------------------------------------------------------------
vtkNetCDFUGRIDReader::vtkNetCDFUGRIDReader()
{
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
}

//--------------------------------------------------------------------------------------------------
vtkNetCDFUGRIDReader::~vtkNetCDFUGRIDReader()
{
  this->Close();
  this->SetFileName(nullptr);
}

//--------------------------------------------------------------------------------------------------
int vtkNetCDFUGRIDReader::RequestInformation(
  vtkInformation* outInfo, vtkInformationVector**, vtkInformationVector*)
{
  if (!this->Open())
  {
    return 0;
  }

  // look for the "time" variable in the top level block, it should contain timesteps
  // of data arrays. Maybe we should use the "standard_name" attribute instead of the var name ?
  int timeDimId;
  if (nc_inq_dimid(this->NcId, "time", &timeDimId) == NC_NOERR)
  {
    std::size_t timeStepCount;
    if (!this->CheckError(nc_inq_dimlen(this->NcId, timeDimId, &timeStepCount)))
    {
      return 0;
    }

    int timeVarId;
    if (!this->CheckError(nc_inq_varid(this->NcId, "time", &timeVarId)))
    {
      return 0;
    }

    this->TimeSteps.resize(timeStepCount);
    if (!this->CheckError(nc_get_var_double(this->NcId, timeVarId, this->TimeSteps.data())))
    {
      return 0;
    }

    const std::array<double, 2> range = { this->TimeSteps.front(), this->TimeSteps.back() };

    outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(), this->TimeSteps.data(),
      static_cast<int>(this->TimeSteps.size()));
    outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), range.data(), 2);
  }
  else
  {
    outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
    outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_RANGE());
  }

  outInfo->Set(CAN_HANDLE_PIECE_REQUEST(), 1);

  return 1;
}

//--------------------------------------------------------------------------------------------------
int vtkNetCDFUGRIDReader::RequestData(
  vtkInformation*, vtkInformationVector**, vtkInformationVector* outputVector)
{
  this->UpdateProgress(0.0);

  if (!this->Open())
  {
    return 0;
  }

  this->UpdateProgress(0.125);

  if (!this->Initialize())
  {
    return 0;
  }

  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkUnstructuredGrid* output =
    vtkUnstructuredGrid::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkInformationDoubleKey* timeKey =
    vtkInformationDoubleKey::SafeDownCast(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP());

  double time = 0.0;
  if (outInfo->Has(timeKey))
  {
    time = outInfo->Get(timeKey);
  }

  output->GetInformation()->Set(vtkDataObject::DATA_TIME_STEP(), time);

  std::size_t timeStep = 0;
  for (std::size_t step{}; step < this->TimeSteps.size(); step++)
  {
    if (this->TimeSteps[step] >= time)
    {
      timeStep = step;
      break;
    }
  }

  this->UpdateProgress(0.25);

  this->FillPoints(output);
  this->UpdateProgress(0.5);

  this->FillCells(output);
  this->UpdateProgress(0.75);

  this->FillArrays(output, timeStep);
  this->UpdateProgress(1.0);

  return 1;
}

//--------------------------------------------------------------------------------------------------
int vtkNetCDFUGRIDReader::RequestUpdateExtent(
  vtkInformation*, vtkInformationVector**, vtkInformationVector* outputVector)
{
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  int piece = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER());
  int numPieces = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES());

  if (piece < 0 || piece >= numPieces)
  {
    return 0;
  }

  return 1;
}

//--------------------------------------------------------------------------------------------------
bool vtkNetCDFUGRIDReader::Open()
{
  if (!this->FileName)
  {
    vtkWarningMacro("No filename specified.");
    return false;
  }

  // Check if we need to reopen the file
  if (this->CurrentFileName && std::strcmp(this->FileName, this->CurrentFileName) == 0)
  {
    return true;
  }

  this->Close();

  int id;
  int error = nc_open(this->FileName, 0, &id);
  if (error != NC_NOERR)
  {
    vtkWarningMacro("Failed to open file \"" << this->FileName << "\": " << nc_strerror(error));
    return false;
  }

  this->SetCurrentFileName(this->FileName);
  this->NcId = id;
  this->Initialized = false;

  return true;
}

//--------------------------------------------------------------------------------------------------
bool vtkNetCDFUGRIDReader::Initialize()
{
  if (this->Initialized)
  {
    return true;
  }

  int varCount;
  if (!this->CheckError(nc_inq_nvars(this->NcId, &varCount)))
  {
    return false;
  }

  std::vector<int> vars;
  vars.resize(static_cast<std::size_t>(varCount));
  if (!this->CheckError(nc_inq_varids(this->NcId, &varCount, vars.data())))
  {
    return false;
  }

  std::vector<int> meshIds;
  std::vector<int> faceIds;
  std::vector<int> nodeIds;

  for (int var : vars)
  {
    int attCount;
    if (!this->CheckError(nc_inq_varnatts(this->NcId, var, &attCount)))
    {
      return false;
    }

    for (int i{}; i < attCount; ++i)
    {
      char name[NC_MAX_NAME]{};
      nc_inq_attname(this->NcId, var, i, name);
      nc_type type;
      nc_inq_atttype(this->NcId, var, name, &type);

      if (type != NC_CHAR)
      {
        continue;
      }

      // if the cf_role attribute is "mesh_topology" then the var is a mesh
      if (std::strcmp(name, "cf_role") == 0)
      {
        const auto value = this->GetAttributeString(var, name);
        if (value == "mesh_topology")
        {
          meshIds.emplace_back(var);
        }
      }
      else if (std::strcmp(name, "location") == 0)
      {
        // location attribute tells us if this data is associated to cells or points
        const auto value = this->GetAttributeString(var, name);
        if (value == "face")
        {
          faceIds.emplace_back(var);
        }
        else if (value == "node")
        {
          nodeIds.emplace_back(var);
        }
      }
    }
  }

  this->FaceArrayVarIds = std::move(faceIds);
  this->NodeArrayVarIds = std::move(nodeIds);

  if (meshIds.empty())
  {
    return false;
  }

  if (meshIds.size() > 1)
  {
    vtkWarningMacro("Only the first mesh will be read from these multiple meshes");
  }

  this->MeshVarId = meshIds[0]; // only single mesh is supported
  int topologyDimension;

  if (!this->CheckError(
        nc_get_att_int(this->NcId, this->MeshVarId, "topology_dimension", &topologyDimension)))
  {
    vtkErrorMacro(
      "Invalid mesh #" << this->MeshVarId << ". Missing required attribute topology_dimension");
    return false;
  }

  if (topologyDimension != 2)
  {
    vtkErrorMacro("Unsupported topology dimension " << topologyDimension);
    return false;
  }

  // face_node_connectivity variable contains the cells
  std::string faceVarName = this->GetAttributeString(this->MeshVarId, "face_node_connectivity");

  if (!this->CheckError(nc_inq_varid(this->NcId, faceVarName.c_str(), &this->FaceVarId)))
  {
    return false;
  }

  int faceDimCount;
  if (!this->CheckError(nc_inq_varndims(this->NcId, FaceVarId, &faceDimCount)))
  {
    return false;
  }

  std::vector<int> faceDimIds;
  faceDimIds.resize(faceDimCount);
  if (!this->CheckError(nc_inq_vardimid(this->NcId, FaceVarId, faceDimIds.data())))
  {
    return false;
  }

  std::vector<std::size_t> faceDimSize;
  faceDimSize.resize(faceDimCount);
  for (std::size_t i{}; i < faceDimSize.size(); ++i)
  {
    if (!this->CheckError(nc_inq_dimlen(this->NcId, faceDimIds[i], &faceDimSize[i])))
    {
      return false;
    }
  }

  // cells data may be either an array of int[cellcount][cellsize] (default) or
  // int[cellsize][cellcount] face_dimension variable helps us disambiguate by telling us which one
  // is `cellcount`
  int faceDimId;
  if (nc_inq_attid(this->NcId, this->MeshVarId, "face_dimension", &faceDimId) != NC_NOERR)
  {
    this->FaceCount = faceDimSize[0];
    this->NodesPerFace = faceDimSize[1];
    this->FaceStride = this->NodesPerFace;
    this->NodesPerFaceStride = 1;
  }
  else
  {
    char name[NC_MAX_NAME]{};
    if (!this->CheckError(nc_inq_dimname(this->NcId, faceDimIds[0], name)))
    {
      return false;
    }

    if (this->GetAttributeString(this->MeshVarId, "face_dimension") == name)
    {
      this->FaceCount = faceDimSize[0];
      this->NodesPerFace = faceDimSize[1];
      this->FaceStride = this->NodesPerFace;
      this->NodesPerFaceStride = 1;
    }
    else
    {
      this->FaceCount = faceDimSize[1];
      this->NodesPerFace = faceDimSize[0];
      this->FaceStride = 1;
      this->NodesPerFaceStride = this->FaceCount;
    }
  }

  // node_coordinates attributes help us get the 2 vars that correspond to x and y
  std::string nodeVarName = this->GetAttributeString(this->MeshVarId, "node_coordinates");
  std::string nodeXVarName{ nodeVarName.begin(),
    std::find(nodeVarName.begin(), nodeVarName.end(), ' ') };
  std::string nodeYVarName{ std::find(nodeVarName.rbegin(), nodeVarName.rend(), ' ').base(),
    nodeVarName.end() };

  if (!this->CheckError(nc_inq_varid(this->NcId, nodeXVarName.c_str(), &this->NodeXVarId)))
  {
    return false;
  }

  if (!this->CheckError(nc_inq_varid(this->NcId, nodeYVarName.c_str(), &this->NodeYVarId)))
  {
    return false;
  }

  int nodeDimCount;
  if (!this->CheckError(nc_inq_varndims(this->NcId, this->NodeXVarId, &nodeDimCount)))
  {
    return false;
  }

  std::vector<int> nodeXDimIds;
  nodeXDimIds.resize(nodeDimCount);
  if (!this->CheckError(nc_inq_vardimid(this->NcId, this->NodeXVarId, nodeXDimIds.data())))
  {
    return false;
  }

  if (!this->CheckError(nc_inq_dimlen(this->NcId, nodeXDimIds[0], &this->NodeCount)))
  {
    return false;
  }

  if (this->NodesPerFace > 3) // may be mixed mesh
  {
    if (!this->CheckError(
          nc_get_att(this->NcId, this->FaceVarId, "_FillValue", &this->FaceFillValue)))
    {
      vtkErrorMacro("_FillValue attribute missing - The connectivity variable has to specify a "
                    "_FillValue attribute because it has more than 3 nodes per face");
      return false;
    }
  }

  if (nc_get_att(this->NcId, this->FaceVarId, "start_index", &this->FaceStartIndex) != NC_NOERR)
  {
    this->FaceStartIndex = 0;
  }

  if (!this->CheckError(nc_inq_vartype(this->NcId, this->NodeXVarId, &this->NodeType)))
  {
    return false;
  }

  this->Initialized = true;
  return true;
}

//--------------------------------------------------------------------------------------------------
bool vtkNetCDFUGRIDReader::FillPoints(vtkUnstructuredGrid* output)
{
  vtkNew<vtkPoints> points;

  if (this->NodeType == NC_FLOAT)
  {
    points->SetDataTypeToFloat();
    points->SetNumberOfPoints(this->NodeCount);

    std::vector<float> x;
    x.resize(this->NodeCount);
    std::vector<float> y;
    y.resize(this->NodeCount);

    if (!this->CheckError(nc_get_var_float(this->NcId, this->NodeXVarId, x.data())))
    {
      return false;
    }

    if (!this->CheckError(nc_get_var_float(this->NcId, this->NodeYVarId, y.data())))
    {
      return false;
    }

    for (std::size_t i{}; i < this->NodeCount; ++i)
    {
      auto data = static_cast<float*>(points->GetVoidPointer(static_cast<int>(i * 3)));

      data[0] = x[i];
      data[1] = y[i];
      data[2] = 0.0f;
    }
  }
  else if (this->NodeType == NC_DOUBLE)
  {
    points->SetDataTypeToDouble();
    points->SetNumberOfPoints(this->NodeCount);

    std::vector<double> x;
    x.resize(this->NodeCount);
    std::vector<double> y;
    y.resize(this->NodeCount);

    if (!this->CheckError(nc_get_var_double(this->NcId, this->NodeXVarId, x.data())))
    {
      return false;
    }

    if (!this->CheckError(nc_get_var_double(this->NcId, this->NodeYVarId, y.data())))
    {
      return false;
    }

    for (std::size_t i{}; i < this->NodeCount; ++i)
    {
      auto data = static_cast<double*>(points->GetVoidPointer(static_cast<int>(i * 3)));

      data[0] = x[i];
      data[1] = y[i];
      data[2] = 0.0;
    }
  }
  else
  {
    vtkErrorMacro("Invalid mesh has nodes that are not floating point values");
    return false;
  }

  output->SetPoints(points);

  return true;
}

//--------------------------------------------------------------------------------------------------
bool vtkNetCDFUGRIDReader::FillCells(vtkUnstructuredGrid* output)
{
  std::vector<int> faces;
  faces.resize(this->NodesPerFace * this->FaceCount);
  if (!this->CheckError(nc_get_var(this->NcId, this->FaceVarId, faces.data())))
  {
    return false;
  }

  output->Allocate(this->FaceCount);

  std::vector<vtkIdType> pointIds;
  pointIds.resize(this->NodesPerFace);
  for (std::size_t i{}; i < this->FaceCount; ++i)
  {
    VTKCellType cell_type = VTK_TRIANGLE;
    vtkIdType point_count = 3;

    for (std::size_t j{}; j < this->NodesPerFace; ++j)
    {
      const vtkIdType id = faces[j * this->NodesPerFaceStride + i * this->FaceStride];

      if (this->NodesPerFace > 3 && id == this->FaceFillValue)
      {
        cell_type = VTK_TRIANGLE;
        point_count = 3;
        continue;
      }
      else if (this->NodesPerFace > 3)
      {
        cell_type = VTK_QUAD;
        point_count = 4;
      }
      else
      {
        cell_type = VTK_TRIANGLE;
        point_count = 3;
      }

      pointIds[j] = id - this->FaceStartIndex;
    }

    output->InsertNextCell(cell_type, point_count, pointIds.data());
  }

  return true;
}

//--------------------------------------------------------------------------------------------------
bool vtkNetCDFUGRIDReader::FillArrays(vtkUnstructuredGrid* output, std::size_t timeStep)
{
  for (auto faceArrayVarId : this->FaceArrayVarIds)
  {
    const auto array = this->GetArrayData(faceArrayVarId, timeStep, this->FaceCount);
    if (!array)
    {
      return false;
    }

    output->GetCellData()->AddArray(array);
  }

  for (auto nodeArrayVarId : this->NodeArrayVarIds)
  {
    const auto array = this->GetArrayData(nodeArrayVarId, timeStep, this->NodeCount);
    if (!array)
    {
      return false;
    }

    output->GetPointData()->AddArray(array);
  }

  return true;
}

//--------------------------------------------------------------------------------------------------
void vtkNetCDFUGRIDReader::Close()
{
  if (this->NcId == -1)
  {
    return;
  }

  int error = nc_close(this->NcId);
  if (error != NC_NOERR)
  {
    vtkWarningMacro("Failed to close file: " << nc_strerror(error));
  }

  this->SetCurrentFileName(nullptr);
  this->NcId = -1;
}

//--------------------------------------------------------------------------------------------------
bool vtkNetCDFUGRIDReader::CheckError(int error)
{
  if (error != NC_NOERR)
  {
    vtkWarningMacro("Failed to read information of file \"" << this->CurrentFileName
                                                            << "\": " << nc_strerror(error));
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------------------------------
std::string vtkNetCDFUGRIDReader::GetAttributeString(int var, std::string name)
{
  std::size_t size;
  if (!this->CheckError(nc_inq_attlen(this->NcId, var, name.c_str(), &size)))
  {
    vtkErrorMacro("Invalid mesh #" << var << ". Missing attribute " << name);
    return "";
  }

  std::string output;
  output.resize(size);
  if (!this->CheckError(nc_get_att_text(this->NcId, var, name.c_str(), &output[0])))
  {
    vtkErrorMacro("Invalid mesh #" << var << ". Missing attribute " << name);
    return "";
  }

  return output;
}

//--------------------------------------------------------------------------------------------------
static vtkDataArray* MakeDataArray(nc_type type)
{
  switch (type)
  {
    case NC_BYTE:
      return vtkSignedCharArray::New();
    case NC_CHAR:
      return vtkCharArray::New();
    case NC_SHORT:
      return vtkShortArray::New();
    case NC_INT:
      return vtkIntArray::New();
    case NC_FLOAT:
      return vtkFloatArray::New();
    case NC_DOUBLE:
      return vtkDoubleArray::New();
    case NC_UBYTE:
      return vtkUnsignedCharArray::New();
    case NC_USHORT:
      return vtkUnsignedShortArray::New();
    case NC_UINT:
      return vtkUnsignedIntArray::New();
    case NC_INT64:
      return vtkLongLongArray::New();
    case NC_UINT64:
      return vtkUnsignedLongLongArray::New();
    default:
      return nullptr;
      break;
  }
}

//--------------------------------------------------------------------------------------------------
struct DataArrayExtractor
{
  template <typename OutArray>
  void operator()(
    OutArray* output, int NcId, int var, std::size_t time, std::size_t size, int* result)
  {
    vtkDataArrayAccessor<OutArray> output_it{ output };

    char name[NC_MAX_NAME]{};
    if (nc_inq_varname(NcId, var, name) != NC_NOERR)
    {
      const auto default_name = std::to_string(var);
      std::strcpy(name, default_name.c_str());
    }

    output->SetName(name);
    output->SetNumberOfComponents(1);
    output->SetNumberOfTuples(size);

    std::array<std::size_t, 2> start{ time, 0 };
    std::array<std::size_t, 2> count{ 1, size };

    *result = nc_get_vara(NcId, var, start.data(), count.data(), output->GetPointer(0));
  }
};

//--------------------------------------------------------------------------------------------------
vtkDataArray* vtkNetCDFUGRIDReader::GetArrayData(int var, std::size_t time, std::size_t size)
{
  nc_type type;
  if (!this->CheckError(nc_inq_vartype(this->NcId, var, &type)))
  {
    return nullptr;
  }

  DataArrayExtractor worker;
  using Dispatcher = vtkArrayDispatch::DispatchByValueType<vtkArrayDispatch::AllTypes>;

  vtkDataArray* output = MakeDataArray(type);
  int result = NC_NOERR;
  Dispatcher::Execute(output, worker, this->NcId, var, time, size, &result);

  if (!this->CheckError(result))
  {
    return nullptr;
  }

  return output;
}

//--------------------------------------------------------------------------------------------------
void vtkNetCDFUGRIDReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Filename  : " << this->FileName << std::endl;
  os << indent << "NodeCount : " << this->NodeCount << std::endl;
  os << indent << "FaceCount : " << this->FaceCount << std::endl;
}

VTK_ABI_NAMESPACE_END
