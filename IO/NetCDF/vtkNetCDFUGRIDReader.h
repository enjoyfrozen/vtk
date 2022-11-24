/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkNetCDFUGRIDReader.h

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef vtkNetCDFCAMReader_h
#define vtkNetCDFCAMReader_h

#include "vtkIONetCDFModule.h" // For export macro
#include "vtkUnstructuredGridAlgorithm.h"

#include <cstdlib>
#include <memory>
#include <vector>

VTK_ABI_NAMESPACE_BEGIN

class vtkDataArraySelection;

/**
 * @class   vtkNetCDFUGRIDeader
 * @brief   Read unstructured NetCDF UGRID files.
 *
 * This reader read a single 2D mesh for a NetCDF UGRID. It will extract points and cells
 * but not edges. Timeline is supported as long as the "time" variable exists in the file.
 * Supported point types are float and double.
 * Supported cell types are triangle and quad.
 * Supported data array types are [u]int[8/16/32/64], float and double.
 */
class VTKIONETCDF_EXPORT vtkNetCDFUGRIDReader : public vtkUnstructuredGridAlgorithm
{
  struct vtkInternals;

public:
  static vtkNetCDFUGRIDReader* New();
  vtkTypeMacro(vtkNetCDFUGRIDReader, vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Get/Set the file name of the file.
   */
  vtkSetFilePathMacro(FileName);
  vtkGetFilePathMacro(FileName);
  ///@}

protected:
  vtkNetCDFUGRIDReader();
  virtual ~vtkNetCDFUGRIDReader();

  int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestUpdateExtent(
    vtkInformation*, vtkInformationVector**, vtkInformationVector* outputVector) override;

  vtkSetFilePathMacro(CurrentFileName);

  bool Open();
  bool Initialize();
  bool FillPoints(vtkUnstructuredGrid* output);
  bool FillCells(vtkUnstructuredGrid* output);
  bool FillArrays(vtkUnstructuredGrid* output, std::size_t timeStep);
  void Close();

  bool CheckError(int error);
  std::string GetAttributeString(int var, std::string name);
  vtkDataArray* GetArrayData(int var, std::size_t time, std::size_t size);

  char* FileName = nullptr;
  char* CurrentFileName = nullptr;
  bool Initialized = false;

  int NcId = -1;
  int MeshVarId = -1;
  int FaceVarId = -1;
  int FaceFillValue = -1;
  int FaceStartIndex = 0;
  int NodeXVarId = -1;
  int NodeYVarId = -1;
  int NodeType = -1;
  std::size_t NodeCount = 0;
  std::size_t FaceCount = 0;
  std::size_t NodesPerFace = 0;
  std::size_t FaceStride = 0;
  std::size_t NodesPerFaceStride = 0;
  std::vector<int> NodeArrayVarIds; // data variables linked to nodes (points)
  std::vector<int> FaceArrayVarIds; // data variables linked to face (cells)
  std::vector<double> TimeSteps;

private:
  vtkNetCDFUGRIDReader(const vtkNetCDFUGRIDReader&) = delete;
  void operator=(const vtkNetCDFUGRIDReader&) = delete;
};

VTK_ABI_NAMESPACE_END

#endif
