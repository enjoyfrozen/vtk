/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkHDFReaderImplementation.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkHDFReaderImplementation.h"
#include <sstream>
#include <stdexcept>
#include <type_traits>

#include "vtkCharArray.h"
#include "vtkDataObject.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkIntArray.h"
#include "vtkLongArray.h"
#include "vtkLongLongArray.h"
#include "vtkShortArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnsignedIntArray.h"
#include "vtkUnsignedLongArray.h"
#include "vtkUnsignedLongLongArray.h"
#include "vtkUnsignedShortArray.h"

//------------------------------------------------------------------------------
vtkHDFReader::Implementation::Implementation(vtkHDFReader* reader)
  : File(-1)
  , VTKGroup(0)
  , DataSetType(-1)
  , NumberOfPartitions(-1)
  , Reader(reader)
{
  std::fill(this->AttributeDataGroup.begin(), this->AttributeDataGroup.end(), -1);
  std::fill(this->Version.begin(), this->Version.end(), 0);
}

//------------------------------------------------------------------------------
vtkHDFReader::Implementation::~Implementation()
{
  this->Close();
}

//------------------------------------------------------------------------------
bool vtkHDFReader::Implementation::Open(const char* fileName)
{
  bool error = false;
  if (!fileName)
  {
    vtkErrorWithObjectMacro(this->Reader, "Invalid filename: " << fileName);
    return false;
  }
  if (this->FileName.empty() || this->FileName != fileName)
  {
    this->FileName = fileName;
    if (this->File >= 0)
    {
      this->Close();
    }
    if ((this->File = H5Fopen(this->FileName.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT) < 0))
    {
      vtkErrorWithObjectMacro(this->Reader, "Error oppening " << fileName);
      return false;
    }
    // open all relevant groups
    std::array<const char*, 3> groupNames = { "VTKHDF", "VTKHDF/PointData", "VTKHDF/CellData" };
    std::array<hid_t*, 3> groups = { &this->VTKGroup, &this->AttributeDataGroup[0],
      &this->AttributeDataGroup[1] };
    for (int i = 0; i < groups.size(); ++i)
    {
      if ((*groups[i] = H5Gopen(this->File, groupNames[i], H5P_DEFAULT)) < 0)
      {
        vtkErrorWithObjectMacro(this->Reader, "Error oppening " << groupNames[i]);
        return false;
      }
    }
    if (!GetAttribute("Version", this->Version.size(), &this->Version[0]))
    {
      return false;
    }
    hid_t attr = -1;
    hid_t dataset = -1;
    hid_t dataspace = -1;
    try
    {
      if ((attr = H5Aopen_name(this->VTKGroup, "WholeExtent")) < 0)
      {
        this->DataSetType = VTK_UNSTRUCTURED_GRID;
      }
      else
      {
        this->DataSetType = VTK_IMAGE_DATA;
        const char* datasetName = "/VTKHDF/Extents";
        if ((dataset = H5Dopen(this->File, datasetName, H5P_DEFAULT)) < 0)
        {
          throw std::runtime_error(std::string("Cannot open ") + datasetName);
        }
        if ((dataspace = H5Dget_space(dataset)) < 0)
        {
          throw std::runtime_error(std::string("Cannot get space for dataset ") + datasetName);
        }
        int rank;
        if ((rank = H5Sget_simple_extent_ndims(dataspace)) < 0)
        {
          throw std::runtime_error(
            std::string(datasetName) + " dataset: get_simple_extent_ndims error");
        }
        if (rank != 2)
        {
          throw std::runtime_error(std::string(datasetName) + " dataset should have rank 2");
        }
        hsize_t dims[2];
        if (H5Sget_simple_extent_dims(dataspace, dims, nullptr) < 0)
        {
          throw std::runtime_error(std::string("Cannot find dimension for ") + datasetName);
        }
        this->NumberOfPartitions = dims[0];
      }
    }
    catch (const std::exception& e)
    {
      vtkErrorWithObjectMacro(this->Reader, << e.what());
      error = true;
    }
    if (attr >= 0)
    {
      error = H5Aclose(attr) < 0 || error;
    }

    if (dataspace >= 0)
    {
      error = H5Sclose(dataspace) < 0 || error;
    }
    if (dataset >= 0)
    {
      error = H5Dclose(dataset) < 0 || error;
    }
  }
  return error;
}

//------------------------------------------------------------------------------
void vtkHDFReader::Implementation::Close()
{
  this->DataSetType = -1;
  this->NumberOfPartitions = 0;
  std::fill(this->Version.begin(), this->Version.end(), 0);
  for (int i = 0; i < this->AttributeDataGroup.size(); ++i)
  {
    if (this->AttributeDataGroup[i] >= 0)
    {
      H5Gclose(this->AttributeDataGroup[i]);
      this->AttributeDataGroup[i] = -1;
    }
  }
  if (this->VTKGroup >= 0)
  {
    H5Gclose(this->VTKGroup);
    this->VTKGroup = -1;
  }
  if (this->File >= 0)
  {
    H5Fclose(this->File);
    this->File = -1;
  }
}

//------------------------------------------------------------------------------
template <typename T>
hid_t vtkHDFReader::Implementation::TemplateToHDFType()
{
  hid_t hdfType = -1;
  if (std::is_same<T, int>::value)
  {
    hdfType = H5T_NATIVE_INT;
  }
  else if (std::is_same<T, double>::value)
  {
    hdfType = H5T_NATIVE_DOUBLE;
  }
  return hdfType;
}

//------------------------------------------------------------------------------
bool vtkHDFReader::Implementation::GetPartitionExtent(hsize_t partitionIndex, int* extent)
{
  hid_t dataset = -1;
  hid_t memspace = -1;
  hid_t dataspace = -1;
  const int RANK = 2;
  bool error = false;
  const char* datasetName = "/VTKHDF/Extents";
  try
  {
    // create the memory space
    hsize_t dimsm[RANK];
    dimsm[0] = 1;
    dimsm[1] = 6;
    if ((memspace = H5Screate_simple(RANK, dimsm, NULL)) < 0)
    {
      throw std::runtime_error("Error H5Screate_simple for memory space");
    }
    // create the file dataspace + hyperslab
    if ((dataset = H5Dopen(this->File, datasetName, H5P_DEFAULT)) < 0)
    {
      throw std::runtime_error(std::string("Cannot open ") + datasetName);
    }
    hsize_t start[RANK] = { partitionIndex, 0 }, count[RANK] = { 1, 2 };

    if ((dataspace = H5Dget_space(dataset)) < 0)
    {
      throw std::runtime_error(std::string("Cannot get space for dataset ") + datasetName);
    }
    if (H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, start, nullptr, count, nullptr) < 0)
    {
      throw std::runtime_error(std::string("Error selecting hyperslab for ") + datasetName);
    }
    // read hyperslab
    if (H5Dread(dataset, H5T_NATIVE_INT, memspace, dataspace, H5P_DEFAULT, extent) < 0)
    {
      throw std::runtime_error(std::string("Error reading hyperslab from ") + datasetName);
    }
  }
  catch (const std::exception& e)
  {
    vtkErrorWithObjectMacro(this->Reader, << e.what());
    error = true;
  }
  if (memspace >= 0)
  {
    error = H5Sclose(memspace) < 0 || error;
  }
  if (dataspace >= 0)
  {
    error = H5Sclose(dataspace) < 0 || error;
  }
  if (dataset >= 0)
  {
    error = H5Dclose(dataset) < 0 || error;
  }
  return error;
}

//------------------------------------------------------------------------------
template <typename T>
bool vtkHDFReader::Implementation::GetAttribute(
  const char* attributeName, int dimExpected, T* value)
{
  hid_t attr = -1;
  hid_t space = -1;
  bool error = false;
  try
  {
    if ((attr = H5Aopen_name(this->VTKGroup, attributeName)) < 0)
    {
      throw std::runtime_error(std::string(attributeName) + " attribute not found");
    }
    if ((space = H5Aget_space(attr)) < 0)
    {
      throw std::runtime_error(std::string(attributeName) + " attribute: get_space error");
    }
    int rank;
    if ((rank = H5Sget_simple_extent_ndims(space)) < 0)
    {
      throw std::runtime_error(
        std::string(attributeName) + " attribute: get_simple_extent_ndims error");
    }
    if (rank != 1)
    {
      throw std::runtime_error(std::string(attributeName) + " attribute should have rank 1");
    }
    hsize_t dims;
    if (H5Sget_simple_extent_dims(space, &dims, nullptr) < 0)
    {
      throw std::runtime_error(std::string("Cannot find dimension for ") + attributeName);
    }

    if (dims != dimExpected)
    {
      std::ostringstream ostr;
      ostr << attributeName << " attribute should have " << dimExpected << " dimensions";
      throw std::runtime_error(ostr.str().c_str());
    }
    hid_t hdfType = 0;
    if ((hdfType = this->TemplateToHDFType<T>()) < 0)
    {
      throw std::runtime_error(std::string("Native type not implemented: ") + typeid(T).name());
    }
    if (H5Aread(attr, hdfType, value) < 0)
    {
      throw std::runtime_error(std::string("Error reading ") + attributeName + " attribute");
    }
  }
  catch (const std::exception& e)
  {
    vtkErrorWithObjectMacro(this->Reader, << e.what());
    error = true;
  }
  if (space >= 0)
  {
    error = H5Sclose(space) < 0 || error;
  }
  if (attr >= 0)
  {
    error = H5Aclose(attr) < 0 || error;
  }
  return error;
}

//------------------------------------------------------------------------------
hid_t vtkHDFReader::Implementation::GetArrayInfo(
  int attributeType, const char* name, hid_t* type, int* components)
{
  hid_t group = this->AttributeDataGroup[attributeType];
  bool error = false;
  hid_t dataset = -1;
  hid_t dataspace = -1;
  try
  {
    if ((dataset = H5Dopen(group, name, H5P_DEFAULT)) < 0)
    {
      throw std::runtime_error(std::string("Cannot open ") + name);
    }
    if ((dataspace = H5Dget_space(dataset)) < 0)
    {
      throw std::runtime_error(std::string("Cannot get space for dataset ") + name);
    }
    if ((*type = H5Tget_native_type(dataspace, H5T_DIR_ASCEND)) < 0)
    {
      throw std::runtime_error(std::string("Cannot get type for dataset ") + name);
    }
    int rank;
    if ((rank = H5Sget_simple_extent_ndims(dataspace)) < 0)
    {
      throw std::runtime_error(std::string(name) + " dataset: get_simple_extent_ndims error");
    }
    std::vector<hsize_t> dims(rank, 0);
    if (H5Sget_simple_extent_dims(dataspace, &dims[0], nullptr) < 0)
    {
      throw std::runtime_error(std::string("Cannot find dimension for ") + name);
    }
    *components = dims[dims.size() - 1];
  }
  catch (const std::exception& e)
  {
    vtkErrorWithObjectMacro(this->Reader, << e.what());
    error = true;
  }
  if (dataspace >= 0)
  {
    error = H5Sclose(dataspace) < 0 || error;
  }
  if (error && dataset >= 0)
  {
    H5Dclose(dataset);
    dataset = -1;
  }
  return dataset;
}

//------------------------------------------------------------------------------
namespace
{
herr_t AddName(hid_t group, const char* name, const H5L_info_t* info, void* op_data)
{
  auto array = static_cast<std::vector<std::string>*>(op_data);
  herr_t status = -1;
  H5O_info_t infobuf;
  if ((status = H5Oget_info_by_name(group, name, &infobuf, H5P_DEFAULT)) >= 0 &&
    infobuf.type == H5O_TYPE_DATASET)
  {
    array->push_back(name);
  }
  return status;
}
};

//------------------------------------------------------------------------------
std::vector<std::string> vtkHDFReader::Implementation::GetArrayNames(int attributeType)
{
  std::vector<std::string> array;
  hid_t group = this->AttributeDataGroup[attributeType];
  H5Literate(group, H5_INDEX_NAME, H5_ITER_NATIVE, NULL, AddName, &array);
  return array;
}

//------------------------------------------------------------------------------
vtkDataArray* vtkHDFReader::Implementation::GetArray(
  hid_t dataset, hid_t type, int attributeType, int* wholeExtent, int numberOfComponents)
{
  vtkDataArray* array = nullptr;
  if (type == H5T_NATIVE_CHAR)
  {
    array = this->GetArray<char>(dataset, attributeType, wholeExtent, numberOfComponents);
  }
  else if (type == H5T_NATIVE_SCHAR)
  {
    array = this->GetArray<char>(dataset, attributeType, wholeExtent, numberOfComponents);
  }
  else if (type == H5T_NATIVE_UCHAR)
  {
    array = this->GetArray<unsigned char>(dataset, attributeType, wholeExtent, numberOfComponents);
  }
  else if (type == H5T_NATIVE_SHORT)
  {
    array = this->GetArray<short>(dataset, attributeType, wholeExtent, numberOfComponents);
  }
  else if (type == H5T_NATIVE_USHORT)
  {
    array = this->GetArray<unsigned short>(dataset, attributeType, wholeExtent, numberOfComponents);
  }
  else if (type == H5T_NATIVE_INT)
  {
    array = this->GetArray<int>(dataset, attributeType, wholeExtent, numberOfComponents);
  }
  else if (type == H5T_NATIVE_UINT)
  {
    array = this->GetArray<unsigned int>(dataset, attributeType, wholeExtent, numberOfComponents);
  }
  else if (type == H5T_NATIVE_LONG)
  {
    array = this->GetArray<long>(dataset, attributeType, wholeExtent, numberOfComponents);
  }
  else if (type == H5T_NATIVE_ULONG)
  {
    array = this->GetArray<unsigned long>(dataset, attributeType, wholeExtent, numberOfComponents);
  }
  else if (type == H5T_NATIVE_LLONG)
  {
    array = this->GetArray<long long>(dataset, attributeType, wholeExtent, numberOfComponents);
  }
  else if (type == H5T_NATIVE_ULLONG)
  {
    array =
      this->GetArray<unsigned long long>(dataset, attributeType, wholeExtent, numberOfComponents);
  }
  else if (type == H5T_NATIVE_FLOAT)
  {
    array = this->GetArray<float>(dataset, attributeType, wholeExtent, numberOfComponents);
  }
  else if (type == H5T_NATIVE_DOUBLE)
  {
    array = this->GetArray<double>(dataset, attributeType, wholeExtent, numberOfComponents);
  }
  return array;
}

//------------------------------------------------------------------------------
template <typename T>
vtkDataArray* vtkHDFReader::Implementation::GetArray(
  hid_t dataset, int attributeType, int* wholeExtent, int numberOfComponents)
{
  int pointAdjustment = (attributeType == vtkDataObject::POINT ? 1 : 0);
  int numberOfTuples = (wholeExtent[1] - wholeExtent[0] + pointAdjustment) *
    (wholeExtent[3] - wholeExtent[2] + pointAdjustment) *
    (wholeExtent[5] - wholeExtent[4] + pointAdjustment);
  auto array = vtkAOSDataArrayTemplate<T>::New();
  array->SetNumberOfComponents(numberOfComponents);
  array->SetNumberOfTuples(numberOfTuples);
  T* data = array->GetPointer(0);
  if (!this->GetArray(dataset, attributeType, wholeExtent, numberOfComponents, data))
  {
    array->Delete();
    array = nullptr;
  }
  return array;
}

//------------------------------------------------------------------------------
template <typename T>
bool vtkHDFReader::Implementation::GetArray(
  hid_t dataset, int attributeType, int* wholeExtent, int numberOfComponents, T* data)
{
  hid_t memspace = -1;
  hid_t dataspace = -1;
  const int RANK = 4;
  bool error = false;
  try
  {
    // create the memory space, reverse axis order for VTK fortran order
    int pointAdjustment = (attributeType == vtkDataObject::POINT ? 1 : 0);
    std::array<hsize_t, RANK> dimsm = { static_cast<hsize_t>(
                                          wholeExtent[5] - wholeExtent[4] + pointAdjustment),
      static_cast<hsize_t>(wholeExtent[3] - wholeExtent[2] + pointAdjustment),
      static_cast<hsize_t>(wholeExtent[1] = wholeExtent[0] + pointAdjustment),
      static_cast<hsize_t>(numberOfComponents) };
    if ((memspace = H5Screate_simple(dimsm.size(), &dimsm[0], NULL)) < 0)
    {
      throw std::runtime_error("Error H5Screate_simple for memory space");
    }
    // create the file dataspace
    if ((dataspace = H5Dget_space(dataset)) < 0)
    {
      throw std::runtime_error("Error H5Dget_space for imagedata");
    }
    // read hyperslab
    if (H5Dread(dataset, H5T_NATIVE_INT, memspace, dataspace, H5P_DEFAULT, data) < 0)
    {
      throw std::runtime_error(std::string("Error reading imagedata"));
    }
  }
  catch (const std::exception& e)
  {
    vtkErrorWithObjectMacro(this->Reader, << e.what());
    error = true;
  }
  if (memspace >= 0)
  {
    error = H5Sclose(memspace) < 0 || error;
  }
  if (dataspace >= 0)
  {
    error = H5Sclose(dataspace) < 0 || error;
  }
  return error;
}

//------------------------------------------------------------------------------
// explicit template instantiation
template bool vtkHDFReader::Implementation::GetAttribute<int>(
  const char* attributeName, int dim, int* value);
template bool vtkHDFReader::Implementation::GetAttribute<double>(
  const char* attributeName, int dim, double* value);
