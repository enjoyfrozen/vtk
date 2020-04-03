/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkHDFReaderImplementation.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkHDFReaderImplementation
 * @brief   Implementation class for vtkHDFReader
 *
 */

#ifndef vtkHDFReaderImplementation_h
#define vtkHDFReaderImplementation_h

#include "vtkHDFReader.h"
#include "vtk_hdf5.h"
#include <array>
#include <string>
#include <vector>

class vtkDataArray;

class vtkHDFReader::Implementation
{
public:
  Implementation(vtkHDFReader* reader);
  virtual ~Implementation();
  bool Open(const char* fileName);
  void Close();
  /**
   * Type of vtkDataSet stored by the HDF file, such as VTK_IMAGE_DATA or
   * VTK_UNSTRUCTURED_GRID, from vtkTypes.h
   */
  int GetDataSetType() { return this->DataSetType; }
  int GetNumberOfPartitions() { return NumberOfPartitions; }
  int GetNumberOfAttributeTypes() { return AttributeDataGroup.size(); }
  const std::array<int, 2>& GetVersion() { return this->Version; }
  template <typename T>
  bool GetAttribute(const char* attributeName, int dim, T* value);
  bool GetPartitionExtent(hsize_t partitionIndex, int* extent);
  /**
   * Returns the native type of the array (hdf dataset) and sets the number of
   * components.
   */
  hid_t GetArrayInfo(int attributeType, const char* name, hid_t* type, int* components);
  std::vector<std::string> GetArrayNames(int attributeType);
  /**
   * Creates a new vtkDataArray that has to be deleted by the user.
   */
  vtkDataArray* GetArray(
    hid_t dataset, hid_t type, int attributeType, int* wholeExtent, int numberOfComponents);

protected:
  /**
   * Convert C++ template type T to HDF5 native type
   */
  template <typename T>
  hid_t TemplateToHDFType();
  template <typename T>
  vtkDataArray* GetArray(
    hid_t dataset, int attributeType, int* wholeExtent, int numberOfComponents);
  template <typename T>
  bool GetArray(
    hid_t dataset, int attributeType, int* wholeExtent, int numberOfComponents, T* data);

private:
  std::string FileName;
  hid_t File;
  hid_t VTKGroup;
  // in the same order as vtkDataObject::AttributeTypes: POINT, CELL
  std::array<hid_t, 2> AttributeDataGroup;
  int DataSetType;
  int NumberOfPartitions;
  std::array<int, 2> Version;
  vtkHDFReader* Reader;
};

//------------------------------------------------------------------------------
// explicit template instantiation declaration
extern template bool vtkHDFReader::Implementation::GetAttribute<int>(
  const char* attributeName, int dim, int* value);
extern template bool vtkHDFReader::Implementation::GetAttribute<double>(
  const char* attributeName, int dim, double* value);

#endif
