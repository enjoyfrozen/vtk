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
#include <map>
#include <string>
#include <vector>

class vtkDataArray;

/**
 * Implementation for the vtkHDFReader. Opens, closes and
 * reads information from a VTK HDF file.
 */
class vtkHDFReader::Implementation
{
public:
  Implementation(vtkHDFReader* reader);
  virtual ~Implementation();
  /**
   * Opens this VTK HDF file and checks if it is valid.
   */
  bool Open(const char* fileName);
  /**
   * Closes the VTK HDF file and releases any allocated resources.
   */
  void Close();
  /**
   * Type of vtkDataSet stored by the HDF file, such as VTK_IMAGE_DATA or
   * VTK_UNSTRUCTURED_GRID, from vtkTypes.h
   */
  int GetDataSetType() { return this->DataSetType; }
  /**
   * Returns the version of the VTK HDF implementation.
   */
  const std::array<int, 2>& GetVersion() { return this->Version; }
  /**
   * Reads an attribute from the /VTKHDF group
   */
  template <typename T>
  bool GetAttribute(const char* attributeName, int numberOfElements, T* value);
  /**
   * Returns the number of partitions for this dataset.
   */
  int GetNumberOfPieces() { return this->NumberOfPieces; }
  /**
   * Reads information about 'partition' from dataset 'name'.
   * It reads 'numberOfElements' values of type 'T'
   */
  template <typename T>
  bool GetPartitionInfo(const char* name, int partition, int numberOfElements, T* value);
  /**
   * For an ImageData, sets the extent for 'partitionIndex'. Returns
   * true for success and false otherwise.
   */
  bool GetPartitionExtent(hsize_t partitionIndex, int* extent);
  /**
   * Returns the names of arrays for 'attributeType' (point or cell).
   */
  std::vector<std::string> GetArrayNames(int attributeType);
  /**
   * Reads and returns a new vtkDataArray of the correct type
   * that has to be deleted by the user.
   */
  vtkDataArray* GetArray(int attributeType, const char* name, hsize_t* fileExtent);

  std::vector<hsize_t> GetDimensions(const char* dataset);

protected:
  /**
   * Used to store HDF native types in a map
   */
  struct TypeDescription
  {
    int Class;
    int Size;
    int Sign;
    TypeDescription()
      : Class(H5T_NO_CLASS)
      , Size(0)
      , Sign(H5T_SGN_ERROR)
    {
    }
    bool operator<(const TypeDescription& other) const
    {
      return Class < other.Class || (Class == other.Class && Size < other.Size) ||
        (Class == other.Class && Size == other.Size && Sign < other.Sign);
    }
  };

protected:
  /**
   * Opens the hdf5 dataset given the 'group'
   * and 'name'.
   * Returns the hdf dataset and sets 'nativeType' and the 'numberOfComponents'.
   */
  hid_t OpenDataSet(
    hid_t group, const char* name, int gridNdims, hid_t* nativeType, hsize_t* numberOfComponents);
  /**
   * Convert C++ template type T to HDF5 native type
   * this can be constexpr in C++17 standard
   */
  template <typename T>
  hid_t TemplateTypeToHdfNativeType();
  /**
   * Create a vtkDataArray based on the C++ template type T.
   * For instance, for a float we create a vtkFloatArray.
   * this can be constexpr in C++17 standard
   */
  template <typename T>
  vtkDataArray* NewVtkDataArray();

  //@{
  /**
   * Reads a vtkDataArray of type T from the attributeType, dataset
   * The array has type 'T' and 'numberOfComponents'. We are reading
   * fileExtent slab from the array. It returns the array or nullptr
   * in case of an error.
   */
  template <typename T>
  vtkDataArray* GetArray(
    int attributeType, hid_t dataset, hsize_t* fileExtent, hsize_t numberOfComponents);
  template <typename T>
  bool GetArray(
    int attributeType, hid_t dataset, hsize_t* fileExtent, hsize_t numberOfComponents, T* data);
  //@}
  /**
   * Builds a map between native types and GetArray routines for that type.
   */
  void BuildTypeReaderMap();
  /**
   * Associates a struc of three integers with HDF type. This can be used as
   * key in a map.
   */
  TypeDescription GetTypeDescription(hid_t type);

private:
  std::string FileName;
  hid_t File;
  hid_t VTKGroup;
  // in the same order as vtkDataObject::AttributeTypes: POINT, CELL
  std::array<hid_t, 2> AttributeDataGroup;
  int DataSetType;
  int NumberOfPieces;
  std::array<int, 2> Version;
  vtkHDFReader* Reader;
  using ArrayReader = vtkDataArray* (vtkHDFReader::Implementation::*)(int attributeType,
    hid_t dataset, hsize_t* fileExtent, hsize_t numberOfComponents);
  std::map<TypeDescription, ArrayReader> TypeReaderMap;
};

//------------------------------------------------------------------------------
// explicit template instantiation declaration
extern template bool vtkHDFReader::Implementation::GetAttribute<int>(
  const char* attributeName, int dim, int* value);
extern template bool vtkHDFReader::Implementation::GetAttribute<double>(
  const char* attributeName, int dim, double* value);

#endif
