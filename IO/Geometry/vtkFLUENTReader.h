// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @class   vtkFLUENTReader
 * @brief   reads a dataset in Fluent file format
 *
 * vtkFLUENTReader creates an unstructured grid multiblock dataset.
 * When multiple zones are defined in the file they are provided in separate blocks.
 * It reads .cas (with associated .dat) and .msh files stored in FLUENT native format.
 *
 * @par Thanks:
 * Thanks to Brian W. Dotson & Terry E. Jordan (Department of Energy, National
 * Energy Technology Laboratory) & Douglas McCorkle (Iowa State University)
 * who developed this class.
 * Please address all comments to Brian Dotson (brian.dotson@netl.doe.gov) &
 * Terry Jordan (terry.jordan@sa.netl.doe.gov)
 * & Doug McCorkle (mccdo@iastate.edu)
 *
 *
 * @sa
 * vtkGAMBITReader
 */

#ifndef vtkFLUENTReader_h
#define vtkFLUENTReader_h

#include "vtkIOGeometryModule.h" // For export macro
#include "vtkMultiBlockDataSetAlgorithm.h"
#include "vtkNew.h" // For vtkNew

VTK_ABI_NAMESPACE_BEGIN
class vtkDataArraySelection;
class vtkPoints;
class vtkTriangle;
class vtkTetra;
class vtkQuad;
class vtkHexahedron;
class vtkPyramid;
class vtkWedge;
class vtkConvexPointSet;

class VTKIOGEOMETRY_EXPORT vtkFLUENTReader : public vtkMultiBlockDataSetAlgorithm
{
public:
  static vtkFLUENTReader* New();
  vtkTypeMacro(vtkFLUENTReader, vtkMultiBlockDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Specify the file name of the Fluent case file to read.
   */
  vtkSetFilePathMacro(FileName);
  vtkGetFilePathMacro(FileName);
  ///@}

  ///@{
  /**
   * Get the total number of cells. The number of cells is only valid after a
   * successful read of the data file is performed. Initial value is 0.
   */
  vtkGetMacro(NumberOfCells, vtkIdType);
  ///@}

  /**
   * Get the number of cell arrays available in the input.
   */
  int GetNumberOfCellArrays();

  /**
   * Get the name of the cell array with the given index in
   * the input.
   */
  const char* GetCellArrayName(int index);

  ///@{
  /**
   * Get/Set whether the cell array with the given name is to
   * be read.
   */
  int GetCellArrayStatus(const char* name);
  void SetCellArrayStatus(const char* name, int status);
  ///@}

  ///@{
  /**
   * Turn on/off all cell arrays.
   */
  void DisableAllCellArrays();
  void EnableAllCellArrays();
  ///@}

  ///@{
  /**
   * These methods should be used instead of the SwapBytes methods.
   * They indicate the byte ordering of the file you are trying
   * to read in. These methods will then either swap or not swap
   * the bytes depending on the byte ordering of the machine it is
   * being run on. For example, reading in a BigEndian file on a
   * BigEndian machine will result in no swapping. Trying to read
   * the same file on a LittleEndian machine will result in swapping.
   * As a quick note most UNIX machines are BigEndian while PC's
   * and VAX tend to be LittleEndian. So if the file you are reading
   * in was generated on a VAX or PC, SetDataByteOrderToLittleEndian
   * otherwise SetDataByteOrderToBigEndian. Not used when reading
   * text files.
   */
  void SetDataByteOrderToBigEndian();
  void SetDataByteOrderToLittleEndian();
  int GetDataByteOrder();
  void SetDataByteOrder(int);
  const char* GetDataByteOrderAsString();
  //
  //  Structures
  //
  struct Cell;
  struct Face;
  struct Zone;
  struct ScalarDataChunk;
  struct VectorDataChunk;
  struct SubSection;
  ///@}

protected:
  vtkFLUENTReader();
  ~vtkFLUENTReader() override;
  int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  ///@{
  /**
   * Set/Get the byte swapping to explicitly swap the bytes of a file.
   * Not used when reading text files.
   */
  vtkSetMacro(SwapBytes, vtkTypeBool);
  vtkTypeBool GetSwapBytes() { return this->SwapBytes; }
  vtkBooleanMacro(SwapBytes, vtkTypeBool);
  ///@}

  virtual bool OpenCaseFile(const char* filename);
  virtual bool OpenDataFile(const char* filename);
  virtual void LoadVariableNames();
  virtual bool ParseCaseFile();
  virtual void GetPartitionInfo() {}
  virtual void CleanCells();
  virtual void PopulateCellNodes();
  virtual void PopulateTriangleCell(size_t cellIdx);
  virtual void PopulateTetraCell(size_t cellIdx);
  virtual void PopulateQuadCell(size_t cellIdx);
  virtual void PopulateHexahedronCell(size_t cellIdx);
  virtual void PopulatePyramidCell(size_t cellIdx);
  virtual void PopulateWedgeCell(size_t cellIdx);
  virtual void PopulatePolyhedronCell(size_t cellIdx);
  virtual void ParseDataFile();
  virtual bool ParallelCheckCell(int vtkNotUsed(i)) { return true; }

private:
  /**
   * @brief Create an output multi block dataset using only the faces of the file
   *
   * This function is used to generate an output when reading a FLUENT Mesh file
   * that only contains faces without cells.
   * It supports triangles and quads.
   *
   * @param output Multiblock to be filled with faces information
   */
  void FillMultiBlockFromFaces(vtkMultiBlockDataSet* output);

  vtkFLUENTReader(const vtkFLUENTReader&) = delete;
  void operator=(const vtkFLUENTReader&) = delete;

  void ReadLittleEndianFlag(const std::string& chunkBuffer);
  void ReadSpeciesVariableNames(const std::string& chunkBuffer);
  void ReadNodesAscii(const std::string& chunkBuffer);
  void ReadNodesSinglePrecision(const std::string& chunkBuffer);
  void ReadNodesDoublePrecision(const std::string& chunkBuffer);
  void ReadCellsAscii(const std::string& chunkBuffer);
  void ReadCellsBinary(const std::string& chunkBuffer);
  void ReadZone(const std::string& chunkBuffer);
  bool ReadFacesAscii(const std::string& chunkBuffer);
  void ReadFacesBinary(const std::string& chunkBuffer);
  void ReadPeriodicShadowFacesAscii(const std::string& chunkBuffer);
  void ReadPeriodicShadowFacesBinary(const std::string& chunkBuffer);
  void ReadCellTreeAscii(const std::string& chunkBuffer);
  void ReadCellTreeBinary(const std::string& chunkBuffer);
  void ReadFaceTreeAscii(const std::string& chunkBuffer);
  void ReadFaceTreeBinary(const std::string& chunkBuffer);
  void ReadInterfaceFaceParentsAscii(const std::string& chunkBuffer);
  void ReadInterfaceFaceParentsBinary(const std::string& chunkBuffer);
  void ReadNonconformalGridInterfaceFaceInformationAscii(const std::string& chunkBuffer);
  void ReadNonconformalGridInterfaceFaceInformationBinary(const std::string& chunkBuffer);
  void ReadData(const std::string& dataBuffer, int dataType);

  //
  //  Variables
  //
  vtkNew<vtkDataArraySelection> CellDataArraySelection;
  char* FileName = nullptr;
  vtkIdType NumberOfCells = 0;

  istream* FluentCaseFile = nullptr;
  istream* FluentDataFile = nullptr;

  // File data cache
  vtkNew<vtkPoints> Points;
  std::vector<Cell> Cells;
  std::vector<Face> Faces;
  std::vector<Zone> Zones;
  std::map<size_t, std::string> VariableNames;
  std::vector<ScalarDataChunk> ScalarDataChunks;
  std::vector<VectorDataChunk> VectorDataChunks;
  std::vector<SubSection> SubSections;

  std::vector<std::string> ScalarVariableNames;
  std::vector<int> ScalarSubSectionIds;
  std::vector<std::string> VectorVariableNames;
  std::vector<int> VectorSubSectionIds;

  vtkTypeBool SwapBytes;
  int GridDimension = 0;
  int NumberOfScalars = 0;
  int NumberOfVectors = 0;
  bool Parsed = false;
};

VTK_ABI_NAMESPACE_END
#endif
