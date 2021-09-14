/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkNewEnSightGoldReader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkNewEnSightGoldReader.h"

#include "vtkByteSwap.h"
#include "vtkDataAssembly.h"
#include "vtkDataSetAttributes.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPartitionedDataSet.h"
#include "vtkPartitionedDataSetCollection.h"
#include "vtkPoints.h"
#include "vtkRectilinearGrid.h"
#include "vtkStructuredGrid.h"
#include "vtkUniformGrid.h"
#include "vtkUnsignedCharArray.h"

#include "vtksys/FStream.hxx"
#include "vtksys/SystemTools.hxx"

#include <cctype> /* isspace */
#include <unordered_set>

namespace
{
constexpr int MAX_LINE_LENGTH = 80;
// This is half the precision of an int.
constexpr int MAXIMUM_PART_ID = 65536;

enum class FileType
{
  ASCII,
  CBinary,
  FBinary
};

enum class GridType
{
  Unknown,
  Uniform,
  Rectilinear,
  Curvilinear,
  Unstructured
};

enum class Endianness
{
  Unknown,
  Little,
  Big
};

struct GridOptions
{
  GridType Type = GridType::Unknown;
  bool IBlanked = false;
  bool WithGhost = false;
  bool HasRange = false;
};

void evaluateOption(const char* option, GridOptions& opts)
{
  if (strncmp(option, "curvilinear", 11) == 0)
  {
    opts.Type = GridType::Curvilinear;
  }
  else if (strncmp(option, "rectilinear", 11) == 0)
  {
    opts.Type = GridType::Rectilinear;
  }
  else if (strncmp(option, "uniform", 7) == 0)
  {
    opts.Type = GridType::Uniform;
  }
  else if (strncmp(option, "iblanked", 8) == 0)
  {
    opts.IBlanked = true;
  }
  else if (strncmp(option, "with_ghost", 10) == 0)
  {
    opts.WithGhost = true;
  }
  else if (strncmp(option, "range", 5) == 0)
  {
    opts.HasRange = true;
  }
}

template <typename T>
bool charTo(const char* input, T* output);

template <>
bool charTo(const char* input, int* output)
{
  try
  {
    *output = atoi(input);
    return true;
  }
  catch (...)
  {
    return false;
  }
}

template <>
bool charTo(const char* input, float* output)
{
  try
  {
    *output = atof(input);
    return true;
  }
  catch (...)
  {
    return false;
  }
}

GridOptions getGridOptions(const char* line)
{
  GridOptions opts;
  char block[MAX_LINE_LENGTH];
  char s1[MAX_LINE_LENGTH], s2[MAX_LINE_LENGTH], s3[MAX_LINE_LENGTH], s4[MAX_LINE_LENGTH];
  // format of line
  // block <curvilinear/rectilinear/uniform/''> [iblanked] [with_ghost] [range]
  if (sscanf(line, "%s %s %s %s %s", block, s1, s2, s3, s4) == 1)
  {
    opts.Type = GridType::Curvilinear;
    return opts;
  }
  evaluateOption(s1, opts);
  evaluateOption(s2, opts);
  evaluateOption(s3, opts);
  evaluateOption(s4, opts);

  if (opts.Type == GridType::Unknown)
  {
    opts.Type = GridType::Curvilinear;
  }

  return opts;
}

struct EnSightFile
{
  std::string FileName;
  vtksys::ifstream* Stream;
  FileType Format = FileType::ASCII;
  Endianness ByteOrder = Endianness::Unknown;

  EnSightFile();
  ~EnSightFile();

  bool OpenFile();
  bool ReadNextLine(char result[MAX_LINE_LENGTH]);
  bool ReadLine(char result[MAX_LINE_LENGTH]);
  void SkipNLines(vtkIdType n);
  void GoBackOneLine();
  bool DetectByteOrder(int* result);

  template <typename T>
  bool ReadNumber(T* result);

  template <typename T>
  bool ReadArray(T* result, vtkIdType n);
};

//------------------------------------------------------------------------------
EnSightFile::EnSightFile()
{
  this->Stream = nullptr;
}

//------------------------------------------------------------------------------
EnSightFile::~EnSightFile()
{
  if (this->Stream)
  {
    if (this->Stream->is_open())
    {
      this->Stream->close();
    }
    delete this->Stream;
    this->Stream = nullptr;
  }
}

//------------------------------------------------------------------------------
bool EnSightFile::OpenFile()
{
  this->Stream = new vtksys::ifstream(this->FileName.c_str(), ios::binary);
  if (this->Stream->fail())
  {
    delete this->Stream;
    this->Stream = nullptr;
    return false;
  }

  // read the first line to check the format
  char line[MAX_LINE_LENGTH];
  char subLine1[MAX_LINE_LENGTH], subLine2[MAX_LINE_LENGTH];
  this->ReadLine(line);
  sscanf(line, "%s %s", subLine1, subLine2);
  if (strncmp(subLine2, "Binary", 6) == 0)
  {
    if (strncmp(subLine1, "C", 1) == 0)
    {
      this->Format = FileType::CBinary;
    }
    else if (strncmp(subLine2, "Fortran", 7) == 0)
    {
      this->Format = FileType::FBinary;
    }
    else
    {
      vtkGenericWarningMacro("File type could not be correctly determined");
      return false;
    }
    this->Stream->seekg(MAX_LINE_LENGTH, ios::beg);
  }
  else
  {
    this->Format = FileType::ASCII;
    this->Stream->seekg(0, ios::beg);
  }

  return true;
}

//------------------------------------------------------------------------------
bool EnSightFile::ReadNextLine(char result[MAX_LINE_LENGTH])
{
  if (this->Format != FileType::ASCII)
  {
    return this->ReadLine(result);
  }

  bool isComment = true;
  bool lineRead = true;

  while (isComment && lineRead)
  {
    lineRead = this->ReadLine(result);
    if (*result && result[0] != '#')
    {
      size_t len = strlen(result);
      unsigned int i = 0;
      while (i < len && (static_cast<unsigned int>(result[i]) <= 255) && isspace(result[i]))
      {
        ++i;
      }
      // If there was only space characters this is a comment, thus skip it
      if (i != len)
      {
        // The line was not empty, not beginning by '#' and not composed
        // of only white space, this is not a comment
        isComment = false;
      }
    }
  }
  return lineRead;
}

//------------------------------------------------------------------------------
// Internal function to read in a line up to MAX_LINE_LENGTH characters.
bool EnSightFile::ReadLine(char result[MAX_LINE_LENGTH])
{
  if (this->Format == FileType::ASCII)
  {
    this->Stream->getline(result, MAX_LINE_LENGTH);
  }
  else
  {
    this->Stream->read(result, MAX_LINE_LENGTH);
    result[79] = '\0';
  }
  if (this->Stream->fail())
  {
    // Reset the error flag before returning. This way, we can keep working
    // if we handle the error downstream.
    this->Stream->clear();
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
void EnSightFile::SkipNLines(vtkIdType n)
{
  if (this->Format != FileType::ASCII)
  {
    return;
  }
  char line[MAX_LINE_LENGTH];
  for (vtkIdType i = 0; i < n; i++)
  {
    this->ReadNextLine(line);
  }
}

//------------------------------------------------------------------------------
bool EnSightFile::DetectByteOrder(int* result)
{
  if (this->ByteOrder == Endianness::Unknown)
  {
    int tmpLE = *result;
    int tmpBE = *result;
    vtkByteSwap::Swap4LE(&tmpLE);
    vtkByteSwap::Swap4BE(&tmpBE);

    if (tmpLE >= 0 && tmpLE < MAXIMUM_PART_ID)
    {
      this->ByteOrder = Endianness::Little;
      *result = tmpLE;
      return true;
    }
    if (tmpBE >= 0 && tmpBE < MAXIMUM_PART_ID)
    {
      this->ByteOrder = Endianness::Big;
      *result = tmpBE;
      return true;
    }
    vtkGenericWarningMacro("Byte order could not be determined.");
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
void EnSightFile::GoBackOneLine()
{
  auto pos = this->Stream->tellg();
  pos -= this->Stream->gcount();
  this->Stream->seekg(pos, ios::beg);
}

//------------------------------------------------------------------------------
template <typename T>
bool EnSightFile::ReadNumber(T* result)
{
  if (this->Format == FileType::ASCII)
  {
    char line[MAX_LINE_LENGTH];
    this->ReadNextLine(line);
    charTo(line, result);
  }
  else
  {
    if (!this->Stream->read((char*)result, sizeof(T)))
    {
      vtkGenericWarningMacro("read failed");
      return false;
    }
    if (this->ByteOrder == Endianness::Little)
    {
      vtkByteSwap::Swap4LE(result);
    }
    else if (this->ByteOrder == Endianness::Big)
    {
      vtkByteSwap::Swap4BE(result);
    }
  }
  return true;
}

//------------------------------------------------------------------------------
template <typename T>
bool EnSightFile::ReadArray(T* result, vtkIdType n)
{
  // in the case of ASCII, it's assumed that each value is on a different line
  if (this->Format == FileType::ASCII)
  {
    for (int i = 0; i < n; i++)
    {
      this->ReadNumber(&result[i]);
    }
  }
  else
  {
    if (!this->Stream->read((char*)result, sizeof(T) * n))
    {
      vtkGenericWarningMacro("read array failed");
      return false;
    }
    if (this->ByteOrder == Endianness::Little)
    {
      vtkByteSwap::Swap4LERange(result, n);
    }
    else if (this->ByteOrder == Endianness::Big)
    {
      vtkByteSwap::Swap4BERange(result, n);
    }
  }
  return true;
}

class EnSightFileStream
{
public:
  bool CheckVersion(const char* casefilename);
  bool ParseCaseFile(const char* casefilename);
  bool ReadGeometry(vtkPartitionedDataSetCollection* output);

private:
  void ParseFormatSection();
  void ParseGeometrySection(const char* line);
  void SetGeometryFileName(const char* fname);

  void CreateUniformGridOutput(const GridOptions& opts, vtkUniformGrid* output);
  void CreateRectilinearGridOutput(const GridOptions& opts, vtkRectilinearGrid* output);
  void CreateStructuredGridOutput(const GridOptions& opts, vtkStructuredGrid* output);

  int ReadPartId();
  void ReadDimensions(int dimensions[3]);
  void ReadRange(int range[6]);
  std::vector<int> ReadOptionalValues(int numVals, std::string sectionName = "");

  bool IsSectionHeader(const char* line);

  void ProcessGhostCells(int numCells, vtkDataSet* output);

  EnSightFile CaseFile;
  EnSightFile GeometryFile;

  std::vector<std::string> FilePath;
  bool IsGoldFile = false;

  std::string GeometryFileName;
  bool NodeIdsListed = false;
  bool ElementIdsListed = false;
};

//------------------------------------------------------------------------------
bool EnSightFileStream::CheckVersion(const char* casefilename)
{
  this->CaseFile.FileName = casefilename;
  if (!this->CaseFile.OpenFile())
  {
    return false;
  }

  char result[MAX_LINE_LENGTH];
  while (this->CaseFile.ReadNextLine(result))
  {
    if (strncmp(result, "FORMAT", 6) == 0)
    {
      this->ParseFormatSection();
      break;
    }
  }
  return this->IsGoldFile;
}

//------------------------------------------------------------------------------
bool EnSightFileStream::IsSectionHeader(const char* line)
{
  std::unordered_set<std::string> headerSet = { "FORMAT", "GEOMETRY", "VARIABLE", "TIME", "FILE" };
  auto it = headerSet.find(line);
  return (it == headerSet.end() ? false : true);
}

//------------------------------------------------------------------------------
bool EnSightFileStream::ParseCaseFile(const char* casefilename)
{
  // has 5 sections: FORMAT, GEOMETRY, VARIABLE, TIME, FILE
  this->CaseFile.FileName = casefilename;
  this->CaseFile.OpenFile();
  auto parentDir = vtksys::SystemTools::GetParentDirectory(casefilename);
  vtksys::SystemTools::SplitPath(parentDir, this->FilePath);

  char result[MAX_LINE_LENGTH];
  auto continueRead = this->CaseFile.ReadNextLine(result);
  while (continueRead)
  {
    if (strncmp(result, "FORMAT", 6) == 0)
    {
      this->ParseFormatSection();
      if (!this->IsGoldFile)
      {
        vtkGenericWarningMacro("This reader handles only EnSight Gold files");
        return false;
      }
    }
    else if (strncmp(result, "GEOMETRY", 8) == 0)
    {
      while ((continueRead = this->CaseFile.ReadNextLine(result)))
      {
        if (this->IsSectionHeader(result))
        {
          break;
        }
        this->ParseGeometrySection(result);
      }
      continue;
    }
    else if (strncmp(result, "VARIABLE", 8) == 0)
    {
      vtkGenericWarningMacro("VARIABLE section not yet supported");
      while ((continueRead = this->CaseFile.ReadNextLine(result)))
      {
        if (this->IsSectionHeader(result))
        {
          break;
        }
      }
      continue;
    }
    else if (strncmp(result, "TIME", 4) == 0)
    {
      vtkGenericWarningMacro("TIME section not yet supported");
      while ((continueRead = this->CaseFile.ReadNextLine(result)))
      {
        if (this->IsSectionHeader(result))
        {
          break;
        }
      }
      continue;
    }
    else if (strncmp(result, "FILE", 4) == 0)
    {
      vtkGenericWarningMacro("FILE section not yet supported");
      while ((continueRead = this->CaseFile.ReadNextLine(result)))
      {
        if (this->IsSectionHeader(result))
        {
          break;
        }
      }
      continue;
    }
    else
    {
      vtkGenericWarningMacro("ParseCaseFile: invalid line - " << result);
    }
    continueRead = this->CaseFile.ReadNextLine(result);
  }
  return true;
}

//------------------------------------------------------------------------------
void EnSightFileStream::ParseFormatSection()
{
  char line[MAX_LINE_LENGTH], subLine[MAX_LINE_LENGTH], subLine1[MAX_LINE_LENGTH];
  this->CaseFile.ReadNextLine(line);
  sscanf(line, " %*s %s %s", subLine, subLine1);
  if (strncmp(subLine, "ensight", 7) == 0 && strncmp(subLine1, "gold", 4) == 0)
  {
    this->IsGoldFile = true;
  }
}

//------------------------------------------------------------------------------
void EnSightFileStream::SetGeometryFileName(const char* fname)
{
  this->FilePath.emplace_back(fname);
  this->GeometryFileName = vtksys::SystemTools::JoinPath(this->FilePath);
  this->FilePath.pop_back();
}

//------------------------------------------------------------------------------
void EnSightFileStream::ParseGeometrySection(const char* line)
{
  char subLine[MAX_LINE_LENGTH];
  if (strncmp(line, "model:", 6) == 0)
  {
    int timeSet, fileSet;
    if (sscanf(line, " %*s %d %d%*[ \t]%s", &timeSet, &fileSet, subLine) == 3)
    {
      vtkGenericWarningMacro("timesets and filesets not supported yet.");
      this->SetGeometryFileName(subLine);
    }
    else if (sscanf(line, " %*s %d%*[ \t]%s", &timeSet, subLine) == 2)
    {
      vtkGenericWarningMacro("timesets not supported yet.");
      this->SetGeometryFileName(subLine);
    }
    else if (sscanf(line, " %*s %s", subLine) == 1)
    {
      this->SetGeometryFileName(subLine);
    }
  }
  else if (strncmp(line, "measured:", 9) == 0)
  {
    vtkGenericWarningMacro("measured files not supported yet");
  }
  else if (strncmp(line, "match:", 6) == 0)
  {
    vtkGenericWarningMacro("match files not supported yet");
  }
  else if (strncmp(line, "boundary:", 9) == 0)
  {
    vtkGenericWarningMacro("boundary files not supported yet");
  }
  else
  {
    vtkGenericWarningMacro("ParseGeometrySection: invalid line - " << line);
  }
}

//------------------------------------------------------------------------------
bool EnSightFileStream::ReadGeometry(vtkPartitionedDataSetCollection* output)
{
  this->GeometryFile.FileName = this->GeometryFileName;
  if (!this->GeometryFile.OpenFile())
  {
    return false;
  }

  char line[MAX_LINE_LENGTH], subLine[MAX_LINE_LENGTH];
  // read 2 description lines
  this->GeometryFile.ReadLine(line);
  this->GeometryFile.ReadLine(line);

  // read node id, which can be off/given/assign/ignore
  this->GeometryFile.ReadNextLine(line);
  sscanf(line, " %*s %*s %s", subLine);
  if (strncmp(subLine, "given", 5) == 0 || strncmp(subLine, "ignore", 6) == 0)
  {
    this->NodeIdsListed = true;
  }

  // similarly for element id
  this->GeometryFile.ReadNextLine(line);
  sscanf(line, " %*s %*s %s", subLine);
  if (strncmp(subLine, "given", 5) == 0 || strncmp(subLine, "ignore", 6) == 0)
  {
    this->ElementIdsListed = true;
  }

  // check if extents section is included and handle if so
  auto lineRead = this->GeometryFile.ReadNextLine(line);
  if (strncmp(line, "extents", 7) == 0)
  {
    if (this->GeometryFile.Format == FileType::ASCII)
    {
      // two values per line in ASCII case
      this->GeometryFile.SkipNLines(3);
    }
    else
    {
      float val[6];
      this->GeometryFile.ReadArray(val, 6);
    }
    lineRead = this->GeometryFile.ReadNextLine(line); // "part"
  }

  while (lineRead && strncmp(line, "part", 4) == 0)
  {
    int partId = this->ReadPartId();
    partId--; // EnSight starts counts at 1

    this->GeometryFile.ReadNextLine(line); // part description line
    char* partName = strdup(line);

    this->GeometryFile.ReadNextLine(line);
    auto opts = getGridOptions(line);
    vtkDataSet* grid;
    switch (opts.Type)
    {
      case GridType::Uniform:
        grid = vtkUniformGrid::New();
        this->CreateUniformGridOutput(opts, vtkUniformGrid::SafeDownCast(grid));
        break;
      case GridType::Rectilinear:
        grid = vtkRectilinearGrid::New();
        this->CreateRectilinearGridOutput(opts, vtkRectilinearGrid::SafeDownCast(grid));
        break;
      case GridType::Curvilinear:
        grid = vtkStructuredGrid::New();
        this->CreateStructuredGridOutput(opts, vtkStructuredGrid::SafeDownCast(grid));
        break;
      case GridType::Unstructured:
        vtkGenericWarningMacro("Unstructured grid not supported yet");
        break;
      default:
        vtkGenericWarningMacro("Grid type not correctly specified");
        return false;
    }
    vtkNew<vtkPartitionedDataSet> pds;
    pds->SetPartition(0, grid);
    grid->Delete();
    output->SetPartitionedDataSet(partId, pds);
    output->GetMetaData(partId)->Set(vtkCompositeDataSet::NAME(), partName);

    auto assembly = output->GetDataAssembly();
    auto validName = vtkDataAssembly::MakeValidNodeName(partName);
    auto node = assembly->AddNode(validName.c_str());
    assembly->AddDataSetIndex(node, partId);
  }

  return true;
}

//------------------------------------------------------------------------------
void EnSightFileStream::CreateUniformGridOutput(const GridOptions& opts, vtkUniformGrid* output)
{
  // read dimensions line
  char line[MAX_LINE_LENGTH];
  int dimensions[3];
  this->ReadDimensions(dimensions);
  output->SetDimensions(dimensions);

  if (opts.HasRange)
  {
    // TODO unsure if this is all that's needed to be done
    int range[6];
    this->ReadRange(range);
    // range contains: imin, imax, jmin, jmax, kmin, kmax
    dimensions[0] = range[1] - range[0] + 1;
    dimensions[1] = range[3] - range[2] + 1;
    dimensions[2] = range[5] - range[4] + 1;
  }

  float origin[3];
  this->GeometryFile.ReadArray(origin, 3);
  output->SetOrigin(origin[0], origin[1], origin[2]);

  float delta[3];
  this->GeometryFile.ReadArray(delta, 3);
  output->SetSpacing(delta[0], delta[1], delta[2]);

  auto numPts = dimensions[0] * dimensions[1] * dimensions[2];
  auto numCells = (dimensions[0] - 1) * (dimensions[1] - 1) * (dimensions[2] - 1);
  if (opts.IBlanked)
  {
    auto data = this->ReadOptionalValues(numPts);
    for (int i = 0; i < data.size(); i++)
    {
      if (!data[i])
      {
        output->BlankPoint(i);
      }
    }
  }

  if (opts.WithGhost)
  {
    this->ProcessGhostCells(numCells, output);
  }

  if (this->NodeIdsListed)
  {
    vtkGenericWarningMacro("node ids not supported yet");
    this->ReadOptionalValues(numPts, "node_ids");
  }

  if (this->ElementIdsListed)
  {
    vtkGenericWarningMacro("element ids not supported yet");
    this->ReadOptionalValues(numCells, "element_ids");
  }
}

//------------------------------------------------------------------------------
void EnSightFileStream::CreateRectilinearGridOutput(
  const GridOptions& opts, vtkRectilinearGrid* output)
{
  // read dimensions line
  char line[MAX_LINE_LENGTH];
  int dimensions[3];
  this->ReadDimensions(dimensions);
  output->SetDimensions(dimensions);

  if (opts.HasRange)
  {
    // TODO unsure if this is all that's needed to be done
    int range[6];
    this->ReadRange(range);
    // range contains: imin, imax, jmin, jmax, kmin, kmax
    dimensions[0] = range[1] - range[0] + 1;
    dimensions[1] = range[3] - range[2] + 1;
    dimensions[2] = range[5] - range[4] + 1;
  }

  vtkNew<vtkFloatArray> xCoords;
  vtkNew<vtkFloatArray> yCoords;
  vtkNew<vtkFloatArray> zCoords;
  xCoords->Allocate(dimensions[0]);
  yCoords->Allocate(dimensions[1]);
  zCoords->Allocate(dimensions[2]);

  for (vtkIdType i = 0; i < dimensions[0]; i++)
  {
    float val;
    this->GeometryFile.ReadNumber(&val);
    xCoords->InsertTuple(i, &val);
  }
  for (vtkIdType i = 0; i < dimensions[1]; i++)
  {
    float val;
    this->GeometryFile.ReadNumber(&val);
    yCoords->InsertTuple(i, &val);
  }
  for (vtkIdType i = 0; i < dimensions[2]; i++)
  {
    float val;
    this->GeometryFile.ReadNumber(&val);
    zCoords->InsertTuple(i, &val);
  }
  output->SetXCoordinates(xCoords);
  output->SetYCoordinates(yCoords);
  output->SetZCoordinates(zCoords);

  auto numPts = dimensions[0] * dimensions[1] * dimensions[2];
  auto numCells = (dimensions[0] - 1) * (dimensions[1] - 1) * (dimensions[2] - 1);
  if (opts.IBlanked)
  {
    vtkGenericWarningMacro("iblanked not supported for vtkRectilinearGrid");
    this->ReadOptionalValues(numPts);
  }

  if (opts.WithGhost)
  {
    this->ProcessGhostCells(numCells, output);
  }

  if (this->NodeIdsListed)
  {
    vtkGenericWarningMacro("node ids not supported yet");
    this->ReadOptionalValues(numPts, "node_ids");
  }

  if (this->ElementIdsListed)
  {
    vtkGenericWarningMacro("element ids not supported yet");
    this->ReadOptionalValues(numCells, "element_ids");
  }
}

//------------------------------------------------------------------------------
void EnSightFileStream::CreateStructuredGridOutput(
  const GridOptions& opts, vtkStructuredGrid* output)
{
  // read dimensions line
  char line[MAX_LINE_LENGTH];
  int dimensions[3];
  this->ReadDimensions(dimensions);
  output->SetDimensions(dimensions);

  if (opts.HasRange)
  {
    // TODO unsure if this is all that's needed to be done
    int range[6];
    this->ReadRange(range);
    // range contains: imin, imax, jmin, jmax, kmin, kmax
    dimensions[0] = range[1] - range[0] + 1;
    dimensions[1] = range[3] - range[2] + 1;
    dimensions[2] = range[5] - range[4] + 1;
  }

  int numPts = dimensions[0] * dimensions[1] * dimensions[2];
  vtkNew<vtkPoints> points;
  points->SetNumberOfPoints(numPts);

  for (vtkIdType i = 0; i < numPts; i++)
  {
    float val;
    this->GeometryFile.ReadNumber(&val);
    points->SetPoint(i, val, 0.0, 0.0);
  }
  for (vtkIdType i = 0; i < numPts; i++)
  {
    float val;
    this->GeometryFile.ReadNumber(&val);
    double point[3];
    points->GetPoint(i, point);
    points->SetPoint(i, point[0], val, point[2]);
  }
  for (vtkIdType i = 0; i < numPts; i++)
  {
    float val;
    this->GeometryFile.ReadNumber(&val);
    double point[3];
    points->GetPoint(i, point);
    points->SetPoint(i, point[0], point[1], val);
  }
  output->SetPoints(points);

  auto numCells = (dimensions[0] - 1) * (dimensions[1] - 1) * (dimensions[2] - 1);
  if (opts.IBlanked)
  {
    auto data = this->ReadOptionalValues(numPts);
    for (int i = 0; i < numPts; i++)
    {
      if (!data[i])
      {
        output->BlankPoint(i);
      }
    }
  }

  if (opts.WithGhost)
  {
    this->ProcessGhostCells(numCells, output);
  }

  if (this->NodeIdsListed)
  {
    vtkGenericWarningMacro("node ids not supported yet");
    this->ReadOptionalValues(numPts, "node_ids");
  }

  if (this->ElementIdsListed)
  {
    vtkGenericWarningMacro("element ids not supported yet");
    this->ReadOptionalValues(numCells, "element_ids");
  }
}

//------------------------------------------------------------------------------
int EnSightFileStream::ReadPartId()
{
  int partId;
  this->GeometryFile.ReadNumber(&partId);
  if (this->GeometryFile.Format != FileType::ASCII &&
    this->GeometryFile.ByteOrder == Endianness::Unknown)
  {
    this->GeometryFile.DetectByteOrder(&partId);
  }
  return partId;
}

//------------------------------------------------------------------------------
void EnSightFileStream::ReadDimensions(int dimensions[3])
{
  char line[MAX_LINE_LENGTH];
  if (this->GeometryFile.Format == FileType::ASCII)
  {
    this->GeometryFile.ReadNextLine(line);
    sscanf(line, " %d %d %d", &dimensions[0], &dimensions[1], &dimensions[2]);
  }
  else
  {
    this->GeometryFile.ReadArray(dimensions, 3);
  }
}

//------------------------------------------------------------------------------
void EnSightFileStream::ReadRange(int range[6])
{
  if (this->GeometryFile.Format == FileType::ASCII)
  {
    char line[MAX_LINE_LENGTH];
    this->GeometryFile.ReadNextLine(line);
    sscanf(
      line, "%d %d %d %d %d %d", &range[0], &range[1], &range[2], &range[3], &range[4], &range[5]);
  }
  else
  {
    this->GeometryFile.ReadArray(range, 6);
  }
}

//------------------------------------------------------------------------------
std::vector<int> EnSightFileStream::ReadOptionalValues(int numVals, std::string sectionName)
{
  std::vector<int> data(numVals, 0);
  // some data has an optional string before it. e.g., for ghost flags,
  // there may be a string "ghost_flags" preceeding it
  if (!sectionName.empty())
  {
    char line[MAX_LINE_LENGTH], subLine[MAX_LINE_LENGTH];
    this->GeometryFile.ReadNextLine(line);
    sscanf(line, "%s", subLine);
    if (strncmp(subLine, sectionName.c_str(), 11) != 0)
    {
      this->GeometryFile.GoBackOneLine();
    }
  }

  this->GeometryFile.ReadArray(data.data(), data.size());
  return data;
}

//------------------------------------------------------------------------------
void EnSightFileStream::ProcessGhostCells(int numCells, vtkDataSet* output)
{
  auto ghostFlags = this->ReadOptionalValues(numCells, "ghost_flags");
  vtkUnsignedCharArray* cellGhostArray = output->GetCellGhostArray();
  if (!cellGhostArray)
  {
    cellGhostArray = output->AllocateCellGhostArray();
  }
  for (vtkIdType i = 0; i < static_cast<vtkIdType>(ghostFlags.size()); i++)
  {
    if (ghostFlags[i])
    {
      cellGhostArray->SetValue(i, cellGhostArray->GetValue(i) | vtkDataSetAttributes::HIDDENCELL);
    }
  }
}

} // end anon namespace

struct vtkNewEnSightGoldReader::ReaderImpl
{
  EnSightFileStream FileStream;
};

vtkStandardNewMacro(vtkNewEnSightGoldReader);

//----------------------------------------------------------------------------
vtkNewEnSightGoldReader::vtkNewEnSightGoldReader()
{
  this->SetNumberOfInputPorts(0);
  this->CaseFileName = nullptr;
  this->Impl = new ReaderImpl;
}

//----------------------------------------------------------------------------
vtkNewEnSightGoldReader::~vtkNewEnSightGoldReader()
{
  delete[] this->CaseFileName;
  delete this->Impl;
}

//------------------------------------------------------------------------------
int vtkNewEnSightGoldReader::CanReadFile(const char* casefilename)
{
  return (this->Impl->FileStream.CheckVersion(casefilename) ? 1 : 0);
}

//------------------------------------------------------------------------------
int vtkNewEnSightGoldReader::RequestInformation(
  vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  if (!this->CaseFileName)
  {
    vtkErrorMacro("CaseFileName is null");
    return 0;
  }

  if (!this->Impl->FileStream.ParseCaseFile(this->CaseFileName))
  {
    vtkErrorMacro("Case file " << this->CaseFileName << " could not be parsed without error");
    return 0;
  }
  return 1;
}

//------------------------------------------------------------------------------
int vtkNewEnSightGoldReader::RequestData(
  vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  vtkPartitionedDataSetCollection* output =
    vtkPartitionedDataSetCollection::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkNew<vtkDataAssembly> assembly;
  output->SetDataAssembly(assembly);

  if (!this->Impl->FileStream.ReadGeometry(output))
  {
    vtkErrorMacro("Geometry file could not be read");
    return 0;
  }

  return 1;
}

//------------------------------------------------------------------------------
void vtkNewEnSightGoldReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Case FileName: " << (this->CaseFileName ? this->CaseFileName : "(none)") << "\n";
}
